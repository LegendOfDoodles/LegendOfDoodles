#include "stdafx.h"
#include "SpecialObject.h"

/// <summary>
/// 목적: 특성 선택 오브젝트
/// 최종 수정자:  이용선
/// 수정자 목록:  이용선, 
/// 최종 수정 날짜: 2018-07-11
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CSpecialObejct::CSpecialObejct(shared_ptr<CCreateMgr> pCreateMgr)
	: CBillboardObject(pCreateMgr)
{
}

CSpecialObejct::CSpecialObejct(shared_ptr<CCreateMgr> pCreateMgr, UIFrameType type)
	: CBillboardObject(pCreateMgr)
{
	CTexturedRectMesh *pRectMesh = NULL;

	switch (type)
	{
	case UIFrameType::SelectSpecial_7:
	case UIFrameType::SelectSpecial_12:
	case UIFrameType::SelectSpecial_17:
	case UIFrameType::SelectSpecial_21:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 1600.f, FRAME_BUFFER_HEIGHT / 900.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	default:
		break;
	}

	m_type = type;
}

CSpecialObejct::~CSpecialObejct()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CSpecialObejct::Animate(float fTimeElapsed)
{
	CBillboardObject::Animate(fTimeElapsed);

	// Texture Animation UV 이동

	// 위치
	XMFLOAT3 newPos{ 0, 0, 0 };

	float UpVectorMoveWeight = 1.f;
	float RightVectorMoveWeight = 1.f;

	switch (m_type)
	{
	case UIFrameType::SelectSpecial_7:
		UpVectorMoveWeight = -(FRAME_BUFFER_HEIGHT / 130.9f);
		RightVectorMoveWeight = -(FRAME_BUFFER_WIDTH / 75.29f);
		break;
	case UIFrameType::SelectSpecial_12:
		UpVectorMoveWeight = -(FRAME_BUFFER_HEIGHT / 163.6f);
		RightVectorMoveWeight = -(FRAME_BUFFER_WIDTH / 75.29f);
		break;
	case UIFrameType::SelectSpecial_17:
		UpVectorMoveWeight = -(FRAME_BUFFER_HEIGHT / 218.18f);
		RightVectorMoveWeight = -(FRAME_BUFFER_WIDTH / 75.29f);
		break;
	case UIFrameType::SelectSpecial_21:
		UpVectorMoveWeight = -(FRAME_BUFFER_HEIGHT / 327.27f);
		RightVectorMoveWeight = -(FRAME_BUFFER_WIDTH / 75.29f);
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

void CSpecialObejct::Render(CCamera * pCamera, UINT istanceCnt)
{
	switch (m_type)
	{
	case UIFrameType::SelectSpecial_7:
	case UIFrameType::SelectSpecial_12:
	case UIFrameType::SelectSpecial_17:
	case UIFrameType::SelectSpecial_21:
		CBillboardObject::Render(pCamera, istanceCnt);
		break;
	default:
		break;
	}
}
