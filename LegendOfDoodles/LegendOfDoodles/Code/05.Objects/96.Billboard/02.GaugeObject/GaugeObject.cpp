#include "stdafx.h"
#include "GaugeObject.h"

/// <summary>
/// 목적: GaugeObject 
/// 최종 수정자:  김나단
/// 수정자 목록:  이용선, 김나단
/// 최종 수정 날짜: 2018-07-03
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CGaugeObject::CGaugeObject(shared_ptr<CCreateMgr> pCreateMgr)
	: CBillboardObject(pCreateMgr)
{
}

CGaugeObject::CGaugeObject(shared_ptr<CCreateMgr> pCreateMgr, GaugeUIType type)
	: CBillboardObject(pCreateMgr)
{
	CTexturedRectMesh * pRectMesh = NULL;

	// HP게이지 Mesh
	switch (type)
	{
	case PlayerGauge:
	case MinionGauge:
	case RoiderGauge:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 40.f, FRAME_BUFFER_HEIGHT / 144.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case NexusAndTower:
	case GolemGauge:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 20.6f, FRAME_BUFFER_HEIGHT / 144.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	default:
		break;
	}

	m_Type = type;
}

CGaugeObject::~CGaugeObject()
{

}

void CGaugeObject::Animate(float fTimeElapsed)
{
	CBillboardObject::Animate(fTimeElapsed);

	if (m_Type == GaugeUIType::PlayerGauge)
	{
		m_xmf4x4World._41 = m_pMasterObject->GetPosition().x;
		m_xmf4x4World._42 = m_pMasterObject->GetPosition().y + 110.f;
		m_xmf4x4World._43 = m_pMasterObject->GetPosition().z;
	}
	else if (m_Type == GaugeUIType::MinionGauge)
	{
		m_xmf4x4World._41 = m_pMasterObject->GetPosition().x;
		m_xmf4x4World._42 = m_pMasterObject->GetPosition().y + 80.f;
		m_xmf4x4World._43 = m_pMasterObject->GetPosition().z;
	}
	else if (m_Type == GaugeUIType::NexusAndTower) {
		if (m_MasterObjectType == ObjectType::Nexus) {
			m_xmf4x4World._41 = m_pMasterObject->GetPosition().x;
			m_xmf4x4World._42 = m_pMasterObject->GetPosition().y + 300.f;
			m_xmf4x4World._43 = m_pMasterObject->GetPosition().z;
		}
		else {
			m_xmf4x4World._41 = m_pMasterObject->GetPosition().x;
			m_xmf4x4World._42 = m_pMasterObject->GetPosition().y + 200.f;
			m_xmf4x4World._43 = m_pMasterObject->GetPosition().z;
		}
	}
	else if (m_Type == GaugeUIType::RoiderGauge) {
		m_xmf4x4World._41 = m_pMasterObject->GetPosition().x;
		m_xmf4x4World._42 = m_pMasterObject->GetPosition().y + 120.f;
		m_xmf4x4World._43 = m_pMasterObject->GetPosition().z;
	}
	else if (m_Type == GaugeUIType::GolemGauge) {
		m_xmf4x4World._41 = m_pMasterObject->GetPosition().x;
		m_xmf4x4World._42 = m_pMasterObject->GetPosition().y + 250.f;
		m_xmf4x4World._43 = m_pMasterObject->GetPosition().z;
	}
}

void CGaugeObject::Render(CCamera * pCamera, UINT istanceCnt)
{
	OnPrepareRender();
	if (!IsVisible(pCamera) || !m_pMasterObject->GetDetected()) return;

	if (m_pMaterial)
	{
		m_pMaterial->Render(pCamera);
		m_pMaterial->UpdateShaderVariables();
	}

	if (m_cbvGPUDescriptorHandle.ptr)
		m_pCommandList->SetGraphicsRootDescriptorTable(7, m_cbvGPUDescriptorHandle);

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(istanceCnt);
		}
	}
}

float CGaugeObject::GetCurrentHP()
{
	if (m_Type == GaugeUIType::PlayerGauge) {
		return (m_pMasterObject->GetPlayerStatus()->HP / m_pMasterObject->GetPlayerStatus()->maxHP);
	}
	else if (m_Type == GaugeUIType::MinionGauge || m_Type == GaugeUIType::RoiderGauge || m_Type == GaugeUIType::GolemGauge)
	{
		return (m_pMasterObject->GetCommonStatus()->HP / m_pMasterObject->GetCommonStatus()->maxHP);
	}
	else if (m_Type == GaugeUIType::NexusAndTower) {
		return (m_pMasterObject->GetNexusAndTowerStatus()->HP / m_pMasterObject->GetNexusAndTowerStatus()->maxHP);
	}
	return 0;
}
