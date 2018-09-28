#include "stdafx.h"
#include "LoadingCardShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/95.Sprites/Sprite.h"
#include "07.Network/Network.h"

/// <summary>
/// 목적: 로딩 바 출력용 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-09-27
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CLoadingCardShader::CLoadingCardShader(shared_ptr<CCreateMgr> pCreateMgr) : CShader(pCreateMgr)
{
}

CLoadingCardShader::~CLoadingCardShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CLoadingCardShader::ReleaseUploadBuffers()
{
	for (int i = 0; i < 4; ++i)
	{
		if (m_LoadingCards[i])m_LoadingCards[i]->ReleaseUploadBuffers();
	}

#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
#endif
}

void CLoadingCardShader::UpdateShaderVariables(int opt)
{
	UNREFERENCED_PARAMETER(opt);
	static UINT elementBytes = ((sizeof(CB_SPRITE_INFO) + 255) & ~255);

	for (int i = 0; i < 4; ++i)
	{
		CB_SPRITE_INFO *pMappedObject = (CB_SPRITE_INFO *)(m_pMappedObjects + elementBytes * i);

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_LoadingCards[i]->GetWorldMatrix())));
		int adjId{ (i == 1) ? 2 : (i == 2) ? 1 : i };
		pMappedObject->m_percentage = (float)m_pNetwork->m_EachCardType[i] * 2 + m_pNetwork->m_EachPlayerLoadPercentage[adjId];
	}
}

void CLoadingCardShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera, 0);
	m_ppMaterials[0]->UpdateShaderVariables();

	for (int j = 0; j < 4; j++)
	{
		m_LoadingCards[j]->Render(pCamera);
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CLoadingCardShader::CreateInputLayout()
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

D3D12_BLEND_DESC CLoadingCardShader::CreateBlendState()
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

D3D12_SHADER_BYTECODE CLoadingCardShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/SpriteShader.hlsl",
		"VSSprite",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CLoadingCardShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/SpriteShader.hlsl",
		"PSLoadingCards",
		"ps_5_1",
		pShaderBlob));
}

void CLoadingCardShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CLoadingCardShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void * pContext)
{
	UNREFERENCED_PARAMETER(pContext);

	UINT ncbElementBytes = ((sizeof(CB_SPRITE_INFO) + 255) & ~255);
	CreateShaderVariables(pCreateMgr, ncbElementBytes, 4);
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 4, 1, 0);
	CreateConstantBufferViews(pCreateMgr, 4, m_pConstBuffer.Get(), ncbElementBytes, 0, 0);

	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];

	m_ppMaterials[0] = Materials::CreateRoomCardsMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);

	CMesh* pTitleButtonMesh{ new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 5.f, FRAME_BUFFER_WIDTH / 5.f * 1.6f, 0.f, 0.f, 0.f, 1.f) };

	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };

	for (int i = 0; i < 4; ++i)
	{
		CSprite *pCard{ new CSprite(pCreateMgr) };
		pCard->SetMesh(0, pTitleButtonMesh);
		pCard->SetPosition(FRAME_BUFFER_WIDTH * (i + 1) / 5.f, FRAME_BUFFER_HEIGHT * 0.6f, 9.f);
		pCard->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + incrementSize * i);
		m_LoadingCards[i] = pCard;
	}
}

void CLoadingCardShader::ReleaseObjects()
{
	for (int i = 0; i < 4; ++i)
	{
		if (m_LoadingCards[i]) Safe_Delete(m_LoadingCards[i]);
	}
}
