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
CBillboardObject::CBillboardObject(shared_ptr<CCreateMgr> pCreateMgr) : CBaseObject(pCreateMgr)
{
}

CBillboardObject::~CBillboardObject()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CBillboardObject::Animate(float fTimeElapsed)
{
	UNREFERENCED_PARAMETER(fTimeElapsed);

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

