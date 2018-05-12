#pragma once

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

	// Set Swap Chain
	void SetSwapChain(IDXGISwapChain3 *pSwapChain) { m_pSwapChain = pSwapChain; }
	void SetSwapChainBufferIndex(UINT swapChainBufferIndex) { m_swapChainBufferIndex = swapChainBufferIndex; }

	// Set Render Target View
	void SetRenderTargetBuffers(ID3D12Resource *ppRenderTargetBuffers[]);
	void SetRtvDescriptorHeap(ID3D12DescriptorHeap *pRtvDescriptorHeap) { m_pRtvDescriptorHeap = pRtvDescriptorHeap; }
	void SetRtvDescriptorIncrementSize(UINT rtvDescriptorIncrementSize) { m_rtvDescriptorIncrementSize = rtvDescriptorIncrementSize;  }

	// Set Depth Stencil View
	void SetDsvDescriptorHeap(ID3D12DescriptorHeap *pDsvDescriptorHeap) { m_pDsvDescriptorHeap = pDsvDescriptorHeap; }

	// Set Command Queue
	void SetCommandQueue(ID3D12CommandQueue *pCommandQueue) { m_pCommandQueue = pCommandQueue; }
	void SetCommandAllocator(ID3D12CommandAllocator *pCommandAllocator) { m_pCommandAllocator = pCommandAllocator; }
	void SetCommandList(ID3D12GraphicsCommandList *pCommandList) { m_pCommandList = pCommandList; }

	// Set Fence
	void SetFence(ID3D12Fence *pFence) { m_pFence = pFence; }

	// Set GraphicsRootSignature
	void SetGraphicsRootSignature(ID3D12RootSignature *pGraphicsRootSignature) { m_pGraphicsRootSignature = pGraphicsRootSignature; }

	// Synchronize
	void WaitForGpuComplete();
	void MoveToNextFrame();

	void ResetCommandList();
	void ExecuteCommandList();

private:	// 내부 함수
	//D3D12_RESOURCE_BARRIER CreateResourceBarrier();
	//void CRenderMgr::SetResourceBarrier(D3D12_RESOURCE_BARRIER &resourceBarrier,
	//	D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

private:	// 변수
	// Swap Chain
	IDXGISwapChain3 *m_pSwapChain{ NULL };
	UINT m_swapChainBufferIndex{ 0 };

	// Render Target View
	ID3D12Resource *m_ppRenderTargetBuffers[SWAP_CHAIN_BUFFER_CNT]{ NULL };
	ID3D12DescriptorHeap *m_pRtvDescriptorHeap{ NULL };
	UINT m_rtvDescriptorIncrementSize{ 0 };

	// Depth Stencil View
	ID3D12DescriptorHeap *m_pDsvDescriptorHeap{ NULL };

	// Command Queue
	ID3D12CommandQueue *m_pCommandQueue{ NULL };
	ID3D12CommandAllocator *m_pCommandAllocator{ NULL };
	ID3D12GraphicsCommandList *m_pCommandList{ NULL };

	// Fence
	ID3D12Fence *m_pFence{ NULL };
	UINT64 m_fenceValues[SWAP_CHAIN_BUFFER_CNT]{ 0 };
	HANDLE m_hFenceEvent{ NULL };

	// Root Signature
	ID3D12RootSignature *m_pGraphicsRootSignature{ NULL };
};

