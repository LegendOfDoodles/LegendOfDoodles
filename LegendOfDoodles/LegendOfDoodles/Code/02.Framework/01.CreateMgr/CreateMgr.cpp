#include "stdafx.h"
#include "CreateMgr.h"
#include "00.Global/01.Utility/02.TextureLoader/DDSTextureLoader12.h"

/// <summary>
/// 목적: 생성 관련 함수를 모아 두어 헷갈리는 일 없이 생성 가능하도록 함
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-09-15
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CCreateMgr::CCreateMgr()
{
}

CCreateMgr::~CCreateMgr()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CCreateMgr::Initialize(HINSTANCE hInstance, HWND hWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hWnd;

	m_pRenderMgr = shared_ptr<CRenderMgr>(new CRenderMgr);

	CreateDirect3dDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	CreateGraphicsRootSignature();

	m_pRenderMgr->Initialize();
}

void CCreateMgr::Release()
{
	HRESULT hResult;

	hResult = m_pSwapChain->SetFullscreenState(FALSE, NULL);
	ThrowIfFailed(hResult);

	// Render Manager
	m_pRenderMgr->Release();

	m_pCommandQueue.Reset();
	m_pCommandAllocator.Reset();
	m_pCommandList.Reset();

	m_pRtvDescriptorHeap.Reset();
	m_pDsvDescriptorHeap.Reset();

	m_pDepthStencilBuffer.Reset();
	m_pShadowDepthBuffer.Reset();

	for (int i = 0; i < RENDER_TARGET_BUFFER_CNT; ++i)
	{
		m_ppRenderTargetBuffers[i].Reset();
	}

	for (int i = 0; i < SWAP_CHAIN_BUFFER_CNT; ++i)
	{
		m_ppSwapChainBackBuffers[i].Reset();
	}

	m_pGraphicsRootSignature.Reset();

	m_pFence.Reset();

	m_pSwapChain.Reset();
	m_pDevice.Reset();
	m_pFactory.Reset();
}

void CCreateMgr::RenderLoadingScreen(float loadingPercentage)
{
	m_pRenderMgr->RenderLoadingScreen(loadingPercentage);
}

void CCreateMgr::Resize(int width, int height)
{
	m_nWndClientWidth = width;
	m_nWndClientHeight = height;

	OnResizeBackBuffers();
}

void CCreateMgr::OnResizeBackBuffers()
{
	HRESULT hResult;

	ResetCommandList();

	m_pTextureToFullScreenShader.reset();
	for (int i = 0; i < RENDER_TARGET_BUFFER_CNT; i++)
	{
		m_ppRenderTargetBuffers[i].Reset();
	}
	for (int i = 0; i < SWAP_CHAIN_BUFFER_CNT; i++)
	{
		m_ppSwapChainBackBuffers[i].Reset();
	}
	if (m_pDepthStencilBuffer) { m_pDepthStencilBuffer.Reset(); }
	if (m_pShadowDepthBuffer) { m_pShadowDepthBuffer.Reset(); }

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	hResult = m_pSwapChain->GetDesc(&swapChainDesc);
	ThrowIfFailed(hResult);

	hResult = m_pSwapChain->ResizeBuffers(SWAP_CHAIN_BUFFER_CNT, m_nWndClientWidth,
		m_nWndClientHeight, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags);
	ThrowIfFailed(hResult);

	m_pRenderMgr->SetSwapChainBufferIndex(0);

	CreateSwapChainRenderTargetViews();
	CreateRenderTargetViews();
	CreateDepthStencilView();
	CreatePostprocessShader();

	ExecuteCommandList();
}

