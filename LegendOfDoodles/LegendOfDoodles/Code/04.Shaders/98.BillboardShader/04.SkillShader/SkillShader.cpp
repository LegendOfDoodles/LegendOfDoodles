#include "stdafx.h"
#include "SkillShader.h"
#include "05.Objects/96.Billboard/04.SkillObject/SkillObject.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"
#include "05.Objects/08.Player/Player.h"
#include "07.Network/Network.h"

/// <summary>
/// 목적: Skill UI Icon Shader (틀만 생성)
/// 최종 수정자:  김나단
/// 수정자 목록:  이용선, 김나단
/// 최종 수정 날짜: 2018-08-22
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CSkillShader::CSkillShader(shared_ptr<CCreateMgr> pCreateMgr)
	: CBillboardShader(pCreateMgr)
{
}

CSkillShader::~CSkillShader()
{
}

////////////////////////////////////////////////////////////////////////
//
void CSkillShader::UpdateShaderVariables(int opt)
{
	UNREFERENCED_PARAMETER(opt);
	static UINT elementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAUGE_INFO *pMappedObject = (CB_GAUGE_INFO *)(m_pMappedObjects + (i * elementBytes));
		if (i >= 4) pMappedObject->m_fCurrentHP = static_cast<CSkillObject*>(m_ppObjects[i])->GetCoolTime();	// Color
		else		 pMappedObject->m_fCurrentHP = 1.0f;	// Grey
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CSkillShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}

	if (m_Change == true) {
		int WeaponType = m_pPlayer->GetPlayerStatus()->Weapon;

		int GrayIcon = WeaponType * 2;
		int ColorIcon = WeaponType * 2 + 1;

		for (int i = 0; i < m_nObjects; ++i)
		{
			if (i < 4) m_ppObjects[i]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[GrayIcon].ptr + (m_incrementSize * i));
			else	   m_ppObjects[i]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[ColorIcon].ptr + (m_incrementSize * (i - 4)));
		}

		m_Change = false;
	}
}

void CSkillShader::Render(CCamera * pCamera)
{
	if (m_pPlayer->GetType() != ObjectType::StickPlayer) 
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			SkillUIType type = (SkillUIType)((CSkillObject*)m_ppObjects[j])->GetType();

			int WeaponType = m_pPlayer->GetPlayerStatus()->Weapon;

			int GrayIcon = WeaponType * 2;
			int ColorIcon = GrayIcon + 1;

			if (j == 0)
			{
				CShader::Render(pCamera, GrayIcon);
			}
			else if (j == 4)
			{
				CShader::Render(pCamera, ColorIcon);
			}

			switch (type)
			{
			case GrayQSkill:
				m_ppMaterials[GrayIcon]->UpdateShaderVariable(0);
				break;
			case GrayWSkill:
				m_ppMaterials[GrayIcon]->UpdateShaderVariable(1);
				break;
			case GrayESkill:
				m_ppMaterials[GrayIcon]->UpdateShaderVariable(2);
				break;
			case GrayRSkill:
				m_ppMaterials[GrayIcon]->UpdateShaderVariable(3);
				break;
			case QSkill:
				m_ppMaterials[ColorIcon]->UpdateShaderVariable(0);
				break;
			case WSkill:
				m_ppMaterials[ColorIcon]->UpdateShaderVariable(1);
				break;
			case ESkill:
				m_ppMaterials[ColorIcon]->UpdateShaderVariable(2);
				break;
			case RSkill:
				m_ppMaterials[ColorIcon]->UpdateShaderVariable(3);
				break;
			default:
				break;
			}

			m_ppObjects[j]->Render(pCamera);
		}
	}
}

bool CSkillShader::OnProcessKeyInput(UCHAR * pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);

	return true;
}

