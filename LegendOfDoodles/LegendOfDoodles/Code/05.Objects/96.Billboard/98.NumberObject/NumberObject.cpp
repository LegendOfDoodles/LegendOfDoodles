#include "stdafx.h"
#include "NumberObject.h"

CNumberOjbect::CNumberOjbect(shared_ptr<CCreateMgr> pCreateMgr) 
	: CBillboardObject(pCreateMgr)
{
	// 일자로 길게 전부 찍을때
	//CTexturedRectMesh *pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 85.3f, FRAME_BUFFER_HEIGHT / 360.f, 0.f);
	
	// 하나
	CTexturedRectMesh *pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 640.f, FRAME_BUFFER_HEIGHT / 360.f, 0.f);
	SetMesh(0, pRectMesh);
}

CNumberOjbect::CNumberOjbect(shared_ptr<CCreateMgr> pCreateMgr, NumberType type)
	: CBillboardObject(pCreateMgr)
{
	CTexturedRectMesh *pRectMesh = NULL;

	switch (type)
	{
	case BlueTeamKill:
	case RedTeamKill:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 640.f, FRAME_BUFFER_HEIGHT / 360.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case TimeMinute:
	case TimeSec:
	case PersonalKill:
	case PersonalDeath:
	case PersonalLevel:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 1280.f, FRAME_BUFFER_HEIGHT / 720.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	default:
		break;
	}

	m_type = type;
}

CNumberOjbect::~CNumberOjbect()
{
}

void CNumberOjbect::Animate(float fTimeElapsed)
{
	CBillboardObject::Animate(fTimeElapsed);

	// Texture Animation UV 이동

	// 위치
	XMFLOAT3 newPos{ 0, 0, 0 };

	float UpVectorMoveWeight = 1.f;
	float RightVectorMoveWeight = 1.f;

	switch (m_type)
	{
	case BlueTeamKill:
		UpVectorMoveWeight = (FRAME_BUFFER_HEIGHT / 84.f);
		RightVectorMoveWeight = -(FRAME_BUFFER_WIDTH / 1422.f) + m_fOffset;
		break;
	case RedTeamKill:
		UpVectorMoveWeight = (FRAME_BUFFER_HEIGHT / 84.f);
		RightVectorMoveWeight = (FRAME_BUFFER_WIDTH / 640.f) + m_fOffset;
		break;
	case TimeMinute:
		// center 15.8
		UpVectorMoveWeight = (FRAME_BUFFER_HEIGHT / 75.78f);
		RightVectorMoveWeight = -(FRAME_BUFFER_WIDTH / 78.5f) + m_fOffset;
		break;
	case TimeSec:
		UpVectorMoveWeight = (FRAME_BUFFER_HEIGHT / 75.78f);
		RightVectorMoveWeight = -(FRAME_BUFFER_WIDTH / 86.4f) + m_fOffset;
		break;
	case PersonalKill:
		UpVectorMoveWeight = (FRAME_BUFFER_HEIGHT / 75.78f);
		RightVectorMoveWeight = (FRAME_BUFFER_WIDTH / 96.96f) + m_fOffset;
		break;
	case PersonalDeath:
		UpVectorMoveWeight = (FRAME_BUFFER_HEIGHT / 75.78f);
		RightVectorMoveWeight = (FRAME_BUFFER_WIDTH / 84.21f) + m_fOffset;
		break;
	case PersonalLevel:
		UpVectorMoveWeight = (FRAME_BUFFER_HEIGHT / 75.78f);
		RightVectorMoveWeight = (FRAME_BUFFER_WIDTH / 74.85f) + m_fOffset;
		break;
	default:
		break;
	}

	newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
	newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), UpVectorMoveWeight)), Vector3::ScalarProduct(m_pCamera->GetRightVector(), RightVectorMoveWeight));

	m_xmf4x4World._41 = newPos.x;
	m_xmf4x4World._42 = newPos.y;
	m_xmf4x4World._43 = newPos.z;
}

void CNumberOjbect::Render(CCamera * pCamera, UINT istanceCnt)
{
	//CBillboardObject::Render(pCamera, istanceCnt);

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
}

void CNumberOjbect::SetOffset(int cnt)
{
	if (m_type == NumberType::PersonalKill || m_type == NumberType::PersonalDeath || m_type == NumberType::PersonalLevel) {
		m_fOffset = cnt * 0.4f;
	}
	else if ( m_type == NumberType::TimeMinute || m_type == NumberType::TimeSec) {
		m_fOffset = cnt * 0.5f;
	}
	else 
		m_fOffset = cnt * 0.8f;
}