void CCreateMgr::ChangeScreenMode()
{
	HRESULT hResult;
	BOOL fullScreenState = FALSE;

	hResult = m_pSwapChain->GetFullscreenState(&fullScreenState, NULL);
	ThrowIfFailed(hResult);

	if (!fullScreenState)
	{
		DXGI_MODE_DESC targetParameters;
		targetParameters.Format = m_backBufferFormat;
		targetParameters.Width = m_nWndClientWidth;
		targetParameters.Height = m_nWndClientHeight;
		targetParameters.RefreshRate.Numerator = 60;
		targetParameters.RefreshRate.Denominator = 1;
		targetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		targetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

		hResult = m_pSwapChain->ResizeTarget(&targetParameters);
		ThrowIfFailed(hResult);
	}
	hResult = m_pSwapChain->SetFullscreenState(!fullScreenState, NULL);
	ThrowIfFailed(hResult);

	m_pRenderMgr->WaitForGpuComplete();
	OnResizeBackBuffers();
	m_pRenderMgr->WaitForGpuComplete();
}

D3D12_HEAP_PROPERTIES CreateBufferHeapProperties(D3D12_HEAP_TYPE heapType)
{
	D3D12_HEAP_PROPERTIES heapPropertiesDesc;
	::ZeroMemory(&heapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	heapPropertiesDesc.Type = heapType;
	heapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapPropertiesDesc.CreationNodeMask = 1;
	heapPropertiesDesc.VisibleNodeMask = 1;

	return(heapPropertiesDesc);
}

D3D12_RESOURCE_DESC CreateBufferResourceDesc(UINT nBytes)
{
	D3D12_RESOURCE_DESC resourceDesc;
	::ZeroMemory(&resourceDesc, sizeof(D3D12_RESOURCE_DESC));
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = nBytes;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	return(resourceDesc);
}

D3D12_RESOURCE_STATES CreateBufferInitialStates(D3D12_HEAP_TYPE heapType)
{
	if (heapType == D3D12_HEAP_TYPE_UPLOAD)
		return D3D12_RESOURCE_STATE_GENERIC_READ;

	if (heapType == D3D12_HEAP_TYPE_READBACK)
		return D3D12_RESOURCE_STATE_COPY_DEST;

	return D3D12_RESOURCE_STATE_COPY_DEST;
}

ComPtr<ID3D12Resource> CCreateMgr::CreateBufferResource(
	void *pData, UINT nBytes, D3D12_HEAP_TYPE heapType,
	D3D12_RESOURCE_STATES resourceStates, ID3D12Resource **ppUploadBuffer)
{
	HRESULT hResult;
	ComPtr<ID3D12Resource> pBuffer;

	D3D12_HEAP_PROPERTIES heapProperties{ CreateBufferHeapProperties(heapType) };
	D3D12_RESOURCE_DESC	resourceDesc{ CreateBufferResourceDesc(nBytes) };

	hResult = m_pDevice->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		CreateBufferInitialStates(heapType),
		NULL,
		IID_PPV_ARGS(pBuffer.GetAddressOf()));
	ThrowIfFailed(hResult);

	if (pData)
	{
		switch (heapType)
		{
		case D3D12_HEAP_TYPE_DEFAULT:
		{
			if (ppUploadBuffer)
			{
				D3D12_HEAP_PROPERTIES uploadHeapProperties{ CreateBufferHeapProperties(D3D12_HEAP_TYPE_UPLOAD) };
				//업로드 버퍼를 생성한다.
				hResult = m_pDevice->CreateCommittedResource(&uploadHeapProperties,
					D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL,
					IID_PPV_ARGS(ppUploadBuffer));
				ThrowIfFailed(hResult);

				//업로드 버퍼를 매핑하여 초기화 데이터를 업로드 버퍼에 복사한다.
				D3D12_RANGE readRange = { 0, 0 };
				UINT8 *pBufferDataBegin = NULL;
				hResult = (*ppUploadBuffer)->Map(0, &readRange, (void **)&pBufferDataBegin);
				ThrowIfFailed(hResult);

				memcpy(pBufferDataBegin, pData, nBytes);
				(*ppUploadBuffer)->Unmap(0, NULL);

				//업로드 버퍼의 내용을 디폴트 버퍼에 복사한다.
				m_pCommandList->CopyResource(pBuffer.Get(), *ppUploadBuffer);

				D3D12_RESOURCE_BARRIER barrierSet{ CreateResourceBarrier(pBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, resourceStates) };
				m_pCommandList->ResourceBarrier(1, &barrierSet);
			}
			break;
		}
		case D3D12_HEAP_TYPE_UPLOAD:
		{
			D3D12_RANGE readRange = { 0, 0 };
			UINT8 *pBufferDataBegin = NULL;
			hResult = pBuffer->Map(0, &readRange, (void **)&pBufferDataBegin);
			ThrowIfFailed(hResult);

			memcpy(pBufferDataBegin, pData, nBytes);
			pBuffer->Unmap(0, NULL);
			break;
		}
		case D3D12_HEAP_TYPE_READBACK:
			break;
		}
	}
	return(pBuffer);
}

