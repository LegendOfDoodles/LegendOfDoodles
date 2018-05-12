#include "stdafx.h"
#include "CreateMgr.h"
#include "00.Global/01.Utility/02.TextureLoader/DDSTextureLoader12.h"

/// <summary>
/// 목적: 생성 관련 함수를 모아 두어 헷갈리는 일 없이 생성 가능하도록 함
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-05-08
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

	CreateDirect3dDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	CreateGraphicsRootSignature();

	m_renderMgr.Initialize(m_wndClientWidth, m_wndClientHeight);
}

void CCreateMgr::Release()
{
	HRESULT hResult;
#ifdef _DEBUG
	Safe_Release(m_pDebugController);
#endif

	// Render Target View
	for (int i = 0; i < SWAP_CHAIN_BUFFER_CNT; i++)
	{
		Safe_Release(m_ppRenderTargetBuffers[i]);
	}
	Safe_Release(m_pRtvDescriptorHeap);

	// Depth Stencil View
	Safe_Release(m_pDepthStencilBuffer);
	Safe_Release(m_pDsvDescriptorHeap);

	// Command Queue
	Safe_Release(m_pCommandAllocator);
	Safe_Release(m_pCommandQueue);
	Safe_Release(m_pCommandList);

	// Fence
	Safe_Release(m_pFence);

	hResult = m_pSwapChain->SetFullscreenState(FALSE, NULL);
	assert(SUCCEEDED(hResult) && "SetFullscreenState Failed");

	// Swap Chain
	Safe_Release(m_pSwapChain);

	// Factory and Device
	Safe_Release(m_pDevice);
	Safe_Release(m_pFactory);

	// Root Signature
	Safe_Release(m_pGraphicsRootSignature);

	// Render Manager
	m_renderMgr.Release();
}

void CCreateMgr::Resize(int width, int height)
{
	m_wndClientWidth = width;
	m_wndClientHeight = height;

	m_renderMgr.WaitForGpuComplete();
	OnResizeBackBuffers();
	m_renderMgr.WaitForGpuComplete();
}

void CCreateMgr::OnResizeBackBuffers()
{
	HRESULT hResult;

	hResult = m_pCommandList->Reset(m_pCommandAllocator, NULL);
	assert(SUCCEEDED(hResult) && "CommandList->Reset Failed");

	for (int i = 0; i < SWAP_CHAIN_BUFFER_CNT; i++)
	{
		if (m_ppRenderTargetBuffers[i])
		{
			m_ppRenderTargetBuffers[i]->Release();
		}
	}
	if (m_pDepthStencilBuffer) { m_pDepthStencilBuffer->Release(); }

#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	hResult = m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	assert(SUCCEEDED(hResult) && "SwapChain->GetDesc Failed");

	hResult = m_pdxgiSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	assert(SUCCEEDED(hResult) && "ResizeBuffers Failed");

	m_nSwapChainBufferIndex = 0;
#else
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	hResult = m_pSwapChain->GetDesc(&swapChainDesc);
	assert(SUCCEEDED(hResult) && "SwapChain->GetDesc Failed");

	hResult = m_pSwapChain->ResizeBuffers(SWAP_CHAIN_BUFFER_CNT, m_wndClientWidth,
		m_wndClientHeight, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags);
	assert(SUCCEEDED(hResult) && "ResizeBuffers Failed");

	m_renderMgr.SetSwapChainBufferIndex(m_pSwapChain->GetCurrentBackBufferIndex());
#endif

	CreateRenderTargetView();
	CreateDepthStencilView();

	hResult = m_pCommandList->Close();
	assert(SUCCEEDED(hResult) && "CommandList->Close Failed");

	ID3D12CommandList *ppCommandLists[] = { m_pCommandList };
	m_pCommandQueue->ExecuteCommandLists(1, ppCommandLists);
}

DXGI_MODE_DESC CreateTargetParameters(int width, int height)
{
	DXGI_MODE_DESC targetParameters;
	targetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	targetParameters.Width = width;
	targetParameters.Height = height;
	targetParameters.RefreshRate.Numerator = 60;
	targetParameters.RefreshRate.Denominator = 1;
	targetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	targetParameters.ScanlineOrdering =
		DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	return(targetParameters);
}