bool CSkillShader::OnProcessMouseInput(WPARAM pKeyBuffer)
{
	POINT cursorPos;

	GetCursorPos(&cursorPos);
	ScreenToClient(m_pCamera->GetHwnd(), &cursorPos);

	if (pKeyBuffer == MK_LBUTTON) {

		if (cursorPos.y > SKILL_MINIMUM_Y && cursorPos.y < SKILL_MAXIMUM_Y) {
		
			if ((cursorPos.x > QSKILL_MINIMUM_X  && cursorPos.x < QSKILL_MAXIMUM_X)) {
				CS_Msg_Demand_Use_Skill p;
				p.Character_id = (BYTE)m_pNetwork->m_myid;
				p.size = sizeof(p);
				p.type = CS_DEMAND_USE_SKILL;
				p.skilltype = AnimationsType::SkillQ;
				m_pNetwork->SendPacket(&p);
				return false;
			}
			if ((cursorPos.x > WSKILL_MINIMUM_X  && cursorPos.x < WSKILL_MAXIMUM_X)) {
				CS_Msg_Demand_Use_Skill p;
				p.Character_id = (BYTE)m_pNetwork->m_myid;
				p.size = sizeof(p);
				p.type = CS_DEMAND_USE_SKILL;
				p.skilltype = AnimationsType::SkillW;
				m_pNetwork->SendPacket(&p);
				return false;
			}
			if ((cursorPos.x > ESKILL_MINIMUM_X  && cursorPos.x < ESKILL_MAXIMUM_X)) {
				CS_Msg_Demand_Use_Skill p;
				p.Character_id = (BYTE)m_pNetwork->m_myid;
				p.size = sizeof(p);
				p.type = CS_DEMAND_USE_SKILL;
				p.skilltype = AnimationsType::SkillE;
				m_pNetwork->SendPacket(&p);
				return false;
			}
			if ((cursorPos.x > RSKILL_MINIMUM_X  && cursorPos.x < RSKILL_MAXIMUM_X)) {
				CS_Msg_Demand_Use_Skill p;
				p.Character_id = (BYTE)m_pNetwork->m_myid;
				p.size = sizeof(p);
				p.type = CS_DEMAND_USE_SKILL;
				p.skilltype = AnimationsType::SkillR;
				m_pNetwork->SendPacket(&p);
				return false;
			}
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////
// 내부함수
D3D12_INPUT_LAYOUT_DESC CSkillShader::CreateInputLayout()
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

D3D12_BLEND_DESC CSkillShader::CreateBlendState()
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

D3D12_SHADER_BYTECODE CSkillShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/UIShader.hlsl",
		"VSTexturedGauge",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CSkillShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/UIShader.hlsl",
		"PSTexturedIconGauge",
		"ps_5_1",
		pShaderBlob));
}

void CSkillShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;

	m_nHeaps = 8;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CSkillShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void * pContext)
{
	m_pCamera = (CCamera*)pContext;

	m_nObjects = 8;
	m_ppObjects = new CBaseObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	CreateShaderVariables(pCreateMgr, ncbElementBytes, m_nObjects);
	
	for (int i = 0; i < m_nHeaps; ++i) {
		CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects / 2, 4, i);
		CreateConstantBufferViews(pCreateMgr, m_nObjects / 2, m_pConstBuffer.Get(), ncbElementBytes, (i % 2) * (m_nObjects / 2), i);
	}
	
	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	m_incrementSize = incrementSize;
	
	CSkillObject *pUIObject{ NULL };

	m_nMaterials = m_nHeaps;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	// Stick
	m_ppMaterials[0] = Materials::CreateStickGraySkillMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[1] = Materials::CreateStickColorSkillMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);
	// Sword
	m_ppMaterials[2] = Materials::CreateSwordGraySkillMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[2], &m_psrvGPUDescriptorStartHandle[2]);
	m_ppMaterials[3] = Materials::CreateSwordColorSkillMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[3], &m_psrvGPUDescriptorStartHandle[3]);
	// Magic
	m_ppMaterials[4] = Materials::CreateMagicGraySkillMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[4], &m_psrvGPUDescriptorStartHandle[4]);
	m_ppMaterials[5] = Materials::CreateMagicColorSkillMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[5], &m_psrvGPUDescriptorStartHandle[5]);
	// Bow
	m_ppMaterials[6] = Materials::CreateBowGraySkillMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[6], &m_psrvGPUDescriptorStartHandle[6]);
	m_ppMaterials[7] = Materials::CreateBowColorSkillMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[7], &m_psrvGPUDescriptorStartHandle[7]);


	for (int i = 0; i < m_nObjects; ++i)
	{
		pUIObject = new CSkillObject(pCreateMgr, (SkillUIType)i);
		pUIObject->SetCamera(m_pCamera);
		if (i < 4)
		{
			pUIObject->SetObject(m_pPlayer);
			pUIObject->SetDistance(FRAME_BUFFER_WIDTH / 128.0128f);	// distance 9.99f
			pUIObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * i));
		}
		else
		{
			pUIObject->SetObject(m_pPlayer);
			pUIObject->SetDistance(FRAME_BUFFER_WIDTH / 128.6432f);	 // distance 9.95f	
			pUIObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[1].ptr + (incrementSize * (i - 4)));
		}

		m_ppObjects[i] = pUIObject;
	}
}

void CSkillShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			Safe_Delete(m_ppObjects[j]);
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

