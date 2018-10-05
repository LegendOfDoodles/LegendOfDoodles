#include "stdafx.h"
#include "Shader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"
#include "05.Objects/99.Material/00.Texture/Texture.h"

/// <summary>
/// 목적: 기본 쉐이터 코드, 인터페이스 용
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-10-05
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CShader::CShader(shared_ptr<CCreateMgr> pCreateMgr)
{
	m_pCommandList = pCreateMgr->GetCommandList();
}

CShader::~CShader()
{
	Safe_Delete_Array(m_pcbvCPUDescriptorStartHandle);
	Safe_Delete_Array(m_pcbvGPUDescriptorStartHandle);
	Safe_Delete_Array(m_psrvCPUDescriptorStartHandle);
	Safe_Delete_Array(m_psrvGPUDescriptorStartHandle);
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CShader::Initialize(shared_ptr<CCreateMgr> pCreateMgr, void *pContext)
{
	pCreateMgr->ResetCommandList();
	CreateShader(pCreateMgr, RENDER_TARGET_BUFFER_CNT);
	BuildObjects(pCreateMgr, pContext);
	pCreateMgr->ExecuteCommandList();
}

void CShader::Finalize()
{
	ReleaseShaderVariables();
	ReleaseObjects();
	Release();
}

void CShader::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			m_ppObjects[j]->ReleaseUploadBuffers();
		}
	}

#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
#endif

	m_bBulidFinished = true;
}

void CShader::UpdateShaderVariables(int opt)
{
	UNREFERENCED_PARAMETER(opt);
}

void CShader::UpdateShaderVariable(XMFLOAT4X4 *pxmf4x4World)
{
	UNREFERENCED_PARAMETER(pxmf4x4World);
}

void CShader::UpdateBoundingBoxShaderVariables()
{
}

void CShader::AnimateObjects(float timeElapsed)
{
	UNREFERENCED_PARAMETER(timeElapsed);
}

void CShader::Render(CCamera *pCamera)
{
	UNREFERENCED_PARAMETER(pCamera);
	OnPrepareRender();
}

void CShader::Render(CCamera *pCamera, int opt)
{
	UNREFERENCED_PARAMETER(pCamera);
	OnPrepareRender(opt);
}

void CShader::Render(CCamera * pCamera, int opt, int index)
{
	UNREFERENCED_PARAMETER(pCamera);
	OnPrepareRender(opt, index);
}

void CShader::RenderBoundingBox(CCamera * pCamera)
{
	UNREFERENCED_PARAMETER(pCamera);
	OnPrepareRenderForBB();
}

void CShader::RenderShadow(CCamera * pCamera)
{
	UNREFERENCED_PARAMETER(pCamera);
}

bool CShader::OnProcessKeyInput(UCHAR* pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);
	return true;
}

bool CShader::OnProcessMouseInput(WPARAM pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);
	return true;
}

void CShader::ChangePipeline(int index)
{
	m_pCommandList->SetPipelineState(m_ppPipelineStates[index].Get());
}

void CShader::OnStatus(int ObjectType)
{
	UNREFERENCED_PARAMETER(ObjectType);
}

void CShader::OffStatus()
{
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC rasterizerDesc;
	::ZeroMemory(&rasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.ForcedSampleCount = 0;
	rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(rasterizerDesc);
}

D3D12_RASTERIZER_DESC CShader::CreateShadowRasterizerState()
{
	D3D12_RASTERIZER_DESC rasterizerDesc;
	::ZeroMemory(&rasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = 100000;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 1.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.ForcedSampleCount = 0;
	rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(rasterizerDesc);
}

D3D12_BLEND_DESC CShader::CreateBlendState()
{
	D3D12_BLEND_DESC blendDesc;
	::ZeroMemory(&blendDesc, sizeof(D3D12_BLEND_DESC));

	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	blendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(blendDesc);
}

D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc;
	::ZeroMemory(&depthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));

	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = 0x00;
	depthStencilDesc.StencilWriteMask = 0x00;
	depthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_REPLACE;
	depthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(depthStencilDesc);
}

