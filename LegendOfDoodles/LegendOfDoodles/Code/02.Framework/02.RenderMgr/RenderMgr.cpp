#include "stdafx.h"
#include "RenderMgr.h"
#include "03.Scenes/00.BaseScene/Scene.h"

/// <summary>
/// ����: ������ ���� �Լ��� ��� �ξ� �ٸ� ������� ���� �׸� �� �ֵ��� �ϱ� ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-08-06
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
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
// ���� �Լ�
void CRenderMgr::Initialize()
{
	// Fence
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_fenceValues[0] = 1;
}

void CRenderMgr::Release()
{
	::CloseHandle(m_hFenceEvent);
}

void CRenderMgr::Render(shared_ptr<CScene> pScene)
{
	RenderDepth(pScene);
	RenderColor(pScene);
	RenderLight(pScene);

	MoveToNextFrame();
}

void CRenderMgr::RenderDepth(shared_ptr<CScene> pScene)
{
	HRESULT hResult;
	// Reset Command List
	hResult = m_pCommandAllocator->Reset();
	ThrowIfFailed(hResult);

	hResult = m_pCommandList->Reset(m_pCommandAllocator.Get(), NULL);
	ThrowIfFailed(hResult);

	// Change to DEPTH_WRITE
	SynchronizeResourceTransition(m_pShadowDepthBuffer, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);

	// Set Viewport and Scissor Rect
	pScene->SetShadowViewportsAndScissorRects();

	// Clear Depth Stencil View
	m_pCommandList->ClearDepthStencilView(m_dsvShadowBufferCPUHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	// Set Render Target and Depth Stencil
	m_pCommandList->OMSetRenderTargets(0, NULL, FALSE, &m_dsvShadowBufferCPUHandle);

	m_pCommandList->SetGraphicsRootSignature(m_pGraphicsRootSignature.Get());

	// Set Camera Matrices
	pScene->UpdateShadowCamera();

	// Render Scene
	pScene->RenderShadow();

	// Change back to GENERIC_READ to read it by texture
	SynchronizeResourceTransition(m_pShadowDepthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
}

void CRenderMgr::RenderColor(shared_ptr<CScene> pScene)
{
	// Set Barrier
	for (int i = 0; i < RENDER_TARGET_BUFFER_CNT; ++i)
	{
		SynchronizeResourceTransition(m_ppRenderTargetBuffers[i], D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	// Set Viewport and Scissor Rect
	pScene->SetViewportsAndScissorRects();

	// Clear Render Target View
	static float pClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
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

	m_pCommandList->SetGraphicsRootSignature(m_pGraphicsRootSignature.Get());

	// Update Camera
	pScene->UpdateCamera();

	// Render Scene
	pScene->Render();

	// Set Barrier
	for (int i = 0; i < RENDER_TARGET_BUFFER_CNT; ++i)
	{
		SynchronizeResourceTransition(m_ppRenderTargetBuffers[i], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);
	}

	ExecuteCommandList();
}

void CRenderMgr::RenderLight(shared_ptr<CScene> pScene)
{
	HRESULT hResult;

	WaitForGpuComplete();

	hResult = m_pCommandAllocator->Reset();
	ThrowIfFailed(hResult);

	hResult = m_pCommandList->Reset(m_pCommandAllocator.Get(), NULL);
	ThrowIfFailed(hResult);

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

	pScene->UpdateShadowCamera(2);

	m_pTextureToFullScreenShader->Render(NULL);

	SynchronizeResourceTransition(m_ppSwapChainBackBuffers[m_swapChainBufferIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	ExecuteCommandList();

	// Present
	hResult = m_pSwapChain->Present(0, 0);
	ThrowIfFailed(hResult);
}

void CRenderMgr::SetDsvCPUHandleWithDsvHeap(ComPtr<ID3D12DescriptorHeap> pDsvDescriptorHeap, UINT incrementSize)
{
	m_dsvDepthStencilBufferCPUHandle = pDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_dsvShadowBufferCPUHandle.ptr = m_dsvDepthStencilBufferCPUHandle.ptr + incrementSize;
}

void CRenderMgr::WaitForGpuComplete()
{
	HRESULT hResult;
	++m_fenceValues[m_swapChainBufferIndex];
	UINT64 fenceValue = m_fenceValues[m_swapChainBufferIndex];

	hResult = m_pCommandQueue->Signal(m_pFence.Get(), fenceValue);
	ThrowIfFailed(hResult);

	//GPU�� �潺�� ���� �����ϴ� ����� ��� ť�� �߰��Ѵ�.
	if (m_pFence->GetCompletedValue() < fenceValue)
	{
		//�潺�� ���� ���� ������ ������ ������ �潺�� ���� ���� ������ ���� �� ������ ��ٸ���.
		hResult = m_pFence->SetEventOnCompletion(fenceValue, m_hFenceEvent);
		ThrowIfFailed(hResult);

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
	m_pCommandList->Reset(m_pCommandAllocator.Get(), NULL);
}

void CRenderMgr::ExecuteCommandList()
{
	HRESULT hResult;

	hResult = m_pCommandList->Close();
	ThrowIfFailed(hResult);

	ID3D12CommandList *ppCommandLists[] = { m_pCommandList.Get() };
	m_pCommandQueue->ExecuteCommandLists(1, ppCommandLists);

	WaitForGpuComplete();
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CRenderMgr::SynchronizeResourceTransition(ComPtr<ID3D12Resource> pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter)
{
	D3D12_RESOURCE_BARRIER resourceBarrier;
	resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resourceBarrier.Transition.pResource = pResource.Get();
	resourceBarrier.Transition.StateBefore = stateBefore;
	resourceBarrier.Transition.StateAfter = stateAfter;
	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pCommandList->ResourceBarrier(1, &resourceBarrier);
}