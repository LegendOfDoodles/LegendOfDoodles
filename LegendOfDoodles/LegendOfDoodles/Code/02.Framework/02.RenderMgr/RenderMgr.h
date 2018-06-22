#pragma once
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"
#include "04.Shaders/97.TextureToFullScreenShader/TextureToFullScreenShader.h"

class CScene;

class CRenderMgr
{
public:	// 생성자, 소멸자
	CRenderMgr();
	~CRenderMgr();

public:	// 공개 함수
	void Initialize(int width, int height);
	void Release();

	void Render(CScene* pScene = NULL);
	void RenderColor(CScene* pScene = NULL);
	void RenderLight(CScene* pScene = NULL);

	// Set Depth Stencil View
	void SetDsvDescriptorHeap(ID3D12DescriptorHeap *pDsvDescriptorHeap);

	// Set Swap Chain
	void SetSwapChain(IDXGISwapChain3 *pSwapChain) { m_pSwapChain = pSwapChain; }
	void SetSwapChainBufferIndex(UINT swapChainBufferIndex) { m_swapChainBufferIndex = swapChainBufferIndex; }

	// Set Render Target View
	void SetRenderTargetBuffers(ID3D12Resource **ppRenderTargetBuffers) { m_ppRenderTargetBuffers = ppRenderTargetBuffers; }
	void SetSwapChainBackBuffers(ID3D12Resource **ppd3dSwapChainBackBuffers) { m_ppSwapChainBackBuffers = ppd3dSwapChainBackBuffers; }
	void SetRtvDescriptorHeap(ID3D12DescriptorHeap *pRtvDescriptorHeap) { m_pRtvDescriptorHeap = pRtvDescriptorHeap; }
	void SetRtvDescriptorIncrementSize(UINT rtvDescriptorIncrementSize) { m_rtvDescriptorIncrementSize = rtvDescriptorIncrementSize; }
	void SetRtvRenderTargetBufferCPUHandles(D3D12_CPU_DESCRIPTOR_HANDLE* pRtvRenderTargetBufferCPUHandles) { m_pRtvRenderTargetBufferCPUHandles = pRtvRenderTargetBufferCPUHandles; }
	void SetRtvSwapChainBackBufferCPUHandles(D3D12_CPU_DESCRIPTOR_HANDLE *pRtvSwapChainBackBufferCPUHandles) { m_pRtvSwapChainBackBufferCPUHandles = pRtvSwapChainBackBufferCPUHandles; }

	// Set Command Queue
	void SetCommandQueue(ID3D12CommandQueue *pCommandQueue) { m_pCommandQueue = pCommandQueue; }
	void SetCommandAllocator(ID3D12CommandAllocator *pCommandAllocator) { m_pCommandAllocator = pCommandAllocator; }
	void SetCommandList(ID3D12GraphicsCommandList *pCommandList) { m_pCommandList = pCommandList; }

	// Set Fence
	void SetFence(ID3D12Fence *pFence) { m_pFence = pFence; }

	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }

	// Set GraphicsRootSignature
	void SaveGraphicsRootSignature(ID3D12RootSignature *pGraphicsRootSignature) { m_pGraphicsRootSignature = pGraphicsRootSignature; }
	void SetTextureToFullScreenShader(CTextureToFullScreenShader* pTTFSShader) { m_pTextureToFullScreenShader = pTTFSShader; }

	// Synchronize
	void WaitForGpuComplete();
	void MoveToNextFrame();

	void ResetCommandList();
	void ExecuteCommandList();

private:	// 내부 함수
	void SynchronizeResourceTransition(ID3D12Resource *pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);

private:	// 변수
	// Swap Chain
	IDXGISwapChain3 * m_pSwapChain{ NULL };
	UINT m_swapChainBufferIndex{ 0 };

	// Render Target View
	ID3D12Resource **m_ppRenderTargetBuffers{ NULL };
	ID3D12DescriptorHeap *m_pRtvDescriptorHeap{ NULL };
	UINT m_rtvDescriptorIncrementSize{ 0 };
	D3D12_CPU_DESCRIPTOR_HANDLE	*m_pRtvRenderTargetBufferCPUHandles{ NULL };

	ID3D12Resource	**m_ppSwapChainBackBuffers{ NULL };
	D3D12_CPU_DESCRIPTOR_HANDLE		*m_pRtvSwapChainBackBufferCPUHandles{ NULL };

	// Depth Stencil View
	ID3D12DescriptorHeap *m_pDsvDescriptorHeap{ NULL };
	D3D12_CPU_DESCRIPTOR_HANDLE		m_dsvDepthStencilBufferCPUHandle;

	// Command Queue
	ID3D12CommandQueue *m_pCommandQueue{ NULL };
	ID3D12CommandAllocator *m_pCommandAllocator{ NULL };
	ID3D12GraphicsCommandList *m_pCommandList{ NULL };

	// Fence
	ID3D12Fence *m_pFence{ NULL };
	UINT64 m_fenceValues[SWAP_CHAIN_BUFFER_CNT];
	HANDLE m_hFenceEvent{ NULL };

	CCamera							*m_pCamera{ NULL };

	CTextureToFullScreenShader *m_pTextureToFullScreenShader{ NULL };

	// Root Signature
	ID3D12RootSignature *m_pGraphicsRootSignature{ NULL };
};

