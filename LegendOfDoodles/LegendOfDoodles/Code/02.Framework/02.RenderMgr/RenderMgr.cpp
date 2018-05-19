#include "stdafx.h"
#include "RenderMgr.h"
#include "../../03.Scenes/00.BaseScene/Scene.h"

/// <summary>
/// 목적: 렌더링 관련 함수를 모아 두어 다른 변경사항 없이 그릴 수 있도록 하기 위함
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-05-19
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CRenderMgr::CRenderMgr()
{
	for (int i = 0; i < RENDER_TARGET_BUFFER_CNT; i++)
	{
		m_fenceValues[i] = 0;
	}
}

CRenderMgr::~CRenderMgr()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CRenderMgr::Initialize(int width, int height)
{
	// Fence
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_fenceValues[0] = 1;
}

void CRenderMgr::Release()
{
	::CloseHandle(m_hFenceEvent);
}

void CRenderMgr::Render(CScene* pScene)
{
	HRESULT hResult;
	// Reset Command List
	hResult = m_pCommandAllocator->Reset();
	assert(SUCCEEDED(hResult) && "CommandAllocator->Reset Failed");

	hResult = m_pCommandList->Reset(m_pCommandAllocator, NULL);
	assert(SUCCEEDED(hResult) && "CommandList->Reset Failed");

	// Set Barrier
	for (int i = 0; i < RENDER_TARGET_BUFFER_CNT; ++i)
	{
		SynchronizeResourceTransition(m_ppRenderTargetBuffers[i], D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	// Set Viewport and Scissor Rect
	pScene->SetViewportsAndScissorRects();

	// Clear Render Target View
	float pClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	for (int i = 0; i < RENDER_TARGET_BUFFER_CNT; ++i)
	{
		m_pCommandList->ClearRenderTargetView(m_pRtvRenderTargetBufferCPUHandles[i], pClearColor, 0, NULL);
	}

	// Clear Depth Stencil View
	m_pCommandList->ClearDepthStencilView(m_dsvDepthStencilBufferCPUHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	// Set Render Target and Depth Stencil
	m_pCommandList->OMSetRenderTargets(RENDER_TARGET_BUFFER_CNT, m_pRtvRenderTargetBufferCPUHandles,
		TRUE, &m_dsvDepthStencilBufferCPUHandle);

	m_pCommandList->SetGraphicsRootSignature(m_pGraphicsRootSignature);

	// Update Camera
	pScene->UpdateCamera();

	// Render Scene
	pScene->Render();

	// Close Command List
	hResult = m_pCommandList->Close();
	assert(SUCCEEDED(hResult) && "CommandList->Close Failed");

	// Excute Command List
	ID3D12CommandList *ppCommandLists[] = { m_pCommandList };
	m_pCommandQueue->ExecuteCommandLists(1, ppCommandLists);

	WaitForGpuComplete();

	//------------------------------------------------------
	// ↓↓↓↓↓↓ Light Rendering ↓↓↓↓↓↓↓

	hResult = m_pCommandAllocator->Reset();
	assert(SUCCEEDED(hResult) && "CommandAllocator->Reset Failed");

	hResult = m_pCommandList->Reset(m_pCommandAllocator, NULL);
	assert(SUCCEEDED(hResult) && "CommandList->Reset Failed");

	// Set Barrier
	for (int i = 0; i < RENDER_TARGET_BUFFER_CNT; ++i)
	{
		SynchronizeResourceTransition(m_ppRenderTargetBuffers[i], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);
	}

	SynchronizeResourceTransition(m_ppSwapChainBackBuffers[m_swapChainBufferIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// Set Viewport and Scissor Rect
	pScene->SetViewportsAndScissorRects();

	m_pCommandList->ClearDepthStencilView(m_dsvDepthStencilBufferCPUHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	m_pCommandList->ClearRenderTargetView(m_pRtvSwapChainBackBufferCPUHandles[m_swapChainBufferIndex], Colors::Azure, 0, NULL);
	m_pCommandList->OMSetRenderTargets(1, &m_pRtvSwapChainBackBufferCPUHandles[m_swapChainBufferIndex],
		TRUE, &m_dsvDepthStencilBufferCPUHandle);

	m_pTextureToFullScreenShader->SetGraphicsRootSignature();

	pScene->RenderWithLights();
	m_pTextureToFullScreenShader->Render(m_pCamera);

	SynchronizeResourceTransition(m_ppSwapChainBackBuffers[m_swapChainBufferIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// Close Command List
	hResult = m_pCommandList->Close();
	assert(SUCCEEDED(hResult) && "CommandList->Close Failed");

	// Excute Command List
	m_pCommandQueue->ExecuteCommandLists(1, ppCommandLists);

	WaitForGpuComplete();

	// Present
	hResult = m_pSwapChain->Present(0, 0);
	assert(SUCCEEDED(hResult) && "SwapChain->Present Failed");
	//ExptProcess::ThrowIfFailed(hResult);

	MoveToNextFrame();
}

void CRenderMgr::SetDsvDescriptorHeap(ID3D12DescriptorHeap * pDsvDescriptorHeap)
{
	m_pDsvDescriptorHeap = pDsvDescriptorHeap;
	m_dsvDepthStencilBufferCPUHandle = m_pDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
}

void CRenderMgr::WaitForGpuComplete()
{
	HRESULT hResult;
	UINT64 fenceValue = ++m_fenceValues[m_swapChainBufferIndex];

	hResult = m_pCommandQueue->Signal(m_pFence, fenceValue);
	assert(SUCCEEDED(hResult) && "CommandQueue->Signal Failed");

	//GPU가 펜스의 값을 설정하는 명령을 명령 큐에 추가한다.
	if (m_pFence->GetCompletedValue() < fenceValue)
	{
		//펜스의 현재 값이 설정한 값보다 작으면 펜스의 현재 값이 설정한 값이 될 때까지 기다린다.
		hResult = m_pFence->SetEventOnCompletion(fenceValue, m_hFenceEvent);
		assert(SUCCEEDED(hResult) && "SetEventOnCompletion Failed");

		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CRenderMgr::MoveToNextFrame()
{
	// Get Next Back Buffer Index
	m_swapChainBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	WaitForGpuComplete();
}

void CRenderMgr::ResetCommandList()
{
	m_pCommandList->Reset(m_pCommandAllocator, NULL);
}

void CRenderMgr::ExecuteCommandList()
{
	HRESULT hResult;

	hResult = m_pCommandList->Close();
	assert(SUCCEEDED(hResult) && "CommandList->Close Failed");

	ID3D12CommandList *ppCommandLists[] = { m_pCommandList };
	m_pCommandQueue->ExecuteCommandLists(1, ppCommandLists);

	WaitForGpuComplete();
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CRenderMgr::SynchronizeResourceTransition(ID3D12Resource * pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter)
{
	D3D12_RESOURCE_BARRIER resourceBarrier;
	resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resourceBarrier.Transition.pResource = pResource;
	resourceBarrier.Transition.StateBefore = stateBefore;
	resourceBarrier.Transition.StateAfter = stateAfter;
	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pCommandList->ResourceBarrier(1, &resourceBarrier);
}