D3D12_INPUT_LAYOUT_DESC CShader::CreateBoundingBoxInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pInputElementDescs[0] = {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[1] = {
		"COLOR",
		0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0,
		12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
	inputLayoutDesc.pInputElementDescs = pInputElementDescs;
	inputLayoutDesc.NumElements = nInputElementDescs;
	return(inputLayoutDesc);
}

D3D12_RASTERIZER_DESC CShader::CreateBoundingBoxRasterizerState()
{
	D3D12_RASTERIZER_DESC rasterizerDesc;
	::ZeroMemory(&rasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.ForcedSampleCount = 0;
	rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(rasterizerDesc);
}

void CShader::CreateDescriptorHeaps()
{
	m_ppCbvSrvDescriptorHeaps.resize(m_nHeaps);

	m_pcbvCPUDescriptorStartHandle = new D3D12_CPU_DESCRIPTOR_HANDLE[m_nHeaps];
	m_pcbvGPUDescriptorStartHandle = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nHeaps];
	m_psrvCPUDescriptorStartHandle = new D3D12_CPU_DESCRIPTOR_HANDLE[m_nHeaps];
	m_psrvGPUDescriptorStartHandle = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nHeaps];
}

void CShader::CreateCbvAndSrvDescriptorHeaps(shared_ptr<CCreateMgr> pCreateMgr, int nConstantBufferViews, int nShaderResourceViews, int index)
{
	UINT incrementSize = pCreateMgr->GetCbvSrvDescriptorIncrementSize();
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc;
	descriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = pCreateMgr->GetDevice()->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(m_ppCbvSrvDescriptorHeaps[index].GetAddressOf()));
	ThrowIfFailed(hResult);

	m_pcbvCPUDescriptorStartHandle[index] = m_ppCbvSrvDescriptorHeaps[index]->GetCPUDescriptorHandleForHeapStart();
	m_pcbvGPUDescriptorStartHandle[index] = m_ppCbvSrvDescriptorHeaps[index]->GetGPUDescriptorHandleForHeapStart();
	m_psrvCPUDescriptorStartHandle[index].ptr = m_pcbvCPUDescriptorStartHandle[index].ptr + (incrementSize * nConstantBufferViews);
	m_psrvGPUDescriptorStartHandle[index].ptr = m_pcbvGPUDescriptorStartHandle[index].ptr + (incrementSize * nConstantBufferViews);
}

void CShader::CreateConstantBufferViews(
	shared_ptr<CCreateMgr> pCreateMgr, int nConstantBufferViews, ID3D12Resource *pConstantBuffers, UINT nStride, int startNum, int index)
{
	UINT incrementSize = pCreateMgr->GetCbvSrvDescriptorIncrementSize();
	D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress = pConstantBuffers->GetGPUVirtualAddress() + (startNum * nStride);
	D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
	CBVDesc.SizeInBytes = nStride;
	for (int j = 0; j < nConstantBufferViews; j++)
	{
		CBVDesc.BufferLocation = gpuVirtualAddress + (nStride * j);
		D3D12_CPU_DESCRIPTOR_HANDLE cbvCPUDescriptorHandle;
		cbvCPUDescriptorHandle.ptr = m_pcbvCPUDescriptorStartHandle[index].ptr + (incrementSize * j);
		pCreateMgr->GetDevice()->CreateConstantBufferView(&CBVDesc, cbvCPUDescriptorHandle);
	}
}