ComPtr<ID3D12Resource> CCreateMgr::CreateTextureResourceFromFile(
	wchar_t *pszFileName,
	ID3D12Resource **ppUploadBuffer,
	D3D12_RESOURCE_STATES resourceStates,
	bool bIsCubeMap)
{
	ComPtr<ID3D12Resource> pTexture;
	std::unique_ptr<uint8_t[]> ddsData;
	std::vector<D3D12_SUBRESOURCE_DATA> vSubresources;
	DDS_ALPHA_MODE ddsAlphaMode = DDS_ALPHA_MODE_UNKNOWN;

	HRESULT hResult = DirectX::LoadDDSTextureFromFileEx(
		m_pDevice.Get(),
		pszFileName,
		0,
		D3D12_RESOURCE_FLAG_NONE,
		DDS_LOADER_DEFAULT,
		pTexture.GetAddressOf(),
		ddsData,
		vSubresources,
		&ddsAlphaMode,
		&bIsCubeMap);
	ThrowIfFailed(hResult);

	D3D12_HEAP_PROPERTIES heapPropertiesDesc;
	::ZeroMemory(&heapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	heapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapPropertiesDesc.CreationNodeMask = 1;
	heapPropertiesDesc.VisibleNodeMask = 1;

	UINT nSubResources = (UINT)vSubresources.size();
	UINT64 nBytes = GetRequiredIntermediateSize(pTexture.Get(), 0, nSubResources);

	D3D12_RESOURCE_DESC resourceDesc;
	::ZeroMemory(&resourceDesc, sizeof(D3D12_RESOURCE_DESC));
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = nBytes;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	hResult = m_pDevice->CreateCommittedResource(
		&heapPropertiesDesc,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL,
		IID_PPV_ARGS(ppUploadBuffer));
	ThrowIfFailed(hResult);

	::UpdateSubresources(m_pCommandList.Get(), pTexture.Get(), *ppUploadBuffer, 0, 0, nSubResources, &vSubresources[0]);

	D3D12_RESOURCE_BARRIER barrierSet{ CreateResourceBarrier(pTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, resourceStates) };
	m_pCommandList->ResourceBarrier(1, &barrierSet);

	return(pTexture);
}

ComPtr<ID3D12Resource> CCreateMgr::CreateTexture2DResource(
	UINT nWidth, UINT nHeight,
	DXGI_FORMAT format,
	D3D12_RESOURCE_FLAGS resourceFlags,
	D3D12_RESOURCE_STATES resourceStates,
	D3D12_CLEAR_VALUE * pClearValue)
{
	ComPtr<ID3D12Resource> pTexture;

	D3D12_HEAP_PROPERTIES heapPropertiesDesc;
	::ZeroMemory(&heapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	heapPropertiesDesc.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapPropertiesDesc.CreationNodeMask = 1;
	heapPropertiesDesc.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC textureResourceDesc;
	::ZeroMemory(&textureResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Alignment = 0;
	textureResourceDesc.Width = nWidth;
	textureResourceDesc.Height = nHeight;
	textureResourceDesc.DepthOrArraySize = 1;
	textureResourceDesc.MipLevels = 1;
	textureResourceDesc.Format = format;
	textureResourceDesc.SampleDesc.Count = 1;
	textureResourceDesc.SampleDesc.Quality = 0;
	textureResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	textureResourceDesc.Flags = resourceFlags;

	HRESULT hResult = m_pDevice->CreateCommittedResource(
		&heapPropertiesDesc,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		resourceStates,
		pClearValue,
		IID_PPV_ARGS(pTexture.GetAddressOf()));
	ThrowIfFailed(hResult);

	return(pTexture);
}

void CCreateMgr::ResetCommandList()
{
	m_pRenderMgr->WaitForGpuComplete();

	HRESULT hResult = m_pCommandList->Reset(m_pCommandAllocator.Get(), NULL);
	ThrowIfFailed(hResult);
}

void CCreateMgr::ExecuteCommandList()
{
	HRESULT hResult = m_pCommandList->Close();
	ThrowIfFailed(hResult);

	ID3D12CommandList *ppCommandLists[] = { m_pCommandList.Get() };
	m_pCommandQueue->ExecuteCommandLists(1, ppCommandLists);

	m_pRenderMgr->WaitForGpuComplete();
}


////////////////////////////////////////////////////////////////////////
// 내부 함수
void CCreateMgr::CreateDirect3dDevice()
{
	HRESULT hResult;
	DWORD debugFactoryFlags{ NULL };

#if defined(_DEBUG) || defined(USE_DEBUG_CONTROLLER)
	ComPtr<ID3D12Debug> pDebugController;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(pDebugController.GetAddressOf())));
	pDebugController->EnableDebugLayer();

	ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
	ThrowIfFailed(DXGIGetDebugInterface1(0, IID_PPV_ARGS(pDxgiInfoQueue.GetAddressOf())));
	debugFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
	pDxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
	pDxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
#endif

	hResult = ::CreateDXGIFactory2(debugFactoryFlags, IID_PPV_ARGS(m_pFactory.ReleaseAndGetAddressOf()));
	ThrowIfFailed(hResult);

	const char factoryName[]{ "m_pFactory" };
	hResult = m_pFactory->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(factoryName), factoryName);
	ThrowIfFailed(hResult);

	//모든 하드웨어 어댑터 대하여 특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성한다.
	ComPtr<IDXGIAdapter1> pAdapter;
	for (UINT i = 0;
		DXGI_ERROR_NOT_FOUND != m_pFactory->EnumAdapters1(i, pAdapter.GetAddressOf());
		i++)
	{
		DXGI_ADAPTER_DESC1 adapterDesc;
		pAdapter->GetDesc1(&adapterDesc);

		if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED((hResult = D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_12_0,
			IID_PPV_ARGS(m_pDevice.GetAddressOf()))))) break;
	}

	//특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성할 수 없으면 WARP 디바이스를 생성한다.
	if (FAILED(hResult))
	{
		hResult = m_pFactory->EnumWarpAdapter(IID_PPV_ARGS(pAdapter.GetAddressOf()));
		ThrowIfFailed(hResult);
		hResult = D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(m_pDevice.GetAddressOf()));
		ThrowIfFailed(hResult);
	}

	// MSAA Check
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = m_backBufferFormat;
	d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x 다중 샘플링
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	hResult = m_pDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	ThrowIfFailed(hResult);

	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;

	// MSAA 
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;

	// Fence
	hResult = m_pDevice->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(m_pFence.GetAddressOf()));
	ThrowIfFailed(hResult);

	hResult = m_pFence->SetName(L"m_pFence");
	ThrowIfFailed(hResult);

	m_pRenderMgr->SetFence(m_pFence);

	// Save Descriptors Increment Size
	m_rtvDescriptorIncrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_cbvSrvDescriptorIncrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_dsvDescriptorIncrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	hResult = m_pDevice->SetName(L"m_pDevice");
	ThrowIfFailed(hResult);
}

