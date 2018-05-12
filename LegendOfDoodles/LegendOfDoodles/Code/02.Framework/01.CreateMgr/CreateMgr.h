#pragma once
#include "02.Framework/02.RenderMgr/RenderMgr.h"

class CCreateMgr
{
public:	// 생성자, 소멸자
	CCreateMgr();
	~CCreateMgr();

public:	// 공개 함수
	void Initialize(HINSTANCE hInstance, HWND hWnd);
	void Release();

	void Resize(int width, int height);

	void OnResizeBackBuffers();
	void ChangeScreenMode();

	ID3D12Resource *CreateBufferResource(
		void *pData, UINT nBytes, 
		D3D12_HEAP_TYPE d3dHeapType = D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		ID3D12Resource **ppd3dUploadBuffer = NULL);

	ID3D12Resource *CreateTextureResourceFromFile(
		wchar_t *pszFileName, 
		ID3D12Resource **ppUploadBuffer, 
		D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	void ResetCommandList();
	void ExecuteCommandList();

	CRenderMgr* GetRenderMgr() { return &m_renderMgr; }

	ID3D12Device* GetDevice() { return m_pDevice; }
	ID3D12GraphicsCommandList* GetCommandList() { return m_pCommandList; }

	ID3D12RootSignature* GetGraphicsRootSignature() { return m_pGraphicsRootSignature; }

	HWND GetHwnd() { return m_hWnd; }

	int GetWindowWidth() { return m_wndClientWidth; }
	int GetWindowHeight() { return m_wndClientHeight; }

	UINT GetCbvSrvDescriptorIncrementSize() { return m_cbvSrvDescriptorIncrementSize; }

private:	
	DXGI_SWAP_CHAIN_DESC CreateSwapChainDesc();

	void CreateDirect3dDevice();
	void CreateCommandQueueAndList();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateDepthStencilView();
	void CreateRenderTargetView();
	void CreateGraphicsRootSignature();

private:	 // 변수
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	// Window Size
	int m_wndClientWidth{ FRAME_BUFFER_WIDTH };
	int m_wndClientHeight{ FRAME_BUFFER_HEIGHT };

	// Factory and Device
	IDXGIFactory4 *m_pFactory{ NULL };
	ID3D12Device *m_pDevice{ NULL };

	// MSAA Set
	bool m_msaa4xEnable{ false };
	UINT m_msaa4xQualityLevels{ 0 };

	// Swap Chain
	IDXGISwapChain3 *m_pSwapChain{ NULL };

	// Render Target View
	ID3D12Resource *m_ppRenderTargetBuffers[SWAP_CHAIN_BUFFER_CNT]{ NULL };
	ID3D12DescriptorHeap *m_pRtvDescriptorHeap{ NULL };
	UINT m_rtvDescriptorIncrementSize{ 0 };

	// Depth Stencil View
	ID3D12Resource *m_pDepthStencilBuffer{ NULL };
	ID3D12DescriptorHeap *m_pDsvDescriptorHeap{ NULL };

	// Command Queue
	ID3D12CommandQueue *m_pCommandQueue{ NULL };
	ID3D12CommandAllocator *m_pCommandAllocator{ NULL };
	ID3D12GraphicsCommandList *m_pCommandList{ NULL };

	// Fence
	ID3D12Fence *m_pFence{ NULL };

#ifdef _DEBUG
	ID3D12Debug *m_pDebugController{ NULL };
#endif

	UINT m_cbvSrvDescriptorIncrementSize{ 0 };

	// Root Signature
	ID3D12RootSignature *m_pGraphicsRootSignature{ NULL };

	// Render Manager
	CRenderMgr m_renderMgr;
};