#include "stdafx.h"
#include "EffectObject.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CEffectObject::CEffectObject(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes)
	: CCollisionObject(pCreateMgr)
{
	m_ObjectType = ObjectType::EffectObject;

	// HP게이지 Mesh
	CTexturedRectMesh *pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 40.f, FRAME_BUFFER_HEIGHT / 22.5f, 0.f);
	SetMesh(0, pRectMesh);
}

CEffectObject::~CEffectObject()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CEffectObject::Animate(float timeElapsed)
{
	if (!m_Activated) return;

	// Billboard Update
	XMFLOAT3 xmf3Up = m_pCamera->GetUpVector();
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT3 xmf3Look = Vector3::ScalarProduct(m_pCamera->GetLookVector(), -1);
	XMFLOAT3 xmf3Right = Vector3::ScalarProduct(m_pCamera->GetRightVector(), -1);

	m_xmf4x4World._11 = xmf3Right.x;	m_xmf4x4World._12 = xmf3Right.y;	m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x;		m_xmf4x4World._22 = xmf3Up.y;		m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x;		m_xmf4x4World._32 = xmf3Look.y;

	// Effect Update
	if (m_EffectObjectType == EffectObjectType::Player_SwordSkill_Q)
	{
		m_AnimaitonTime += timeElapsed;
		if (m_AnimaitonTime > m_maxAnimaitonTime)
		{
			m_curState = StatesType::Remove;
		}
	}
}

void CEffectObject::SetDirection(const XMFLOAT3 & direction)
{
	m_direction = direction;
	LookAt(Vector3::Add(GetPosition(), Vector3::ScalarProduct(direction, 10000, false)));
}

void CEffectObject::LookAt(XMFLOAT3 target)
{
	if (m_curState == States::Attack) return;
	if (m_curState == States::Win) return;
	if (m_curState == States::Defeat) return;

	XMFLOAT3 upVector{ 0.f, 1.f, 0.f };
	XMFLOAT3 objLookVector = GetLook();
	XMFLOAT3 objPos = GetPosition();

	target.y = objPos.y;

	XMFLOAT3 towardVector = Vector3::Subtract(GetPosition(), target, true);

	float angle{ Vector3::DotProduct(towardVector, objLookVector) };
	angle = XMConvertToDegrees(acos(angle));

	if (isnan(angle)) return;

	float check{ Vector3::DotProduct(Vector3::CrossProduct(towardVector, objLookVector), upVector) };

	// 회전 방향 결정
	if (check < 0.0f)
		Rotate(0.0f, angle, 0.0f);
	else if (check > 0.0f)
		Rotate(0.0f, -angle, 0.0f);
}

void CEffectObject::LookAt(XMFLOAT2 target)
{
	LookAt(XMFLOAT3(target.x, 0, target.y));
}