void CCreateMgr::ChangeScreenMode()
{
	HRESULT hResult;
	BOOL fullScreenState = FALSE;

	hResult = m_pSwapChain->GetFullscreenState(&fullScreenState, NULL);
	assert(SUCCEEDED(hResult) && "GetFullscreenState Failed");

	if (!fullScreenState)
	{
		hResult = m_pSwapChain->ResizeTarget(&CreateTargetParameters(m_wndClientWidth, m_wndClientHeight));
		assert(SUCCEEDED(hResult) && "ResizeTarget Failed");
	}
	hResult = m_pSwapChain->SetFullscreenState(!fullScreenState, NULL);
	assert(SUCCEEDED(hResult) && "SetFullscreenState Failed");

	m_renderMgr.WaitForGpuComplete();
	OnResizeBackBuffers();
	m_renderMgr.WaitForGpuComplete();
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

ID3D12Resource* CCreateMgr::CreateBufferResource(
	void *pData, UINT nBytes, D3D12_HEAP_TYPE heapType,
	D3D12_RESOURCE_STATES resourceStates, ID3D12Resource **ppUploadBuffer)
{
	HRESULT hResult;
	ID3D12Resource *pBuffer = NULL;

	hResult = m_pDevice->CreateCommittedResource(
		&CreateBufferHeapProperties(heapType),
		D3D12_HEAP_FLAG_NONE, 
		&CreateBufferResourceDesc(nBytes),
		CreateBufferInitialStates(heapType),
		NULL,
		IID_PPV_ARGS(&pBuffer));
	assert(SUCCEEDED(hResult) && "CreateCommittedResource Failed");

	if (pData)
	{
		switch (heapType)
		{
		case D3D12_HEAP_TYPE_DEFAULT:
		{
			if (ppUploadBuffer)
			{
				//업로드 버퍼를 생성한다.
				hResult = m_pDevice->CreateCommittedResource(&CreateBufferHeapProperties(D3D12_HEAP_TYPE_UPLOAD),
					D3D12_HEAP_FLAG_NONE, &CreateBufferResourceDesc(nBytes), D3D12_RESOURCE_STATE_GENERIC_READ, NULL,
					IID_PPV_ARGS(ppUploadBuffer));
				assert(SUCCEEDED(hResult) && "CreateCommittedResource Failed");

				//업로드 버퍼를 매핑하여 초기화 데이터를 업로드 버퍼에 복사한다.
				D3D12_RANGE readRange = { 0, 0 };
				UINT8 *pBufferDataBegin = NULL;
				hResult = (*ppUploadBuffer)->Map(0, &readRange, (void **)&pBufferDataBegin);
				assert(SUCCEEDED(hResult) && "(*ppUploadBuffer)->Map Failed");

				memcpy(pBufferDataBegin, pData, nBytes);
				(*ppUploadBuffer)->Unmap(0, NULL);

				//업로드 버퍼의 내용을 디폴트 버퍼에 복사한다.
				m_pCommandList->CopyResource(pBuffer, *ppUploadBuffer);

				m_pCommandList->ResourceBarrier(1, &CreateResourceBarrier(pBuffer, D3D12_RESOURCE_STATE_COPY_DEST, resourceStates));
			}
			break;
		}
		case D3D12_HEAP_TYPE_UPLOAD:
		{
			D3D12_RANGE readRange = { 0, 0 };
			UINT8 *pBufferDataBegin = NULL;
			hResult = pBuffer->Map(0, &readRange, (void **)&pBufferDataBegin);
			assert(SUCCEEDED(hResult) && "pBuffer->Map Failed");

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

ID3D12Resource* CCreateMgr::CreateTextureResourceFromFile(
	wchar_t *pszFileName, 
	ID3D12Resource **ppUploadBuffer,
	D3D12_RESOURCE_STATES resourceStates)
{
	ID3D12Resource *pTexture{ NULL };
	std::unique_ptr<uint8_t[]> ddsData;
	std::vector<D3D12_SUBRESOURCE_DATA> vSubresources;
	DDS_ALPHA_MODE ddsAlphaMode = DDS_ALPHA_MODE_UNKNOWN;
	bool bIsCubeMap = false;

	HRESULT hResult = DirectX::LoadDDSTextureFromFileEx(
		m_pDevice, 
		pszFileName, 
		0, 
		D3D12_RESOURCE_FLAG_NONE,
		DDS_LOADER_DEFAULT, 
		&pTexture, 
		ddsData, 
		vSubresources, 
		&ddsAlphaMode, 
		&bIsCubeMap);
	assert(SUCCEEDED(hResult) && "LoadDDSTextureFromFileEx Failed");

	D3D12_HEAP_PROPERTIES heapPropertiesDesc;
	::ZeroMemory(&heapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	heapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapPropertiesDesc.CreationNodeMask = 1;
	heapPropertiesDesc.VisibleNodeMask = 1;

	//	D3D12_RESOURCE_DESC d3dResourceDesc = pd3dTexture->GetDesc();
	//	UINT nSubResources = d3dResourceDesc.DepthOrArraySize * d3dResourceDesc.MipLevels;
	UINT nSubResources = (UINT)vSubresources.size();
	//	UINT64 nBytes = 0;
	//	pd3dDevice->GetCopyableFootprints(&d3dResourceDesc, 0, nSubResources, 0, NULL, NULL, NULL, &nBytes);
	UINT64 nBytes = GetRequiredIntermediateSize(pTexture, 0, nSubResources);

	D3D12_RESOURCE_DESC resourceDesc;
	::ZeroMemory(&resourceDesc, sizeof(D3D12_RESOURCE_DESC));
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; //Upload Heap에는 텍스쳐를 생성할 수 없음
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
	assert(SUCCEEDED(hResult) && "CreateCommittedResource Failed");

	//UINT nSubResources = (UINT)vSubresources.size();
	//D3D12_SUBRESOURCE_DATA *pd3dSubResourceData = new D3D12_SUBRESOURCE_DATA[nSubResources];
	//for (UINT i = 0; i < nSubResources; i++) pd3dSubResourceData[i] = vSubresources.at(i);

	//	std::vector<D3D12_SUBRESOURCE_DATA>::pointer ptr = &vSubresources[0];
	::UpdateSubresources(m_pCommandList, pTexture, *ppUploadBuffer, 0, 0, nSubResources, &vSubresources[0]);

	m_pCommandList->ResourceBarrier(1, &CreateResourceBarrier(pTexture, D3D12_RESOURCE_STATE_COPY_DEST, resourceStates));

	//	delete[] pd3dSubResourceData;

	return(pTexture);
}

void CCreateMgr::ResetCommandList()
{
	m_pCommandList->Reset(m_pCommandAllocator, NULL);
}

void CCreateMgr::ExecuteCommandList()
{
	HRESULT hResult;

	hResult = m_pCommandList->Close();
	assert(SUCCEEDED(hResult) && "CommandList->Close Failed");

	ID3D12CommandList *ppCommandLists[] = { m_pCommandList };
	m_pCommandQueue->ExecuteCommandLists(1, ppCommandLists);

	m_renderMgr.WaitForGpuComplete();
}


////////////////////////////////////////////////////////////////////////
// 내부 함수
DXGI_SWAP_CHAIN_DESC CCreateMgr::CreateSwapChainDesc()
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	::ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferDesc.Width = m_wndClientWidth;
	swapChainDesc.BufferDesc.Height = m_wndClientHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = SWAP_CHAIN_BUFFER_CNT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.OutputWindow = m_hWnd;
	swapChainDesc.SampleDesc.Count = (m_msaa4xEnable) ? 4 : 1;
	swapChainDesc.SampleDesc.Quality = (m_msaa4xEnable) ? (m_msaa4xQualityLevels - 1) : 0;
	swapChainDesc.Windowed = TRUE;

#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
	swapChainDesc.Flags = 0;
#else
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#endif

	return(swapChainDesc);
}

void CCreateMgr::CreateDirect3dDevice()
{
	HRESULT hResult;
#if defined(_DEBUG)
	hResult = D3D12GetDebugInterface(IID_PPV_ARGS(&m_pDebugController));
	assert(SUCCEEDED(hResult) && "D3D12GetDebugInterface Failed");

	m_pDebugController->EnableDebugLayer();
#endif

	::CreateDXGIFactory1(IID_PPV_ARGS(&m_pFactory));

	//모든 하드웨어 어댑터 대하여 특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성한다.
	IDXGIAdapter1 *pAdapter = NULL;
	for (UINT i = 0;
		DXGI_ERROR_NOT_FOUND != m_pFactory->EnumAdapters1(i, &pAdapter);
		i++)
	{
		DXGI_ADAPTER_DESC1 adapterDesc;
		pAdapter->GetDesc1(&adapterDesc);

		if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_12_0,
			IID_PPV_ARGS(&m_pDevice)))) break;
	}

	//특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성할 수 없으면 WARP 디바이스를 생성한다.
	if (!pAdapter)
	{
		hResult = m_pFactory->EnumWarpAdapter(IID_PPV_ARGS(&pAdapter));
		D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_pDevice));
		assert(SUCCEEDED(hResult) && "EnumWarpAdapter Failed");
	}

	// MSAA Check
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x 다중 샘플링
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	hResult = m_pDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	assert(SUCCEEDED(hResult) && "CheckFeatureSupport Failed");

	m_msaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;

	// MSAA 
	m_msaa4xEnable = (m_msaa4xQualityLevels > 1) ? true : false;

	// Fence
	hResult = m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&m_pFence));
	assert(SUCCEEDED(hResult) && "CreateFence Failed");

	m_renderMgr.SetFence(m_pFence);

	// Save CbvSrvDescriptorIncrementSize
	m_cbvSrvDescriptorIncrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	if (pAdapter) pAdapter->Release();
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
		IID_PPV_ARGS(&m_pCommandQueue));
	assert(SUCCEEDED(hResult) && "CreateCommandQueue Failed");

	m_renderMgr.SetCommandQueue(m_pCommandQueue);

	// Create Allocator
	hResult = m_pDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&m_pCommandAllocator));
	assert(SUCCEEDED(hResult) && "CreateCommandAllocator Failed");
	
	m_renderMgr.SetCommandAllocator(m_pCommandAllocator);

	// Create Command List
	hResult = m_pDevice->CreateCommandList(
		0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_pCommandAllocator, NULL,
		IID_PPV_ARGS(&m_pCommandList));
	m_renderMgr.SetCommandList(m_pCommandList);
	assert(SUCCEEDED(hResult) && "CreateCommandList Failed");

	hResult = m_pCommandList->Close();
	assert(SUCCEEDED(hResult) && "CommandList->Close Failed");
}