void CCreateMgr::CreateCommandQueueAndList()
{
	HRESULT hResult;

	// Create Queue
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
	::ZeroMemory(&commandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	hResult = m_pDevice->CreateCommandQueue(
		&commandQueueDesc,
		IID_PPV_ARGS(m_pCommandQueue.GetAddressOf()));
	ThrowIfFailed(hResult);

	hResult = m_pCommandQueue->SetName(L"m_pCommandQueue");
	ThrowIfFailed(hResult);

	m_pRenderMgr->SetCommandQueue(m_pCommandQueue);

	// Create Allocator
	hResult = m_pDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(m_pCommandAllocator.GetAddressOf()));
	ThrowIfFailed(hResult);

	hResult = m_pCommandAllocator->SetName(L"m_pCommandAllocator");
	ThrowIfFailed(hResult);

	m_pRenderMgr->SetCommandAllocator(m_pCommandAllocator);

	// Create Command List
	hResult = m_pDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_pCommandAllocator.Get(),
		NULL,
		IID_PPV_ARGS(m_pCommandList.GetAddressOf()));
	ThrowIfFailed(hResult);

	hResult = m_pCommandList->SetName(L"m_pCommandList");
	ThrowIfFailed(hResult);

	m_pRenderMgr->SetCommandList(m_pCommandList);

	hResult = m_pCommandList->Close();
	ThrowIfFailed(hResult);
}

