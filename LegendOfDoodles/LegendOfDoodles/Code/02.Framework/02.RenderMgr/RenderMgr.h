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
	void Initialize();
	void Release();

	void Render(shared_ptr<CScene> pScene);
	void RenderDepth(shared_ptr<CScene> pScene);
	void RenderColor(shared_ptr<CScene> pScene);
	void RenderLight(shared_ptr<CScene> pScene);

	void RenderLoadingScreen(float loadingPercentage = 0.f);

	// Set dsvCPUHandle by dsvDescriptorHeap
	void SetDsvCPUHandleWithDsvHeap(ComPtr<ID3D12DescriptorHeap> pDsvDescriptorHeap, UINT incrementSize);

	// Synchronize
	void WaitForGpuComplete();
	void MoveToNextFrame();

	void ResetCommandList();
	void ExecuteCommandList();

	// Set Swap Chain
	void SetSwapChain(ComPtr<IDXGISwapChain3> pSwapChain) { m_pSwapChain = pSwapChain; }
	void SetSwapChainBufferIndex(UINT swapChainBufferIndex) { m_swapChainBufferIndex = swapChainBufferIndex; }

	// Set Render Target View
	void SetRenderTargetBuffers(ComPtr<ID3D12Resource>* ppRenderTargetBuffers) { m_ppRenderTargetBuffers = ppRenderTargetBuffers; }
	void SetSwapChainBackBuffers(ComPtr<ID3D12Resource>* ppd3dSwapChainBackBuffers) { m_ppSwapChainBackBuffers = ppd3dSwapChainBackBuffers; }
	void SetRtvRenderTargetBufferCPUHandles(D3D12_CPU_DESCRIPTOR_HANDLE* pRtvRenderTargetBufferCPUHandles) { m_pRtvRenderTargetBufferCPUHandles = pRtvRenderTargetBufferCPUHandles; }
	void SetRtvSwapChainBackBufferCPUHandles(D3D12_CPU_DESCRIPTOR_HANDLE *pRtvSwapChainBackBufferCPUHandles) { m_pRtvSwapChainBackBufferCPUHandles = pRtvSwapChainBackBufferCPUHandles; }

	// Set Command Queue
	void SetCommandQueue(ComPtr<ID3D12CommandQueue> pCommandQueue) { m_pCommandQueue = pCommandQueue; }
	void SetCommandAllocator(ComPtr<ID3D12CommandAllocator> pCommandAllocator) { m_pCommandAllocator = pCommandAllocator; }
	void SetCommandList(ComPtr<ID3D12GraphicsCommandList> pCommandList) { m_pCommandList = pCommandList; }

	// Set Fence
	void SetFence(ComPtr<ID3D12Fence> pFence) { m_pFence = pFence; }

	// Set GraphicsRootSignature
	void SaveGraphicsRootSignature(ComPtr<ID3D12RootSignature> pGraphicsRootSignature) { m_pGraphicsRootSignature = pGraphicsRootSignature; }
	void SetTextureToFullScreenShader(shared_ptr<CTextureToFullScreenShader> pTTFSShader) { m_pTextureToFullScreenShader = pTTFSShader; }

	// Set ShadowMap Texture
	void SetShadowDepthBuffer(ComPtr<ID3D12Resource> pShadowDepthBuffer) { m_pShadowDepthBuffer = pShadowDepthBuffer; }

	void SetLoadingScene(shared_ptr<CScene> pLoadingScene) { m_pLoadingScene = pLoadingScene; }

private:	// 내부 함수
	void SynchronizeResourceTransition(ComPtr<ID3D12Resource> pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);

private:	// 변수
			// Swap Chain
	ComPtr<IDXGISwapChain3> m_pSwapChain;
	UINT m_swapChainBufferIndex{ 0 };

	// Command Queue
	ComPtr<ID3D12CommandQueue> m_pCommandQueue;
	ComPtr<ID3D12CommandAllocator> m_pCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_pCommandList;

	// Fence
	ComPtr<ID3D12Fence> m_pFence;
	UINT64 m_fenceValues[SWAP_CHAIN_BUFFER_CNT];
	HANDLE m_hFenceEvent{ NULL };

	// Buffers
	ComPtr<ID3D12Resource> *m_ppRenderTargetBuffers{ NULL };
	ComPtr<ID3D12Resource> *m_ppSwapChainBackBuffers{ NULL };
	ComPtr<ID3D12Resource> m_pShadowDepthBuffer;

	// Render Target View
	D3D12_CPU_DESCRIPTOR_HANDLE	*m_pRtvRenderTargetBufferCPUHandles{ NULL };
	D3D12_CPU_DESCRIPTOR_HANDLE	*m_pRtvSwapChainBackBufferCPUHandles{ NULL };

	// Depth Stencil View
	D3D12_CPU_DESCRIPTOR_HANDLE	m_dsvDepthStencilBufferCPUHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE	m_dsvShadowBufferCPUHandle;

	shared_ptr<CTextureToFullScreenShader> m_pTextureToFullScreenShader;

	// Root Signature
	ComPtr<ID3D12RootSignature> m_pGraphicsRootSignature;

	// Loading Scene
	shared_ptr<CScene> m_pLoadingScene;
};