void CCreateMgr::CreateSwapChain()
{
	HRESULT hResult;
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_wndClientWidth = rcClient.right - rcClient.left;
	m_wndClientHeight = rcClient.bottom - rcClient.top;

	// Create Swap Chain
	hResult = m_pFactory->CreateSwapChain(m_pCommandQueue,
		&CreateSwapChainDesc(), (IDXGISwapChain **)&m_pSwapChain);
	assert(SUCCEEDED(hResult) && "CreateSwapChain Failed");

	m_renderMgr.SetSwapChain(m_pSwapChain);

	//“Alt+Enter” 키의 동작을 비활성화한다.
	hResult = m_pFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	assert(SUCCEEDED(hResult) && "MakeWindowAssociation Failed");

	//스왑체인의 현재 후면버퍼 인덱스를 저장한다.
	m_renderMgr.SetSwapChainBufferIndex(m_pSwapChain->GetCurrentBackBufferIndex());
}

void CCreateMgr::CreateRtvAndDsvDescriptorHeaps()
{
	HRESULT hResult;

	// Create Render Target View Descriptor Heap
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc;
	::ZeroMemory(&descriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	descriptorHeapDesc.NumDescriptors = SWAP_CHAIN_BUFFER_CNT;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptorHeapDesc.NodeMask = 0;

	hResult = m_pDevice->CreateDescriptorHeap(&descriptorHeapDesc,
		IID_PPV_ARGS(&m_pRtvDescriptorHeap));
	assert(SUCCEEDED(hResult) && "CreateDescriptorHeap Failed");

	m_renderMgr.SetRtvDescriptorHeap(m_pRtvDescriptorHeap);

	m_rtvDescriptorIncrementSize =
		m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_renderMgr.SetRtvDescriptorIncrementSize(m_rtvDescriptorIncrementSize);

	// Create Depth Stencil View Descriptor Heap
	descriptorHeapDesc.NumDescriptors = 1;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pDevice->CreateDescriptorHeap(&descriptorHeapDesc,
		IID_PPV_ARGS(&m_pDsvDescriptorHeap));
	assert(SUCCEEDED(hResult) && "CreateDescriptorHeap Failed");

	m_renderMgr.SetDsvDescriptorHeap(m_pDsvDescriptorHeap);
}

D3D12_HEAP_PROPERTIES CreateDepthStencilHeapProperties()
{
	D3D12_HEAP_PROPERTIES heapProperties;
	::ZeroMemory(&heapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CreationNodeMask = 1;
	heapProperties.VisibleNodeMask = 1;

	return(heapProperties);
}

D3D12_RESOURCE_DESC CreateDepthStencilResourceDesc(int width, int height, bool massEnable, UINT massLevel)
{
	D3D12_RESOURCE_DESC resourceDesc;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.SampleDesc.Count = (massEnable) ? 4 : 1;
	resourceDesc.SampleDesc.Quality =
		(massEnable) ? (massLevel - 1) : 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	return(resourceDesc);
}

D3D12_CLEAR_VALUE CreateDepthStencilClearValue()
{
	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;

	return(clearValue);
}

void CCreateMgr::CreateDepthStencilView()
{
	HRESULT hResult;

	// Create Depth Stencil Buffer
	hResult = m_pDevice->CreateCommittedResource(
		&CreateDepthStencilHeapProperties(), 
		D3D12_HEAP_FLAG_NONE,
		&CreateDepthStencilResourceDesc(m_wndClientWidth, m_wndClientHeight, m_msaa4xEnable, m_msaa4xQualityLevels),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, 
		&CreateDepthStencilClearValue(),
		IID_PPV_ARGS(&m_pDepthStencilBuffer));
	assert(SUCCEEDED(hResult) && "CreateCommittedResource Failed");

	// Create Depth Stencil View
	D3D12_CPU_DESCRIPTOR_HANDLE dsvCPUDescriptorHandle =
		m_pDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, NULL,
		dsvCPUDescriptorHandle);
}

void CCreateMgr::CreateRenderTargetView()
{
	HRESULT hResult;

	D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUDescriptorHandle =
		m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	for (UINT i = 0; i <SWAP_CHAIN_BUFFER_CNT; i++)
	{
		hResult = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_ppRenderTargetBuffers[i]));
		assert(SUCCEEDED(hResult) && "SwapChain->GetBuffer Failed");

		m_pDevice->CreateRenderTargetView(m_ppRenderTargetBuffers[i], NULL,
			rtvCPUDescriptorHandle);
		rtvCPUDescriptorHandle.ptr += m_rtvDescriptorIncrementSize;
	}
	m_renderMgr.SetRenderTargetBuffers(m_ppRenderTargetBuffers);
}

