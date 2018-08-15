#include "stdafx.h"
#include "CharacterFrameGaugeShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "05.Objects/96.Billboard/02.GaugeObject/GaugeObject.h"
#include "05.Objects/96.Billboard/01.FrameObject/UIFrameObject.h"

/// <summary>
/// 목적: Character Frame UI HP, MP Gauge 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  이용선, 김나단
/// 최종 수정 날짜: 2018-07-03
/// </summary>


CharacterFrameGaugeShader::CharacterFrameGaugeShader(shared_ptr<CCreateMgr> pCreateMgr)
	: CBillboardShader(pCreateMgr)
{
}

CharacterFrameGaugeShader::~CharacterFrameGaugeShader()
{
}

////////////////////////////////////////////////////////////////////////
//
void CharacterFrameGaugeShader::UpdateShaderVariables(int opt)
{
	UNREFERENCED_PARAMETER(opt);
	static UINT elementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAUGE_INFO *pMappedObject = (CB_GAUGE_INFO *)(m_pMappedObjects + (i * elementBytes));
		
		if (i == 0 ) pMappedObject->m_fCurrentHP = ((CUIFrameObject*)m_ppObjects[i])->GetCurrentHP();
		else pMappedObject->m_fCurrentHP = ((CUIFrameObject*)m_ppObjects[i])->GetCurrentExp();

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CharacterFrameGaugeShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CharacterFrameGaugeShader::Render(CCamera * pCamera)
{	
	CCollisionObject* master = ((CUIFrameObject*)m_ppObjects[0])->GetMasterObject();

	if (master->GetTeam() == TeamType::Blue)
	{
		CShader::Render(pCamera, 0);
		switch (master->GetType())
		{
		case ObjectType::StickPlayer:
			m_ppMaterials[0]->UpdateShaderVariable(0);
			break;
		case ObjectType::SwordPlayer:
			m_ppMaterials[0]->UpdateShaderVariable(1);
			break;
		case ObjectType::BowPlayer:
			m_ppMaterials[0]->UpdateShaderVariable(3);
			break;
		case ObjectType::StaffPlayer:
			m_ppMaterials[0]->UpdateShaderVariable(2);
			break;
		}
		if (m_ppObjects[0]) m_ppObjects[0]->Render(pCamera);
	}
	else if (master->GetTeam() == TeamType::Red)
	{
		CShader::Render(pCamera, 1);
		switch (master->GetType())
		{
		case ObjectType::StickPlayer:
			m_ppMaterials[1]->UpdateShaderVariable(0);
			break;
		case ObjectType::SwordPlayer:
			m_ppMaterials[1]->UpdateShaderVariable(1);
			break;
		case ObjectType::BowPlayer:
			m_ppMaterials[1]->UpdateShaderVariable(3);
			break;
		case ObjectType::StaffPlayer:
			m_ppMaterials[1]->UpdateShaderVariable(2);
			break;
		}
		if (m_ppObjects[1]) m_ppObjects[1]->Render(pCamera);
	}
	
	CShader::Render(pCamera, 2);
	m_ppMaterials[2]->UpdateShaderVariable(0);
	if (m_ppObjects[2]) m_ppObjects[2]->Render(pCamera);
}

void CharacterFrameGaugeShader::GetCamera(CCamera * pCamera)
{
	m_pCamera = pCamera;

	for (int i = 0; i < m_nObjects; ++i) {
		static_cast<CUIFrameObject*>(m_ppObjects[i])->SetCamera(m_pCamera);
	}
}

bool CharacterFrameGaugeShader::OnProcessKeyInput(UCHAR * pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);

	return true;
}

bool CharacterFrameGaugeShader::OnProcessMouseInput(WPARAM pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);

	return true;
}

////////////////////////////////////////////////////////////////////////
//
D3D12_INPUT_LAYOUT_DESC CharacterFrameGaugeShader::CreateInputLayout()
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

D3D12_BLEND_DESC CharacterFrameGaugeShader::CreateBlendState()
{
	D3D12_BLEND_DESC blendDesc;
	::ZeroMemory(&blendDesc, sizeof(D3D12_BLEND_DESC));

	blendDesc.AlphaToCoverageEnable = TRUE;
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

D3D12_SHADER_BYTECODE CharacterFrameGaugeShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSTexturedGauge",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CharacterFrameGaugeShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSTexturedIconGauge",
		"ps_5_1",
		pShaderBlob));
}

void CharacterFrameGaugeShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;

	m_nHeaps = 3;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CharacterFrameGaugeShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void * pContext)
{
	m_pCamera = (CCamera*)pContext;

	m_nObjects = 3;
	m_ppObjects = new CBaseObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	CreateShaderVariables(pCreateMgr, ncbElementBytes, m_nObjects);
	for (int i = 0; i < m_nObjects; ++i) {
		CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 1, 4, i);
		CreateConstantBufferViews(pCreateMgr, 1, m_pConstBuffer.Get(), ncbElementBytes, i, i);
	}

	m_nMaterials = m_nHeaps;
	m_ppMaterials = new CMaterial*[m_nMaterials];

	m_ppMaterials[0] = Materials::CreatePlayerBlueIconMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[1] = Materials::CreatePlayerRedIconMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);
	m_ppMaterials[2] = Materials::CreatePlayerMPGagueMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[2], &m_psrvGPUDescriptorStartHandle[2]);

	CUIFrameObject *pGaugeObject = NULL;

	for (int i = 0; i < m_nObjects; ++i) {
		pGaugeObject = new CUIFrameObject(pCreateMgr, (UIFrameType)(CharacterFrameHP + CLAMP(i - 1, 0, 1)));

		pGaugeObject->SetCamera(m_pCamera);
		pGaugeObject->SetDistance(FRAME_BUFFER_WIDTH / (128.0128f - (i * 0.001f)));	 // distance 9.9
		pGaugeObject->SetObject(m_pPlayer);
		pGaugeObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[i].ptr);

		m_ppObjects[i] = pGaugeObject;
	}
}

void CharacterFrameGaugeShader::ReleaseObjects()
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
