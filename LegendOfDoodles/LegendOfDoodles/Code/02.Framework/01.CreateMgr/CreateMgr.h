#pragma once
#include "02.Framework/02.RenderMgr/RenderMgr.h"
#include "04.Shaders/97.TextureToFullScreenShader/TextureToFullScreenShader.h"
#include "05.Objects/95.ShadowMap/ShadowMap.h"

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
		D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		bool bIsCubeMap = false);

	ID3D12Resource* CreateTexture2DResource(
		UINT nWidth, UINT nHeight,
		DXGI_FORMAT dxgiFormat,
		D3D12_RESOURCE_FLAGS resourceFlags,
		D3D12_RESOURCE_STATES resourceStates,
		D3D12_CLEAR_VALUE *pClearValue);

	void ResetCommandList();
	void ExecuteCommandList();

	CRenderMgr* GetRenderMgr() { return &m_renderMgr; }

	ID3D12Device* GetDevice() { return m_pDevice; }
	ID3D12GraphicsCommandList* GetCommandList() { return m_pCommandList; }

	ID3D12RootSignature* GetGraphicsRootSignature() { return m_pGraphicsRootSignature; }

	HWND GetHwnd() { return m_hWnd; }

	int GetWindowWidth() { return m_nWndClientWidth; }
	int GetWindowHeight() { return m_nWndClientHeight; }

	UINT GetCbvSrvDescriptorIncrementSize() { return m_cbvSrvDescriptorIncrementSize; }

private:
	void CreateDirect3dDevice();
	void CreateCommandQueueAndList();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateSwapChainRenderTargetViews();
	void CreateDepthStencilView();
	void CreateRenderTargetViews();
	void CreateGraphicsRootSignature();

private:	 // 변수
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	// Window Size
	int m_nWndClientWidth{ FRAME_BUFFER_WIDTH };
	int m_nWndClientHeight{ FRAME_BUFFER_HEIGHT };

	// Factory and Device
	IDXGIFactory4 *m_pFactory{ NULL };
	ID3D12Device *m_pDevice{ NULL };

	// Swap Chain
	IDXGISwapChain3 *m_pSwapChain{ NULL };

	// MSAA Set
	bool m_bMsaa4xEnable{ false };
	UINT m_nMsaa4xQualityLevels{ 0 };

	// Render Target View
	ID3D12Resource *m_ppRenderTargetBuffers[RENDER_TARGET_BUFFER_CNT];
	D3D12_CPU_DESCRIPTOR_HANDLE m_pRtvRenderTargetBufferCPUHandles[RENDER_TARGET_BUFFER_CNT];

	ID3D12Resource					*m_ppSwapChainBackBuffers[SWAP_CHAIN_BUFFER_CNT];
	ID3D12DescriptorHeap *m_pRtvDescriptorHeap{ NULL };
	UINT m_nRtvDescriptorIncrementSize{ 0 };
	D3D12_CPU_DESCRIPTOR_HANDLE		m_pRtvSwapChainBackBufferCPUHandles[SWAP_CHAIN_BUFFER_CNT];

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

	CTextureToFullScreenShader *m_pTextureToFullScreenShader{ NULL };

	// Root Signature
	ID3D12RootSignature *m_pGraphicsRootSignature{ NULL };

	// Render Manager
	CRenderMgr m_renderMgr;

	// Shadow Map
	CShadowMap *m_pShadowMap{ NULL };
};