void CShader::GetShaderResourceViewDesc(
	D3D12_RESOURCE_DESC resourceDesc,
	UINT nTextureType,
	D3D12_SHADER_RESOURCE_VIEW_DESC *pShaderResourceViewDesc)
{
	pShaderResourceViewDesc->Format = resourceDesc.Format;
	pShaderResourceViewDesc->Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	switch (nTextureType)
	{
	case RESOURCE_TEXTURE_2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	case RESOURCE_TEXTURE_2D_ARRAY:
		pShaderResourceViewDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		pShaderResourceViewDesc->Texture2D.MipLevels = static_cast<UINT>(-1);
		pShaderResourceViewDesc->Texture2D.MostDetailedMip = 0;
		pShaderResourceViewDesc->Texture2D.PlaneSlice = 0;
		pShaderResourceViewDesc->Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_TEXTURE_2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		pShaderResourceViewDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		pShaderResourceViewDesc->Texture2DArray.MipLevels = static_cast<UINT>(-1);
		pShaderResourceViewDesc->Texture2DArray.MostDetailedMip = 0;
		pShaderResourceViewDesc->Texture2DArray.PlaneSlice = 0;
		pShaderResourceViewDesc->Texture2DArray.ResourceMinLODClamp = 0.0f;
		pShaderResourceViewDesc->Texture2DArray.FirstArraySlice = 0;
		pShaderResourceViewDesc->Texture2DArray.ArraySize = resourceDesc.DepthOrArraySize;
		break;
	case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
		pShaderResourceViewDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		pShaderResourceViewDesc->TextureCube.MipLevels = 1;
		pShaderResourceViewDesc->TextureCube.MostDetailedMip = 0;
		pShaderResourceViewDesc->TextureCube.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		pShaderResourceViewDesc->ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		pShaderResourceViewDesc->Buffer.FirstElement = 0;
		pShaderResourceViewDesc->Buffer.NumElements = 0;
		pShaderResourceViewDesc->Buffer.StructureByteStride = 0;
		pShaderResourceViewDesc->Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	}
}

void CShader::CreateShaderResourceViews(
	shared_ptr<CCreateMgr> pCreateMgr, CTexture *pTexture,
	UINT nRootParameterStartIndex, bool bAutoIncrement, int index)
{
	UINT incrementSize = pCreateMgr->GetCbvSrvDescriptorIncrementSize();
	D3D12_CPU_DESCRIPTOR_HANDLE srvCPUDescriptorHandle = m_psrvCPUDescriptorStartHandle[index];
	D3D12_GPU_DESCRIPTOR_HANDLE srvGPUDescriptorHandle = m_psrvGPUDescriptorStartHandle[index];
	D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	int nTextures = pTexture->GetTextureCount();
	int nTextureType = pTexture->GetTextureType();
	for (int i = 0; i < nTextures; i++)
	{
		ComPtr<ID3D12Resource> pShaderResource = pTexture->GetTexture(i);
		D3D12_RESOURCE_DESC resourceDesc = pShaderResource->GetDesc();
		GetShaderResourceViewDesc(resourceDesc, nTextureType, &shaderResourceViewDesc);
		pCreateMgr->GetDevice()->CreateShaderResourceView(pShaderResource.Get(), &shaderResourceViewDesc, srvCPUDescriptorHandle);
		srvCPUDescriptorHandle.ptr += incrementSize;

		pTexture->SetRootArgument(i, (bAutoIncrement) ? (nRootParameterStartIndex + i) : nRootParameterStartIndex, srvGPUDescriptorHandle);
		srvGPUDescriptorHandle.ptr += incrementSize;
	}
}

