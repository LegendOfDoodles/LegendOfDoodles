#include "stdafx.h"
#include "LoadingScreenShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/95.Sprites/Sprite.h"

/// <summary>
/// 목적: 로딩 스크린 출력용 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-09-17
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CLoadingScreenShader::CLoadingScreenShader(shared_ptr<CCreateMgr> pCreateMgr) : CShader(pCreateMgr)
{
}

CLoadingScreenShader::~CLoadingScreenShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CLoadingScreenShader::ReleaseUploadBuffers()
{
	if (m_pBackground) m_pBackground->ReleaseUploadBuffers();

#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
#endif
}

void CLoadingScreenShader::UpdateShaderVariables(int opt)
{
	UNREFERENCED_PARAMETER(opt);
	static UINT elementBytes = ((sizeof(CB_SPRITE_INFO) + 255) & ~255);

	CB_SPRITE_INFO *pMappedObject = (CB_SPRITE_INFO *)m_pMappedObjects;

	XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
		XMMatrixTranspose(XMLoadFloat4x4(m_pBackground->GetWorldMatrix())));
	pMappedObject->m_percentage = 1.f;
}

void CLoadingScreenShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera, 0);
	m_ppMaterials[0]->UpdateShaderVariables();
	m_pBackground->Render(pCamera);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CLoadingScreenShader::CreateInputLayout()
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
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC CLoadingScreenShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC rasterizerDesc;
	::ZeroMemory(&rasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthClipEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.ForcedSampleCount = 0;
	rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(rasterizerDesc);
}

D3D12_SHADER_BYTECODE CLoadingScreenShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/SpriteShader.hlsl",
		"VSSprite",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CLoadingScreenShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/SpriteShader.hlsl",
		"PSTextured",
		"ps_5_1",
		pShaderBlob));
}

void CLoadingScreenShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CLoadingScreenShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void * pContext)
{
	UNREFERENCED_PARAMETER(pContext);

	UINT ncbElementBytes = ((sizeof(CB_SPRITE_INFO) + 255) & ~255);
	CreateShaderVariables(pCreateMgr, ncbElementBytes, 1);
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 1, 1, 0);
	CreateConstantBufferViews(pCreateMgr, 1, m_pConstBuffer.Get(), ncbElementBytes, 0, 0);

	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];

	m_ppMaterials[0] = Materials::CreateLoadingScreenMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);

	CMesh* pFullScreenRectMesh{ new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 0.f, 0.f, 1.f) };

	m_pBackground = new CSprite(pCreateMgr);
	m_pBackground->SetMesh(0, pFullScreenRectMesh);

	m_pBackground->SetPosition(FRAME_BUFFER_WIDTH * 0.5f, FRAME_BUFFER_HEIGHT * 0.5f, 10.f);

	m_pBackground->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr);
}

void CLoadingScreenShader::ReleaseObjects()
{
	if (m_pBackground) Safe_Delete(m_pBackground);
}