void CCreateMgr::CreateGraphicsRootSignature()
{
	HRESULT hResult;

#if USE_INSTANCING
	D3D12_DESCRIPTOR_RANGE pDescriptorRanges[2];

	pDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pDescriptorRanges[0].NumDescriptors = 1;
	pDescriptorRanges[0].BaseShaderRegister = 0; //Texture
	pDescriptorRanges[0].RegisterSpace = 0;
	pDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;

	pDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pDescriptorRanges[1].NumDescriptors = 1;
	pDescriptorRanges[1].BaseShaderRegister = 1; //Normal
	pDescriptorRanges[1].RegisterSpace = 0;
	pDescriptorRanges[1].OffsetInDescriptorsFromTableStart = 0;
#else
	D3D12_DESCRIPTOR_RANGE pDescriptorRanges[8];

	pDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pDescriptorRanges[0].NumDescriptors = 1;
	pDescriptorRanges[0].BaseShaderRegister = 2; //Game Objects
	pDescriptorRanges[0].RegisterSpace = 0;
	pDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;

	pDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pDescriptorRanges[1].NumDescriptors = 1;
	pDescriptorRanges[1].BaseShaderRegister = 0; //Texture
	pDescriptorRanges[1].RegisterSpace = 0;
	pDescriptorRanges[1].OffsetInDescriptorsFromTableStart = 0;

	pDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pDescriptorRanges[2].NumDescriptors = 1;
	pDescriptorRanges[2].BaseShaderRegister = 1; //Normal
	pDescriptorRanges[2].RegisterSpace = 0;
	pDescriptorRanges[2].OffsetInDescriptorsFromTableStart = 0;

	pDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pDescriptorRanges[3].NumDescriptors = 1;
	pDescriptorRanges[3].BaseShaderRegister = 2; //RoughnessMetalFresnel
	pDescriptorRanges[3].RegisterSpace = 0;
	pDescriptorRanges[3].OffsetInDescriptorsFromTableStart = 0;

	pDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pDescriptorRanges[4].NumDescriptors = 1;
	pDescriptorRanges[4].BaseShaderRegister = 3; //Specular
	pDescriptorRanges[4].RegisterSpace = 0;
	pDescriptorRanges[4].OffsetInDescriptorsFromTableStart = 0;

	pDescriptorRanges[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pDescriptorRanges[5].NumDescriptors = 1;
	pDescriptorRanges[5].BaseShaderRegister = 5; //Animation Objects
	pDescriptorRanges[5].RegisterSpace = 0;
	pDescriptorRanges[5].OffsetInDescriptorsFromTableStart = 0;

	pDescriptorRanges[6].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pDescriptorRanges[6].NumDescriptors = 1;
	pDescriptorRanges[6].BaseShaderRegister = 6; //UI
	pDescriptorRanges[6].RegisterSpace = 0;
	pDescriptorRanges[6].OffsetInDescriptorsFromTableStart = 0;

	pDescriptorRanges[7].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pDescriptorRanges[7].NumDescriptors = 4;
	pDescriptorRanges[7].BaseShaderRegister = 4; //UI Textures
	pDescriptorRanges[7].RegisterSpace = 0;
	pDescriptorRanges[7].OffsetInDescriptorsFromTableStart = 0;
#endif

	D3D12_ROOT_PARAMETER pRootParameters[12];
	pRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pRootParameters[0].Descriptor.ShaderRegister = 0; //Player
	pRootParameters[0].Descriptor.RegisterSpace = 0;
	pRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pRootParameters[1].Descriptor.ShaderRegister = 1; //Camera
	pRootParameters[1].Descriptor.RegisterSpace = 0;
	pRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

#if USE_INSTANCING
	pRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pRootParameters[2].Descriptor.ShaderRegister = 2; // t2 : Objects
	pRootParameters[2].Descriptor.RegisterSpace = 0;
	pRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameters[3].DescriptorTable.pDescriptorRanges = &pDescriptorRanges[0]; //Texture
	pRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameters[4].DescriptorTable.pDescriptorRanges = &pDescriptorRanges[1]; //Normal
	pRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

#else
	pRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameters[2].DescriptorTable.pDescriptorRanges = &pDescriptorRanges[0]; //Game Objects
	pRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameters[3].DescriptorTable.pDescriptorRanges = &pDescriptorRanges[1]; //Texture
	pRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameters[4].DescriptorTable.pDescriptorRanges = &pDescriptorRanges[2]; //Normal
	pRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameters[5].DescriptorTable.pDescriptorRanges = &pDescriptorRanges[3]; //RoughnessMetalFresnel
	pRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameters[6].DescriptorTable.pDescriptorRanges = &pDescriptorRanges[4]; //Specular
	pRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

#endif

	pRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pRootParameters[7].Descriptor.ShaderRegister = 3; //Lights
	pRootParameters[7].Descriptor.RegisterSpace = 0;
	pRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pRootParameters[8].Descriptor.ShaderRegister = 4; //Materials
	pRootParameters[8].Descriptor.RegisterSpace = 0;
	pRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pRootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameters[9].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameters[9].DescriptorTable.pDescriptorRanges = &pDescriptorRanges[5]; //Animation Objects
	pRootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pRootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameters[10].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameters[10].DescriptorTable.pDescriptorRanges = &pDescriptorRanges[6]; // UI
	pRootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pRootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameters[11].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameters[11].DescriptorTable.pDescriptorRanges = &pDescriptorRanges[7];  //UI Textures
	pRootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

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
	samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

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
	samplerDesc[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
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
		IID_PPV_ARGS(&m_pGraphicsRootSignature));
	assert(SUCCEEDED(hResult) && "CreateRootSignature Failed");
	// ExptProcess::PrintErrorBlob(pErrorBlob);

	m_renderMgr.SetGraphicsRootSignature(m_pGraphicsRootSignature);
}