D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	UNREFERENCED_PARAMETER(pShaderBlob);

	D3D12_SHADER_BYTECODE shaderByteCode;
	shaderByteCode.BytecodeLength = 0;
	shaderByteCode.pShaderBytecode = NULL;

	return(shaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreateHullShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	UNREFERENCED_PARAMETER(pShaderBlob);

	D3D12_SHADER_BYTECODE shaderByteCode;
	shaderByteCode.BytecodeLength = 0;
	shaderByteCode.pShaderBytecode = NULL;

	return(shaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreateDomainShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	UNREFERENCED_PARAMETER(pShaderBlob);

	D3D12_SHADER_BYTECODE shaderByteCode;
	shaderByteCode.BytecodeLength = 0;
	shaderByteCode.pShaderBytecode = NULL;

	return(shaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	UNREFERENCED_PARAMETER(pShaderBlob);

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreateBoundingBoxVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/DefaultShader.hlsl",
		"VSDiffused",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CShader::CreateBoundingBoxPixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/DefaultShader.hlsl",
		"PSDiffused",
		"ps_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CShader::CreateShadowHullShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	UNREFERENCED_PARAMETER(pShaderBlob);

	D3D12_SHADER_BYTECODE shaderByteCode;
	shaderByteCode.BytecodeLength = 0;
	shaderByteCode.pShaderBytecode = NULL;

	return(shaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreateShadowDomainShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	UNREFERENCED_PARAMETER(pShaderBlob);

	D3D12_SHADER_BYTECODE shaderByteCode;
	shaderByteCode.BytecodeLength = 0;
	shaderByteCode.pShaderBytecode = NULL;

	return(shaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreateShadowVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	UNREFERENCED_PARAMETER(pShaderBlob);

	D3D12_SHADER_BYTECODE shaderByteCode;
	shaderByteCode.BytecodeLength = 0;
	shaderByteCode.pShaderBytecode = NULL;

	return(shaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreateShadowPixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/ShadowShader.hlsl",
		"PSMain",
		"ps_5_1",
		pShaderBlob));
}

void CShader::CreateShaderWithTess(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderShadow)
{
	m_ppPipelineStates.resize(m_nPipelineStates);

	int index{ 0 };
	HRESULT hResult;
	ComPtr<ID3DBlob> pVertexShaderBlob, pHullShaderBlob, pDomainShaderBlob, pPixelShaderBlob;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc;
	::ZeroMemory(&pipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	pipelineStateDesc.pRootSignature = pCreateMgr->GetGraphicsRootSignature().Get();
	pipelineStateDesc.VS = CreateVertexShader(pVertexShaderBlob);
	pipelineStateDesc.HS = CreateHullShader(pHullShaderBlob);
	pipelineStateDesc.DS = CreateDomainShader(pDomainShaderBlob);
	pipelineStateDesc.PS = CreatePixelShader(pPixelShaderBlob);
	pipelineStateDesc.RasterizerState = CreateRasterizerState();
	pipelineStateDesc.BlendState = CreateBlendState();
	pipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	pipelineStateDesc.InputLayout = CreateInputLayout();
	pipelineStateDesc.SampleMask = UINT_MAX;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	pipelineStateDesc.NumRenderTargets = nRenderTargets;
	for (UINT i = 0; i < nRenderTargets; i++)
	{
		pipelineStateDesc.RTVFormats[i] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	}
	pipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	pipelineStateDesc.SampleDesc.Count = 1;
	pipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	hResult = pCreateMgr->GetDevice()->CreateGraphicsPipelineState(
		&pipelineStateDesc,
		IID_PPV_ARGS(m_ppPipelineStates[index++].GetAddressOf()));
	ThrowIfFailed(hResult);

	if (isRenderShadow)
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC ShadowPipelineStateDesc{ pipelineStateDesc };
		ShadowPipelineStateDesc.VS = CreateShadowVertexShader(pVertexShaderBlob);
		ShadowPipelineStateDesc.HS = CreateShadowHullShader(pHullShaderBlob);
		ShadowPipelineStateDesc.DS = CreateShadowDomainShader(pDomainShaderBlob);
		ShadowPipelineStateDesc.PS = CreateShadowPixelShader(pPixelShaderBlob);
		pipelineStateDesc.RasterizerState = CreateShadowRasterizerState();
		pipelineStateDesc.NumRenderTargets = 0;
		pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
		hResult = pCreateMgr->GetDevice()->CreateGraphicsPipelineState(
			&ShadowPipelineStateDesc,
			IID_PPV_ARGS(m_ppPipelineStates[index++].GetAddressOf()));
		ThrowIfFailed(hResult);
	}
}

void CShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_ppPipelineStates.resize(m_nPipelineStates);

	m_isRenderBB = isRenderBB;

	int index{ 0 };
	HRESULT hResult;
	ComPtr<ID3DBlob> pVertexShaderBlob, pPixelShaderBlob;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc;
	::ZeroMemory(&pipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	pipelineStateDesc.pRootSignature = pCreateMgr->GetGraphicsRootSignature().Get();
	pipelineStateDesc.VS = CreateVertexShader(pVertexShaderBlob);
	pipelineStateDesc.PS = CreatePixelShader(pPixelShaderBlob);
	pipelineStateDesc.RasterizerState = CreateRasterizerState();
	pipelineStateDesc.BlendState = CreateBlendState();
	pipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	pipelineStateDesc.InputLayout = CreateInputLayout();
	pipelineStateDesc.SampleMask = UINT_MAX;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.NumRenderTargets = nRenderTargets;
	for (UINT i = 0; i < nRenderTargets; i++)
	{
		pipelineStateDesc.RTVFormats[i] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	}
	pipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	pipelineStateDesc.SampleDesc.Count = 1;
	pipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	hResult = pCreateMgr->GetDevice()->CreateGraphicsPipelineState(
		&pipelineStateDesc,
		IID_PPV_ARGS(m_ppPipelineStates[index++].GetAddressOf()));
	ThrowIfFailed(hResult);

	if (isRenderBB)
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC BBPipelineStateDesc{ pipelineStateDesc };
		BBPipelineStateDesc.VS = CreateBoundingBoxVertexShader(pVertexShaderBlob);
		BBPipelineStateDesc.PS = CreateBoundingBoxPixelShader(pPixelShaderBlob);
		BBPipelineStateDesc.RasterizerState = CreateBoundingBoxRasterizerState();
		BBPipelineStateDesc.InputLayout = CreateBoundingBoxInputLayout();
		hResult = pCreateMgr->GetDevice()->CreateGraphicsPipelineState(
			&BBPipelineStateDesc,
			IID_PPV_ARGS(m_ppPipelineStates[index++].GetAddressOf()));
		ThrowIfFailed(hResult);
	}

	if (isRenderShadow)
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC ShadowPipelineStateDesc{ pipelineStateDesc };
		ShadowPipelineStateDesc.VS = CreateShadowVertexShader(pVertexShaderBlob);
		ShadowPipelineStateDesc.PS = CreateShadowPixelShader(pPixelShaderBlob);
		pipelineStateDesc.RasterizerState = CreateShadowRasterizerState();
		pipelineStateDesc.NumRenderTargets = 0;
		pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
		hResult = pCreateMgr->GetDevice()->CreateGraphicsPipelineState(
			&ShadowPipelineStateDesc,
			IID_PPV_ARGS(m_ppPipelineStates[index++].GetAddressOf()));
		ThrowIfFailed(hResult);
	}
}

void CShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, ComPtr<ID3D12RootSignature> pGraphicsRootSignature, UINT nRenderTargets)
{
	m_ppPipelineStates.resize(m_nPipelineStates);

	int index{ 0 };
	HRESULT hResult;
	ComPtr<ID3DBlob> pVertexShaderBlob, pPixelShaderBlob;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc;
	::ZeroMemory(&pipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	pipelineStateDesc.pRootSignature = pGraphicsRootSignature.Get();
	pipelineStateDesc.VS = CreateVertexShader(pVertexShaderBlob);
	pipelineStateDesc.PS = CreatePixelShader(pPixelShaderBlob);
	pipelineStateDesc.RasterizerState = CreateRasterizerState();
	pipelineStateDesc.BlendState = CreateBlendState();
	pipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	pipelineStateDesc.InputLayout = CreateInputLayout();
	pipelineStateDesc.SampleMask = UINT_MAX;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.NumRenderTargets = nRenderTargets;
	for (UINT i = 0; i < nRenderTargets; i++)
	{
		pipelineStateDesc.RTVFormats[i] = DXGI_FORMAT_R8G8B8A8_UNORM;
	}
	pipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	pipelineStateDesc.SampleDesc.Count = 1;
	pipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	hResult = pCreateMgr->GetDevice()->CreateGraphicsPipelineState(
		&pipelineStateDesc,
		IID_PPV_ARGS(m_ppPipelineStates[index++].GetAddressOf()));
	ThrowIfFailed(hResult);
}

void CShader::CreateShaderVariables(shared_ptr<CCreateMgr> pCreateMgr, UINT stride, int nBuffers, bool isRenderBB, UINT BBStride, int BBnBuffers)
{
	HRESULT hResult;

	m_pConstBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		stride * nBuffers,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	hResult = m_pConstBuffer->Map(0, NULL, (void **)&m_pMappedObjects);
	ThrowIfFailed(hResult);

	if (isRenderBB)
	{
		m_pBoundingBoxBuffer = pCreateMgr->CreateBufferResource(
			NULL,
			BBStride * BBnBuffers,
			D3D12_HEAP_TYPE_UPLOAD,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
			NULL);

		hResult = m_pBoundingBoxBuffer->Map(0, NULL, (void **)&m_pMappedBoundingBoxes);
		ThrowIfFailed(hResult);
	}
}

void CShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext)
{
	UNREFERENCED_PARAMETER(pCreateMgr);
	UNREFERENCED_PARAMETER(pContext);
}

