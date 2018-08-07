#include "stdafx.h"
#include "FlyingObject.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"

/// <summary>
/// 목적: 날아가는(스킬, 화살 등) 오브젝트 처리 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-08-03
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CFlyingObject::CFlyingObject(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CCollisionObject(pCreateMgr, nMeshes)
{
	m_ObjectType = ObjectType::FlyingObject;
}

CFlyingObject::~CFlyingObject()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CFlyingObject::Animate(float timeElapsed)
{
	if (!m_Activated) return;

	if (m_flyingObjectType == FlyingObjectType::Roider_Dumbel ||
		m_flyingObjectType == FlyingObjectType::Minion_Arrow)
	{
		m_distance += timeElapsed * m_speed;
		MoveToDirection(timeElapsed * m_speed);
		m_pColManager->RequestCollide(CollisionType::SPHERE, this, 0, m_attackRange, m_damage * timeElapsed);
		if (m_distance > m_maxDistance)
		{
			m_curState = StatesType::Remove;
		}
	}
	else if (m_flyingObjectType == FlyingObjectType::Minion_Magic)
	{
		m_distance += timeElapsed * m_speed;
		MoveToDirection(timeElapsed * m_speed);
		m_pColManager->RequestCollide(CollisionType::SPHERE, this, 0, m_attackRange, m_damage * timeElapsed);
		Rotate(0, 0, 10.0f);
		if (m_distance > m_maxDistance)
		{
			m_curState = StatesType::Remove;
		}
	}
}

void CFlyingObject::SetFlyingObjectsType(FlyingObjectType type)
{
	m_flyingObjectType = type;
	m_curState = StatesType::Idle;

	if (type == FlyingObjectType::Roider_Dumbel)
	{
		m_attackRange = CONVERT_PaperUnit_to_InG(3);
		m_damage = 10.0f;
		m_distance = 0.0f;
		m_maxDistance = CONVERT_PaperUnit_to_InG(30);
		m_speed = CONVERT_cm_to_InG(1.805f);
	}
	else if (type == FlyingObjectType::Minion_Arrow)
	{
		m_attackRange = CONVERT_PaperUnit_to_InG(2);
		m_damage = 10.0f;
		m_distance = 0.0f;
		m_maxDistance = CONVERT_PaperUnit_to_InG(30);
		m_speed = CONVERT_cm_to_InG(1.805f);
	}
	else if (type == FlyingObjectType::Minion_Magic)
	{
		m_attackRange = CONVERT_PaperUnit_to_InG(2);
		m_damage = 12.0f;
		m_distance = 0.0f;
		m_maxDistance = CONVERT_PaperUnit_to_InG(24);
		m_speed = CONVERT_cm_to_InG(1.805f);
	}
}

void CFlyingObject::SetDirection(const XMFLOAT3 & direction)
{
	m_direction = direction;
	LookAt(Vector3::Add(GetPosition(), Vector3::ScalarProduct(direction, 10000, false)));
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CFlyingObject::LookAt(XMFLOAT3 target)
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

void CFlyingObject::LookAt(XMFLOAT2 target)
{
	LookAt(XMFLOAT3(target.x, 0, target.y));
}

void CFlyingObject::MoveToDirection(float dist)
{
	CBaseObject::SetPosition(Vector3::Add(GetPosition(), m_direction, dist));
}
