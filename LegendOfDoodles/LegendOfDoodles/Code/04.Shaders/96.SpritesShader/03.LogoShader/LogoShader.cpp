#include "stdafx.h"
#include "LogoShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/95.Sprites/Sprite.h"

/// <summary>
/// 목적: 로딩 바 출력용 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-09-16
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CLogoShader::CLogoShader(shared_ptr<CCreateMgr> pCreateMgr) : CShader(pCreateMgr)
{
}

CLogoShader::~CLogoShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CLogoShader::ReleaseUploadBuffers()
{
	if (m_pLogoScreen) m_pLogoScreen->ReleaseUploadBuffers();

#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
#endif
}

void CLogoShader::UpdateShaderVariables(int opt)
{
	UNREFERENCED_PARAMETER(opt);
	static UINT elementBytes = ((sizeof(CB_SPRITE_INFO) + 255) & ~255);

	CB_SPRITE_INFO *pMappedObject = (CB_SPRITE_INFO *)m_pMappedObjects;

	XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
		XMMatrixTranspose(XMLoadFloat4x4(m_pLogoScreen->GetWorldMatrix())));

	pMappedObject->m_percentage = min(m_loadPercentage / TIME_LOGO_SHOW, 1.f);
}

void CLogoShader::AnimateObjects(float timeElapsed)
{
	m_loadPercentage += timeElapsed;
}

void CLogoShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera, 0);
	m_ppMaterials[0]->UpdateShaderVariables();
	m_pLogoScreen->Render(pCamera);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CLogoShader::CreateInputLayout()
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

D3D12_SHADER_BYTECODE CLogoShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/DefaultShader.hlsl",
		"VSSprite",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CLogoShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/DefaultShader.hlsl",
		"PSLogo",
		"ps_5_1",
		pShaderBlob));
}

void CLogoShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CLogoShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void * pContext)
{
	UNREFERENCED_PARAMETER(pContext);

	UINT ncbElementBytes = ((sizeof(CB_SPRITE_INFO) + 255) & ~255);
	CreateShaderVariables(pCreateMgr, ncbElementBytes, 1);
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 1, 1, 0);
	CreateConstantBufferViews(pCreateMgr, 1, m_pConstBuffer.Get(), ncbElementBytes, 0, 0);

	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];

	m_ppMaterials[0] = Materials::CreateLogoScreenMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);

	CMesh* pLogoScreenMesh{ new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.f, 0.f, 0.f, 1.f) };

	m_pLogoScreen = new CSprite(pCreateMgr);
	m_pLogoScreen->SetMesh(0, pLogoScreenMesh);

	m_pLogoScreen->SetPosition(0.f, 0.f, 10.f);

	m_pLogoScreen->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr);
}

void CLogoShader::ReleaseObjects()
{
	if (m_pLogoScreen) Safe_Delete(m_pLogoScreen);
}
