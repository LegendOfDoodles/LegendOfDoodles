#include "stdafx.h"
#include "Billboard.h"

/// <summary>
/// 목적: UI 이용을 위한 Bilboard 클래스 제작
/// 최종 수정자:  김나단
/// 수정자 목록:  이용선, 김나단
/// 최종 수정 날짜: 2018-05-22
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CBillboardObject::CBillboardObject(CCreateMgr *pCreateMgr) : CBaseObject(pCreateMgr)
{
}

CBillboardObject::~CBillboardObject()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CBillboardObject::Animate(float fTimeElapsed)
{
	if (m_pCamera != NULL) {
		SetLookAt();
	}
}

void CBillboardObject::SetLookAt()
{
	// Up Vector를 1로 가정하고 계산
	XMFLOAT3 xmf3Up = m_pCamera->GetUpVector();
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT3 xmf3Look = Vector3::ScalarProduct(m_pCamera->GetLookVector(), -1);
	XMFLOAT3 xmf3Right = Vector3::ScalarProduct(m_pCamera->GetRightVector(), -1);

	// 이후 Up 벡터를 카메라의 방향으로 갱신한다.
	//xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf4x4World._11 = xmf3Right.x;	m_xmf4x4World._12 = xmf3Right.y;	m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x;		m_xmf4x4World._22 = xmf3Up.y;		m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x;		m_xmf4x4World._32 = xmf3Look.y;
}

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CUIObject::CUIObject(CCreateMgr * pCreateMgr) : CBillboardObject(pCreateMgr)
{
	m_xmf3Position = XMFLOAT3(0, 0, 0);
}

CUIObject::CUIObject(CCreateMgr * pCreateMgr, UIType type) :CBillboardObject(pCreateMgr)
{
	CTexturedRectMesh *pRectMesh = NULL;

	switch (type)
	{
	case UIType::Minimap:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 160.f, FRAME_BUFFER_HEIGHT / 180.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIType::KDA:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 458.f, FRAME_BUFFER_HEIGHT / 720.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIType::Skill:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 200.f, FRAME_BUFFER_HEIGHT / 225.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIType::Status:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 276.f, FRAME_BUFFER_HEIGHT / 225.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	default:
		break;
	}

	m_type = type;
}

CUIObject::~CUIObject()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CUIObject::Animate(float fTimeElapsed)
{
	CBillboardObject::Animate(fTimeElapsed);

	XMFLOAT3 newPos{ 0, 0, 0 };

	switch (m_type)
	{
	case Minimap:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -(FRAME_BUFFER_HEIGHT / 102.85f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), (FRAME_BUFFER_WIDTH / 94.8f)));
		break;
	case KDA:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), (FRAME_BUFFER_HEIGHT / 80.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), (FRAME_BUFFER_WIDTH / 82.5f)));
		break;
	case Skill:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -(FRAME_BUFFER_HEIGHT / 96.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), (FRAME_BUFFER_WIDTH / 1280.f)));
		break;
	case Status:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), (FRAME_BUFFER_HEIGHT / 96.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), -(FRAME_BUFFER_WIDTH / 91.4f)));
		break;
	default:
		break;
	}

	m_xmf4x4World._41 = newPos.x;
	m_xmf4x4World._42 = newPos.y;
	m_xmf4x4World._43 = newPos.z;
}

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CHPGaugeObjects::CHPGaugeObjects(CCreateMgr * pCreateMgr)
	: CBillboardObject(pCreateMgr)
{
}

CHPGaugeObjects::CHPGaugeObjects(CCreateMgr * pCreateMgr, GagueUIType type)
	: CBillboardObject(pCreateMgr)
{
	CTexturedRectMesh * pRectMesh = NULL;

	// HP게이지 Mesh
	switch (type)
	{
	case PlayerGauge:
	case MinionGauge:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 40.f, FRAME_BUFFER_HEIGHT / 144.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case NexusAndTower:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 20.6f, FRAME_BUFFER_HEIGHT / 144.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	default:
		break;
	}

	m_Type = type;
}

CHPGaugeObjects::~CHPGaugeObjects()
{

}

void CHPGaugeObjects::Animate(float fTimeElapsed)
{
	CBillboardObject::Animate(fTimeElapsed);

	if (m_Type == GagueUIType::PlayerGauge)
	{
		m_xmf4x4World._41 = m_pMasterObject->GetPosition().x;
		m_xmf4x4World._42 = m_pMasterObject->GetPosition().y + 110.f;
		m_xmf4x4World._43 = m_pMasterObject->GetPosition().z;
	}
	else if (m_Type == GagueUIType::MinionGauge)
	{
		m_xmf4x4World._41 = m_pMasterObject->GetPosition().x;
		m_xmf4x4World._42 = m_pMasterObject->GetPosition().y + 80.f;
		m_xmf4x4World._43 = m_pMasterObject->GetPosition().z;
	}
	else if (m_Type == GagueUIType::NexusAndTower) {
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

}

void CHPGaugeObjects::Render(CCamera * pCamera, UINT istanceCnt)
{
	OnPrepareRender();

	if (m_pMaterial)
	{
		m_pMaterial->Render(pCamera);
		m_pMaterial->UpdateShaderVariables();
	}

	if (m_cbvGPUDescriptorHandle.ptr)
		m_pCommandList->SetGraphicsRootDescriptorTable(8, m_cbvGPUDescriptorHandle);

	if (m_pShader)
	{
		UpdateShaderVariables();
		m_pShader->Render(pCamera);
	}

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(istanceCnt);
		}
	}
}

float CHPGaugeObjects::GetCurrentHP()
{
	if (m_Type == GagueUIType::PlayerGauge) {
		return (m_pMasterObject->GetPlayerStatus()->HP / m_pMasterObject->GetPlayerStatus()->maxHP);
	}
	else if (m_Type == GagueUIType::MinionGauge)
	{
		return (m_pMasterObject->GetCommonStatus()->HP / m_pMasterObject->GetCommonStatus()->maxHP);
	}
	else if (m_Type == GagueUIType::NexusAndTower) {
		return (m_pMasterObject->GetNexusAndTowerStatus()->HP / m_pMasterObject->GetNexusAndTowerStatus()->maxHP);
	}

}

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자

CMinimapIconObjects::CMinimapIconObjects(CCreateMgr * pCreateMgr)
	: CUIObject(pCreateMgr)
{
}

CMinimapIconObjects::CMinimapIconObjects(CCreateMgr * pCreateMgr, IconUIType type)
	: CUIObject(pCreateMgr)
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
	case NexusAndTowerIcon:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 3200.f, FRAME_BUFFER_HEIGHT / 1800.f, 0.f);
		SetMesh(0, pRectMesh);
	default:
		break;
	}

	m_MinimapPosition = XMFLOAT3(0, 0, 0);
}

CMinimapIconObjects::~CMinimapIconObjects()
{

}

void CMinimapIconObjects::Animate(float fTimeElapsed)
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

void CMinimapIconObjects::Render(CCamera *pCamera, UINT istanceCnt)
{
	CBillboardObject::Render(pCamera, istanceCnt);
}

void CMinimapIconObjects::WorldToMinimap()
{
	m_MinimapPosition = m_pMasterObject->GetPosition();

	m_MinimapPosition.x = (m_MinimapPosition.x / TERRAIN_SIZE_WIDTH) * (FRAME_BUFFER_WIDTH / 160.f);
	m_MinimapPosition.z = (m_MinimapPosition.z / TERRAIN_SIZE_HEIGHT) * (FRAME_BUFFER_HEIGHT / 180.f);
}