void CShader::ReleaseShaderVariables()
{
	if (m_pConstBuffer.Get())
	{
		m_pConstBuffer->Unmap(0, NULL);
		m_pConstBuffer.Reset();
	}

	if (m_pBoundingBoxBuffer.Get())
	{
		m_pBoundingBoxBuffer->Unmap(0, NULL);
		m_pBoundingBoxBuffer.Reset();
	}

	for (int i = 0; i < m_nHeaps; ++i)
	{
		m_ppCbvSrvDescriptorHeaps[i].Reset();
	}
	m_ppCbvSrvDescriptorHeaps.clear();
}

void CShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			delete m_ppObjects[j];
		}
		Safe_Delete_Array(m_ppObjects);
	}

#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i < m_nMaterials; ++i)
		{
			Safe_Delete(m_ppMaterials[i]);
		}
		Safe_Delete_Array(m_ppMaterials);
	}
#endif
}

void CShader::OnPrepareRender(int opt, int index)
{
	//파이프라인에 그래픽스 상태 객체를 설정한다.
	m_pCommandList->SetPipelineState(m_ppPipelineStates[index].Get());
	if (!m_ppCbvSrvDescriptorHeaps.empty()) m_pCommandList->SetDescriptorHeaps(1, m_ppCbvSrvDescriptorHeaps[opt].GetAddressOf());

	UpdateShaderVariables(opt);
}

