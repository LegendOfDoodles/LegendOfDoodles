#include "stdafx.h"
#include "TextureToFullScreenShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

/// <summary>
/// 목적: 디퍼드 쉐이딩 적용하기 위한 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-05-21
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자

CTextureToFullScreenShader::CTextureToFullScreenShader(CCreateMgr *pCreateMgr) : CShader(pCreateMgr)
{
}

CTextureToFullScreenShader::~CTextureToFullScreenShader()
{
	ReleaseShaderVariables();
	ReleaseObjects();
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CTextureToFullScreenShader::CreateGraphicsRootSignature(CCreateMgr *pCreateMgr)
{
	HRESULT hResult;
	D3D12_DESCRIPTOR_RANGE pDescriptorRanges[1];

	pDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pDescriptorRanges[0].NumDescriptors = RENDER_TARGET_BUFFER_CNT;
	pDescriptorRanges[0].BaseShaderRegister = 2; //Texture
	pDescriptorRanges[0].RegisterSpace = 0;
	pDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pRootParameters[3];
	pRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameters[0].DescriptorTable.pDescriptorRanges = &pDescriptorRanges[0]; //Texture
	pRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pRootParameters[1].Descriptor.ShaderRegister = 0; //Camera
	pRootParameters[1].Descriptor.RegisterSpace = 0;
	pRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pRootParameters[2].Descriptor.ShaderRegister = 2; //Lights
	pRootParameters[2].Descriptor.RegisterSpace = 0;
	pRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_STATIC_SAMPLER_DESC samplerDesc[1];
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
	assert(SUCCEEDED(hResult) && "CreateRootSignature Failed");
	// ExptProcess::PrintErrorBlob(pErrorBlob);
}

void CTextureToFullScreenShader::CreateShader(CCreateMgr * pCreateMgr, ID3D12RootSignature * pGraphicsRootSignature, UINT nRenderTargets)
{
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	for (int i = 0; i < m_nPipelineStates; ++i)
	{
		m_ppPipelineStates[i] = NULL;
	}

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, pGraphicsRootSignature, nRenderTargets);
}

void CTextureToFullScreenShader::BuildObjects(CCreateMgr * pCreateMgr, void * pContext)
{
	m_pTexture = (CTexture *)pContext;

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 0, m_pTexture->GetTextureCount());
	CreateShaderVariables(pCreateMgr);
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

D3D12_SHADER_BYTECODE CTextureToFullScreenShader::CreateVertexShader(ID3DBlob ** ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "VSTextureToFullScreen", "vs_5_1", ppShaderBlob));
}

D3D12_SHADER_BYTECODE CTextureToFullScreenShader::CreatePixelShader(ID3DBlob ** ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "PSTextureToFullScreen", "ps_5_1", ppShaderBlob));
}

void CTextureToFullScreenShader::ReleaseObjects()
{
	Safe_Delete(m_pTexture);
	Safe_Release(m_pGraphicsRootSignature);
}