void CCreateMgr::CreateSwapChain()
{
	HRESULT hResult;
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

	// Create Swap Chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	::ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferCount = SWAP_CHAIN_BUFFER_CNT;
	swapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	swapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	swapChainDesc.BufferDesc.Format = m_backBufferFormat;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.OutputWindow = m_hWnd;
	swapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	swapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	swapChainDesc.Windowed = TRUE;

#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
	swapChainDesc.Flags = 0;
#else
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#endif

	hResult = m_pFactory->CreateSwapChain(
		m_pCommandQueue.Get(),
		&swapChainDesc,
		(IDXGISwapChain **)m_pSwapChain.GetAddressOf());
	ThrowIfFailed(hResult);

	m_pRenderMgr->SetSwapChain(m_pSwapChain);

	//“Alt+Enter” 키의 동작을 비활성화한다.
	hResult = m_pFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	ThrowIfFailed(hResult);

	//스왑체인의 현재 후면버퍼 인덱스를 저장한다.
	m_pRenderMgr->SetSwapChainBufferIndex(m_pSwapChain->GetCurrentBackBufferIndex());

	const char swapChainName[]{ "m_pSwapChain" };
	hResult = m_pSwapChain->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(swapChainName), swapChainName);
	ThrowIfFailed(hResult);
}

