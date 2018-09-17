#include "stdafx.h"
#include "LoadingBarShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/95.Sprites/Sprite.h"

/// <summary>
/// 목적: 로딩 바 출력용 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-09-17
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CLoadingBarShader::CLoadingBarShader(shared_ptr<CCreateMgr> pCreateMgr) : CShader(pCreateMgr)
{
}

CLoadingBarShader::~CLoadingBarShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CLoadingBarShader::ReleaseUploadBuffers()
{
	if (m_pLoadingBar) m_pLoadingBar->ReleaseUploadBuffers();

#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
#endif
}

void CLoadingBarShader::UpdateShaderVariables(int opt)
{
	UNREFERENCED_PARAMETER(opt);
	static UINT elementBytes = ((sizeof(CB_SPRITE_INFO) + 255) & ~255);

	CB_SPRITE_INFO *pMappedObject = (CB_SPRITE_INFO *)m_pMappedObjects;

	XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
		XMMatrixTranspose(XMLoadFloat4x4(m_pLoadingBar->GetWorldMatrix())));
	pMappedObject->m_percentage = m_loadPercentage;
}

void CLoadingBarShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera, 0);
	m_ppMaterials[0]->UpdateShaderVariables();
	m_pLoadingBar->Render(pCamera);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CLoadingBarShader::CreateInputLayout()
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

D3D12_BLEND_DESC CLoadingBarShader::CreateBlendState()
{
	D3D12_BLEND_DESC blendDesc;
	::ZeroMemory(&blendDesc, sizeof(D3D12_BLEND_DESC));

	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(blendDesc);
}

D3D12_SHADER_BYTECODE CLoadingBarShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/SpriteShader.hlsl",
		"VSSprite",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CLoadingBarShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/UIShader.hlsl",
		"PSTexturedGauge",
		"ps_5_1",
		pShaderBlob));
}

void CLoadingBarShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CLoadingBarShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void * pContext)
{
	UNREFERENCED_PARAMETER(pContext);

	UINT ncbElementBytes = ((sizeof(CB_SPRITE_INFO) + 255) & ~255);
	CreateShaderVariables(pCreateMgr, ncbElementBytes, 1);
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 1, 1, 0);
	CreateConstantBufferViews(pCreateMgr, 1, m_pConstBuffer.Get(), ncbElementBytes, 0, 0);

	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];

	m_ppMaterials[0] = Materials::CreateLoadingBarMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);

	CMesh* pLoadingBarMesh{ new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH * 0.4f, FRAME_BUFFER_HEIGHT * 0.04f, 0.f, 0.f, 0.f, 1.f) };

	m_pLoadingBar = new CSprite(pCreateMgr);
	m_pLoadingBar->SetMesh(0, pLoadingBarMesh);

	m_pLoadingBar->SetPosition(FRAME_BUFFER_WIDTH * 0.5f, FRAME_BUFFER_HEIGHT * 0.2f, 9.f);

	m_pLoadingBar->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr);
}

void CLoadingBarShader::ReleaseObjects()
{
	if (m_pLoadingBar) Safe_Delete(m_pLoadingBar);
}
