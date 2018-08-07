#include "stdafx.h"
#include "IconObject.h"

/// <summary>
/// 목적: Icon Object
/// 최종 수정자:  김나단
/// 수정자 목록:  이용선, 김나단
/// 최종 수정 날짜: 2018-07-03
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CIconObject::CIconObject(shared_ptr<CCreateMgr> pCreateMgr)
	: CUIFrameObject(pCreateMgr)
{
}

CIconObject::CIconObject(shared_ptr<CCreateMgr> pCreateMgr, IconUIType type)
	: CUIFrameObject(pCreateMgr)
{
	CTexturedRectMesh *pRectMesh = NULL;

	switch (type)
	{
	case PlayerIcon:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 3200.f, FRAME_BUFFER_HEIGHT / 1800.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case MinionIcon:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 12800.f, FRAME_BUFFER_HEIGHT / 7200.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case RoiderIcon:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 4266.6f, FRAME_BUFFER_HEIGHT / 2400.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case NexusAndTowerIcon:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 3200.f, FRAME_BUFFER_HEIGHT / 1800.f, 0.f);
		SetMesh(0, pRectMesh);
	default:
		break;
	}

	m_type = type;

	m_MinimapPosition = XMFLOAT3(0, 0, 0);
}

CIconObject::~CIconObject()
{

}

void CIconObject::Animate(float fTimeElapsed)
{
	CBillboardObject::Animate(fTimeElapsed);

	WorldToMinimap();

	XMFLOAT3 newPos{ 0, 0, 0 };

	newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
	newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -(FRAME_BUFFER_HEIGHT / 80.f) + m_MinimapPosition.z)), Vector3::ScalarProduct(m_pCamera->GetRightVector(), (FRAME_BUFFER_WIDTH / 134.7f) + m_MinimapPosition.x));

	m_xmf4x4World._41 = newPos.x;
	m_xmf4x4World._42 = newPos.y;
	m_xmf4x4World._43 = newPos.z;
}

void CIconObject::Render(CCamera *pCamera, UINT istanceCnt)
{

	switch (m_type)
	{
	case PlayerIcon:
	case MinionIcon:
	case RoiderIcon:
		CBillboardObject::Render(pCamera, istanceCnt);
		break;
	case NexusAndTowerIcon:
		OnPrepareRender();

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
		break;
	}

}

float CIconObject::GetCurrentHP()
{
	if (m_type == NexusAndTowerIcon)
		return (m_pMasterObject->GetNexusAndTowerStatus()->HP / m_pMasterObject->GetNexusAndTowerStatus()->maxHP);
	return 0;
}

void CIconObject::WorldToMinimap()
{
	m_MinimapPosition = m_pMasterObject->GetPosition();

	m_MinimapPosition.x = (m_MinimapPosition.x / TERRAIN_SIZE_WIDTH) * (FRAME_BUFFER_WIDTH / 160.f);
	m_MinimapPosition.z = (m_MinimapPosition.z / TERRAIN_SIZE_HEIGHT) * (FRAME_BUFFER_HEIGHT / 180.f);
}