void CCreateMgr::CreateRtvAndDsvDescriptorHeaps()
{
	HRESULT hResult;

	// Create Render Target View Descriptor Heap
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc;
	::ZeroMemory(&descriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	descriptorHeapDesc.NumDescriptors = SWAP_CHAIN_BUFFER_CNT + RENDER_TARGET_BUFFER_CNT;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptorHeapDesc.NodeMask = 0;

	hResult = m_pDevice->CreateDescriptorHeap(
		&descriptorHeapDesc,
		IID_PPV_ARGS(m_pRtvDescriptorHeap.GetAddressOf()));
	ThrowIfFailed(hResult);

	hResult = m_pRtvDescriptorHeap->SetName(L"m_pRtvDescriptorHeap");
	ThrowIfFailed(hResult);

	// Create Depth Stencil View Descriptor Heap
	descriptorHeapDesc.NumDescriptors = 2;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	hResult = m_pDevice->CreateDescriptorHeap(
		&descriptorHeapDesc,
		IID_PPV_ARGS(m_pDsvDescriptorHeap.GetAddressOf()));
	ThrowIfFailed(hResult);

	hResult = m_pDsvDescriptorHeap->SetName(L"m_pDsvDescriptorHeap");
	ThrowIfFailed(hResult);
}

void CCreateMgr::CreateSwapChainRenderTargetViews()
{
	HRESULT hResult;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUDescriptorHandle = m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = m_backBufferFormat;
	renderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	renderTargetViewDesc.Texture2D.PlaneSlice = 0;

	for (UINT i = 0; i <SWAP_CHAIN_BUFFER_CNT; ++i)
	{
		m_pRtvSwapChainBackBufferCPUHandles[i] = rtvCPUDescriptorHandle;
		hResult = m_pSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void **)&m_ppSwapChainBackBuffers[i]);
		ThrowIfFailed(hResult);
		m_pDevice->CreateRenderTargetView(m_ppSwapChainBackBuffers[i].Get(), &renderTargetViewDesc, m_pRtvSwapChainBackBufferCPUHandles[i]);
		rtvCPUDescriptorHandle.ptr += m_rtvDescriptorIncrementSize;
		hResult = m_ppSwapChainBackBuffers[i]->SetName(L"m_ppSwapChainBackBuffers");
		ThrowIfFailed(hResult);
	}
	m_pRenderMgr->SetSwapChainBackBuffers(m_ppSwapChainBackBuffers);
	m_pRenderMgr->SetRtvSwapChainBackBufferCPUHandles(m_pRtvSwapChainBackBufferCPUHandles);
}

