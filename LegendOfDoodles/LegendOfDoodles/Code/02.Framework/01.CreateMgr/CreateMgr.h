#pragma once
#include "02.Framework/02.RenderMgr/RenderMgr.h"
#include "05.Objects/99.Material/00.Texture/Texture.h"
#include "04.Shaders/97.TextureToFullScreenShader/TextureToFullScreenShader.h"

class CCreateMgr : public std::enable_shared_from_this<CCreateMgr>
{
public:	// 생성자, 소멸자
	CCreateMgr();
	~CCreateMgr();

public:	// 공개 함수
	void Initialize(HINSTANCE hInstance, HWND hWnd);
	void Release();

	void RenderLoadingScreen(float loadingPercentage = 0.f);

	void Resize(int width, int height);

	void OnResizeBackBuffers();
	void ChangeScreenMode();

	ComPtr<ID3D12Resource> CreateBufferResource(
		void *pData, UINT nBytes,
		D3D12_HEAP_TYPE d3dHeapType = D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		ID3D12Resource **ppUploadBuffer = NULL);

	ComPtr<ID3D12Resource> CreateTextureResourceFromFile(
		wchar_t *pszFileName,
		ID3D12Resource **ppUploadBuffer,
		D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		bool bIsCubeMap = false);

	ComPtr<ID3D12Resource> CreateTexture2DResource(
		UINT nWidth, UINT nHeight,
		DXGI_FORMAT dxgiFormat,
		D3D12_RESOURCE_FLAGS resourceFlags,
		D3D12_RESOURCE_STATES resourceStates,
		D3D12_CLEAR_VALUE *pClearValue);

	void ResetCommandList();
	void ExecuteCommandList();

	shared_ptr<CRenderMgr> GetRenderMgr() { return m_pRenderMgr; }

	ComPtr<ID3D12Device> GetDevice() { return m_pDevice; }
	ComPtr<ID3D12GraphicsCommandList> GetCommandList() { return m_pCommandList; }

	ComPtr<ID3D12RootSignature> GetGraphicsRootSignature() { return m_pGraphicsRootSignature; }

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
	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	void CreatePostprocessShader();
	void CreateGraphicsRootSignature();

private:	 // 변수
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	// Window Size
	int m_nWndClientWidth{ FRAME_BUFFER_WIDTH };
	int m_nWndClientHeight{ FRAME_BUFFER_HEIGHT };

	// Factory and Device
	ComPtr<IDXGIFactory4> m_pFactory;
	ComPtr<ID3D12Device> m_pDevice;

	// Swap Chain
	ComPtr<IDXGISwapChain3> m_pSwapChain;

	// BackBuffer Format
	DXGI_FORMAT m_backBufferFormat{ DXGI_FORMAT_R8G8B8A8_UNORM };
	DXGI_FORMAT m_renderBufferFormat{ DXGI_FORMAT_R32G32B32A32_FLOAT };

	// MSAA Set
	bool m_bMsaa4xEnable{ false };
	UINT m_nMsaa4xQualityLevels{ 0 };

	// Descriptor Heaps
	ComPtr<ID3D12DescriptorHeap> m_pRtvDescriptorHeap;
	ComPtr<ID3D12DescriptorHeap> m_pDsvDescriptorHeap;

	// RTV / DSV 버퍼 생성용
	shared_ptr<CTexture> m_pTexture;

	// Resources
	ComPtr<ID3D12Resource> m_ppRenderTargetBuffers[RENDER_TARGET_BUFFER_CNT];
	ComPtr<ID3D12Resource> m_ppSwapChainBackBuffers[SWAP_CHAIN_BUFFER_CNT];
	ComPtr<ID3D12Resource> m_pDepthStencilBuffer;
	ComPtr<ID3D12Resource> m_pShadowDepthBuffer;

	D3D12_CPU_DESCRIPTOR_HANDLE m_pRtvRenderTargetBufferCPUHandles[RENDER_TARGET_BUFFER_CNT];
	D3D12_CPU_DESCRIPTOR_HANDLE	m_pRtvSwapChainBackBufferCPUHandles[SWAP_CHAIN_BUFFER_CNT];

	// Command Queue
	ComPtr<ID3D12CommandQueue> m_pCommandQueue;
	ComPtr<ID3D12CommandAllocator> m_pCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_pCommandList;

	// Fence
	ComPtr<ID3D12Fence> m_pFence;

	UINT m_rtvDescriptorIncrementSize{ 0 };
	UINT m_cbvSrvDescriptorIncrementSize{ 0 };
	UINT m_dsvDescriptorIncrementSize{ 0 };

	// PostProcessing Shader
	shared_ptr<CTextureToFullScreenShader> m_pTextureToFullScreenShader;

	// Root Signature
	ComPtr<ID3D12RootSignature> m_pGraphicsRootSignature;

	// Render Manager
	shared_ptr<CRenderMgr> m_pRenderMgr;
};