void CShader::OnPrepareRenderForBB()
{
	//파이프라인에 그래픽스 상태 객체를 설정한다.
	m_pCommandList->SetPipelineState(m_ppPipelineStates[BOUNDING_BOX].Get());
	if (!m_ppCbvSrvDescriptorHeaps.empty()) m_pCommandList->SetDescriptorHeaps(1, m_ppCbvSrvDescriptorHeaps[m_boundingBoxHeapNumber].GetAddressOf());
	UpdateBoundingBoxShaderVariables();
}

D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(
	WCHAR *pszFileName, LPCSTR pszShaderName,
	LPCSTR pszShaderProfile, ComPtr<ID3DBlob>& pShaderBlob)
{
	HRESULT hResult;
	UINT nCompileFlags{ 0 };

#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3DBlob> pErrorBlob{ nullptr };
	hResult = D3DCompileFromFile(
		pszFileName
		, NULL
		, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, pszShaderName
		, pszShaderProfile
		, nCompileFlags
		, 0
		, pShaderBlob.GetAddressOf()
		, pErrorBlob.GetAddressOf());
	if (pErrorBlob != nullptr) PrintErrorBlob(pErrorBlob);
	ThrowIfFailed(hResult);

	D3D12_SHADER_BYTECODE shaderByteCode;
	shaderByteCode.BytecodeLength = pShaderBlob->GetBufferSize();
	shaderByteCode.pShaderBytecode = pShaderBlob->GetBufferPointer();

	return(shaderByteCode);
}