void CCreateMgr::CreateDepthStencilView()
{
	HRESULT hResult;

	D3D12_HEAP_PROPERTIES heapProperties;
	::ZeroMemory(&heapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CreationNodeMask = 1;
	heapProperties.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC resourceDesc;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = m_nWndClientWidth;
	resourceDesc.Height = m_nWndClientHeight;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	resourceDesc.SampleDesc.Quality =
		(m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;

	// Create Depth Stencil Buffer
	hResult = m_pDevice->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(m_pDepthStencilBuffer.GetAddressOf()));
	ThrowIfFailed(hResult);

	hResult = m_pDepthStencilBuffer->SetName(L"m_pDepthStencilBuffer");
	ThrowIfFailed(hResult);

	// Create Depth Stencil View
	D3D12_CPU_DESCRIPTOR_HANDLE dsvCPUDescriptorHandle =
		m_pDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pDevice->CreateDepthStencilView(
		m_pDepthStencilBuffer.Get(),
		NULL,
		dsvCPUDescriptorHandle);

	// Create Depth Stencil Buffer For ShadowMap
	m_pShadowDepthBuffer = m_pTexture->CreateTexture(
		shared_from_this(),
		SHADOW_MAP_SIZE,
		SHADOW_MAP_SIZE,
		DXGI_FORMAT_R24G8_TYPELESS,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&clearValue, RENDER_TARGET_BUFFER_CNT);

	hResult = m_pShadowDepthBuffer->SetName(L"m_pShadowDepthBuffer");
	ThrowIfFailed(hResult);

	// Depth DSV 설정
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE dsvShadowCPUDescriptorHandle{ m_pDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	dsvShadowCPUDescriptorHandle.ptr = dsvShadowCPUDescriptorHandle.ptr + m_dsvDescriptorIncrementSize;
	m_pDevice->CreateDepthStencilView(
		m_pShadowDepthBuffer.Get(),
		&dsvDesc,
		dsvShadowCPUDescriptorHandle);

	m_pRenderMgr->SetDsvCPUHandleWithDsvHeap(m_pDsvDescriptorHeap, m_dsvDescriptorIncrementSize);
	m_pRenderMgr->SetShadowDepthBuffer(m_pShadowDepthBuffer);
}

void CCreateMgr::CreateRenderTargetViews()
{
	HRESULT hResult;
	m_pTexture.reset();
	m_pTexture = shared_ptr<CTexture>(new CTexture(RENDER_TARGET_BUFFER_CNT + 1, RESOURCE_TEXTURE_2D_ARRAY, 0));

	D3D12_CLEAR_VALUE clearValue = { m_renderBufferFormat,{ 0.0f, 0.0f, 0.0f, 1.0f } };
	for (UINT i = 0; i < RENDER_TARGET_BUFFER_CNT; i++)
	{
		m_ppRenderTargetBuffers[i] = m_pTexture->CreateTexture(
			shared_from_this(),
			m_nWndClientWidth,
			m_nWndClientHeight,
			m_renderBufferFormat,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			&clearValue, i);
		hResult = m_ppRenderTargetBuffers[i]->SetName(L"m_ppRenderTargetBuffers");
		ThrowIfFailed(hResult);
	}
	m_pRenderMgr->SetRenderTargetBuffers(m_ppRenderTargetBuffers);

	// 렌더 타겟 SRV설정
	D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUDescriptorHandle = m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	rtvCPUDescriptorHandle.ptr += (SWAP_CHAIN_BUFFER_CNT * m_rtvDescriptorIncrementSize);

	D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = m_renderBufferFormat;
	renderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	renderTargetViewDesc.Texture2D.PlaneSlice = 0;

	for (UINT i = 0; i < RENDER_TARGET_BUFFER_CNT; i++)
	{
		m_pRtvRenderTargetBufferCPUHandles[i] = rtvCPUDescriptorHandle;
		m_pDevice->CreateRenderTargetView(m_pTexture->GetTexture(i).Get(), &renderTargetViewDesc, m_pRtvRenderTargetBufferCPUHandles[i]);
		rtvCPUDescriptorHandle.ptr += m_rtvDescriptorIncrementSize;
	}
	m_pRenderMgr->SetRtvRenderTargetBufferCPUHandles(m_pRtvRenderTargetBufferCPUHandles);
}

void CCreateMgr::CreatePostprocessShader()
{
	m_pTextureToFullScreenShader = shared_ptr<CTextureToFullScreenShader>(new CTextureToFullScreenShader(shared_from_this()));
	m_pTextureToFullScreenShader->CreateGraphicsRootSignature(shared_from_this());
	m_pTextureToFullScreenShader->CreateShader(shared_from_this(), m_pTextureToFullScreenShader->GetGraphicsRootSignature());
	m_pTextureToFullScreenShader->BuildObjects(shared_from_this(), m_pTexture);

	m_pRenderMgr->SetTextureToFullScreenShader(m_pTextureToFullScreenShader);
}

void CCreateMgr::CreateGraphicsRootSignature()
{
	HRESULT hResult;

	D3D12_DESCRIPTOR_RANGE pDescriptorRanges[6];

	pDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pDescriptorRanges[0].NumDescriptors = 1;
	pDescriptorRanges[0].BaseShaderRegister = 1; //Game Objects
	pDescriptorRanges[0].RegisterSpace = 0;
	pDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;

	pDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pDescriptorRanges[1].NumDescriptors = 1;
	pDescriptorRanges[1].BaseShaderRegister = 0; //Texture
	pDescriptorRanges[1].RegisterSpace = 0;
	pDescriptorRanges[1].OffsetInDescriptorsFromTableStart = 0;

	pDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pDescriptorRanges[2].NumDescriptors = 1;
	pDescriptorRanges[2].BaseShaderRegister = 1; //Textures
	pDescriptorRanges[2].RegisterSpace = 0;
	pDescriptorRanges[2].OffsetInDescriptorsFromTableStart = 0;

	pDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pDescriptorRanges[3].NumDescriptors = 1;
	pDescriptorRanges[3].BaseShaderRegister = 4; //Animation Objects
	pDescriptorRanges[3].RegisterSpace = 0;
	pDescriptorRanges[3].OffsetInDescriptorsFromTableStart = 0;

	pDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pDescriptorRanges[4].NumDescriptors = 1;
	pDescriptorRanges[4].BaseShaderRegister = 5; //UI Gague
	pDescriptorRanges[4].RegisterSpace = 0;
	pDescriptorRanges[4].OffsetInDescriptorsFromTableStart = 0;
	
	pDescriptorRanges[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pDescriptorRanges[5].NumDescriptors = 1;
	pDescriptorRanges[5].BaseShaderRegister = 7; //Effect
	pDescriptorRanges[5].RegisterSpace = 0;
	pDescriptorRanges[5].OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pRootParameters[11];

	pRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pRootParameters[0].Descriptor.ShaderRegister = 0; //Camera
	pRootParameters[0].Descriptor.RegisterSpace = 0;
	pRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameters[1].DescriptorTable.pDescriptorRanges = &pDescriptorRanges[0]; //Game Objects
	pRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameters[2].DescriptorTable.pDescriptorRanges = &pDescriptorRanges[1]; //Texture
	//pRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	pRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameters[3].DescriptorTable.pDescriptorRanges = &pDescriptorRanges[2]; //Textures
	pRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pRootParameters[4].Descriptor.ShaderRegister = 2; //Lights
	pRootParameters[4].Descriptor.RegisterSpace = 0;
	pRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pRootParameters[5].Descriptor.ShaderRegister = 3; //Materials
	pRootParameters[5].Descriptor.RegisterSpace = 0;
	pRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameters[6].DescriptorTable.pDescriptorRanges = &pDescriptorRanges[3]; //Animation Objects
	pRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameters[7].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameters[7].DescriptorTable.pDescriptorRanges = &pDescriptorRanges[4];  //UI Gauge Textures
	pRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pRootParameters[8].Descriptor.ShaderRegister = 6; //Light Camera
	pRootParameters[8].Descriptor.RegisterSpace = 0;
	pRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pRootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pRootParameters[9].Descriptor.ShaderRegister = 12; //t12
	pRootParameters[9].Descriptor.RegisterSpace = 0;
	pRootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pRootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameters[10].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameters[10].DescriptorTable.pDescriptorRanges = &pDescriptorRanges[5];  //Effects
	pRootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_STATIC_SAMPLER_DESC samplerDesc[2];
	::ZeroMemory(&samplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
	samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].MipLODBias = 0;
	samplerDesc[0].MaxAnisotropy = 1;
	samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	samplerDesc[0].MinLOD = 0;
	samplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc[0].ShaderRegister = 0;
	samplerDesc[0].RegisterSpace = 0;
	samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	samplerDesc[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	samplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	samplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	samplerDesc[1].MipLODBias = 0;
	samplerDesc[1].MaxAnisotropy = 1;
	samplerDesc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	samplerDesc[1].MinLOD = 0;
	samplerDesc[1].MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc[1].ShaderRegister = 1;
	samplerDesc[1].RegisterSpace = 0;
	samplerDesc[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	::ZeroMemory(&rootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));

	rootSignatureDesc.NumParameters = _countof(pRootParameters);
	rootSignatureDesc.pParameters = pRootParameters;
	rootSignatureDesc.NumStaticSamplers = _countof(samplerDesc);
	rootSignatureDesc.pStaticSamplers = samplerDesc;
	rootSignatureDesc.Flags = rootSignatureFlags;

	ComPtr<ID3DBlob> pSignatureBlob{ NULL };		ComPtr<ID3DBlob> pErrorBlob{ NULL };
	::D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&pSignatureBlob,
		&pErrorBlob);

	hResult = m_pDevice->CreateRootSignature(
		0,
		pSignatureBlob->GetBufferPointer(),
		pSignatureBlob->GetBufferSize(),
		IID_PPV_ARGS(m_pGraphicsRootSignature.GetAddressOf()));
	if (pErrorBlob != nullptr) PrintErrorBlob(pErrorBlob);
	ThrowIfFailed(hResult);

	hResult = m_pGraphicsRootSignature->SetName(L"m_pGraphicsRootSignature");
	ThrowIfFailed(hResult);

	m_pRenderMgr->SaveGraphicsRootSignature(m_pGraphicsRootSignature);
}