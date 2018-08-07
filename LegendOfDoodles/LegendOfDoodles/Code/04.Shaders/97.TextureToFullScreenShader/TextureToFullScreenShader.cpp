#include "stdafx.h"
#include "TextureToFullScreenShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

/// <summary>
/// 목적: 디퍼드 쉐이딩 적용하기 위한 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-08-05
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자

CTextureToFullScreenShader::CTextureToFullScreenShader(shared_ptr<CCreateMgr> pCreateMgr) : CShader(pCreateMgr)
{
}

CTextureToFullScreenShader::~CTextureToFullScreenShader()
{
	ReleaseShaderVariables();
	ReleaseObjects();
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CTextureToFullScreenShader::CreateGraphicsRootSignature(shared_ptr<CCreateMgr> pCreateMgr)
{
	HRESULT hResult;
	D3D12_DESCRIPTOR_RANGE pDescriptorRanges[4];

	pDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pDescriptorRanges[0].NumDescriptors = RENDER_TARGET_BUFFER_CNT + 1;
	pDescriptorRanges[0].BaseShaderRegister = 3; //Textures By Render Target
	pDescriptorRanges[0].RegisterSpace = 0;
	pDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;

	pDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pDescriptorRanges[1].NumDescriptors = 1;
	pDescriptorRanges[1].BaseShaderRegister = 2; //TextureCube
	pDescriptorRanges[1].RegisterSpace = 0;
	pDescriptorRanges[1].OffsetInDescriptorsFromTableStart = 0;

	pDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pDescriptorRanges[2].NumDescriptors = 1;
	pDescriptorRanges[2].BaseShaderRegister = 1; //Textures
	pDescriptorRanges[2].RegisterSpace = 0;
	pDescriptorRanges[2].OffsetInDescriptorsFromTableStart = 0;

	pDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pDescriptorRanges[3].NumDescriptors = 1;
	pDescriptorRanges[3].BaseShaderRegister = 0; //Texture
	pDescriptorRanges[3].RegisterSpace = 0;
	pDescriptorRanges[3].OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pRootParameters[7];
	pRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameters[0].DescriptorTable.pDescriptorRanges = &pDescriptorRanges[0]; //Textures By Render Target
	pRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pRootParameters[1].Descriptor.ShaderRegister = 0; //Camera
	pRootParameters[1].Descriptor.RegisterSpace = 0;
	pRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pRootParameters[2].Descriptor.ShaderRegister = 2; //Lights
	pRootParameters[2].Descriptor.RegisterSpace = 0;
	pRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameters[3].DescriptorTable.pDescriptorRanges = &pDescriptorRanges[1]; //TextureCube
	pRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameters[4].DescriptorTable.pDescriptorRanges = &pDescriptorRanges[2]; //Textures
	pRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameters[5].DescriptorTable.pDescriptorRanges = &pDescriptorRanges[3]; //Texture
	pRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pRootParameters[6].Descriptor.ShaderRegister = 6; //Light Camera
	pRootParameters[6].Descriptor.RegisterSpace = 0;
	pRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

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

	samplerDesc[1].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc[1].MipLODBias = 0;
	samplerDesc[1].MaxAnisotropy = 16;
	samplerDesc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	samplerDesc[1].MinLOD = 0;
	samplerDesc[1].MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc[1].ShaderRegister = 2;
	samplerDesc[1].RegisterSpace = 0;
	samplerDesc[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	samplerDesc[1].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;

	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	::ZeroMemory(&rootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	rootSignatureDesc.NumParameters = _countof(pRootParameters);
	rootSignatureDesc.pParameters = pRootParameters;
	rootSignatureDesc.NumStaticSamplers = _countof(samplerDesc);
	rootSignatureDesc.pStaticSamplers = samplerDesc;
	rootSignatureDesc.Flags = rootSignatureFlags;

	ComPtr<ID3DBlob> pSignatureBlob{ NULL };		ComPtr<ID3DBlob> pErrorBlob{ NULL };
	D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSignatureBlob, &pErrorBlob);
	hResult = pCreateMgr->GetDevice()->CreateRootSignature(
		0,
		pSignatureBlob->GetBufferPointer(),
		pSignatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&m_pGraphicsRootSignature));
	if (pErrorBlob != nullptr) PrintErrorBlob(pErrorBlob);
	ThrowIfFailed(hResult);
}

void CTextureToFullScreenShader::CreateShaderResourceViews(shared_ptr<CCreateMgr> pCreateMgr, shared_ptr<CTexture> pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement, int index)
{
	UINT incrementSize = pCreateMgr->GetCbvSrvDescriptorIncrementSize();
	D3D12_CPU_DESCRIPTOR_HANDLE srvCPUDescriptorHandle = m_psrvCPUDescriptorStartHandle[index];
	D3D12_GPU_DESCRIPTOR_HANDLE srvGPUDescriptorHandle = m_psrvGPUDescriptorStartHandle[index];
	D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	int nTextureType = pTexture->GetTextureType();
	for (int i = 0; i < RENDER_TARGET_BUFFER_CNT; i++)
	{
		ComPtr<ID3D12Resource> pShaderResource = pTexture->GetTexture(i);
		D3D12_RESOURCE_DESC resourceDesc = pShaderResource->GetDesc();
		GetShaderResourceViewDesc(resourceDesc, nTextureType, &shaderResourceViewDesc);
		pCreateMgr->GetDevice()->CreateShaderResourceView(pShaderResource.Get(), &shaderResourceViewDesc, srvCPUDescriptorHandle);
		srvCPUDescriptorHandle.ptr += incrementSize;

		pTexture->SetRootArgument(i, (bAutoIncrement) ? (nRootParameterStartIndex + i) : nRootParameterStartIndex, srvGPUDescriptorHandle);
		srvGPUDescriptorHandle.ptr += incrementSize;
	}

	// Shadow 텍스처
	ComPtr<ID3D12Resource> pShadowDepthBuffer = pTexture->GetTexture(RENDER_TARGET_BUFFER_CNT);
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDescForShadow = {};
	srvDescForShadow.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDescForShadow.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDescForShadow.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDescForShadow.Texture2D.MostDetailedMip = 0;
	srvDescForShadow.Texture2D.MipLevels = 1;
	srvDescForShadow.Texture2D.ResourceMinLODClamp = 0.0f;
	srvDescForShadow.Texture2D.PlaneSlice = 0;
	pCreateMgr->GetDevice()->CreateShaderResourceView(pShadowDepthBuffer.Get(), &srvDescForShadow, srvCPUDescriptorHandle);

	pTexture->SetRootArgument(RENDER_TARGET_BUFFER_CNT, nRootParameterStartIndex, srvGPUDescriptorHandle);
}

void CTextureToFullScreenShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, ComPtr<ID3D12RootSignature> pGraphicsRootSignature, UINT nRenderTargets)
{
	m_nPipelineStates = 1;

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, pGraphicsRootSignature, nRenderTargets);
}

void CTextureToFullScreenShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, shared_ptr<CTexture> pContext)
{
	m_pTexture = pContext;

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 0, m_pTexture->GetTextureCount());
	CreateShaderResourceViews(pCreateMgr, m_pTexture, 0, false);
}

void CTextureToFullScreenShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera);

	if (m_pTexture) m_pTexture->UpdateShaderVariables();

	m_pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pCommandList->DrawInstanced(6, 1, 0, 0);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CTextureToFullScreenShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
	inputLayoutDesc.pInputElementDescs = NULL;
	inputLayoutDesc.NumElements = 0;

	return(inputLayoutDesc);
}

D3D12_SHADER_BYTECODE CTextureToFullScreenShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/PostProcessShaders.hlsl",
		"VSTextureToFullScreen",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CTextureToFullScreenShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/PostProcessShaders.hlsl",
		"PSTextureToFullScreen",
		"ps_5_1",
		pShaderBlob));
}