#include "stdafx.h"
#include "CollisionObject.h"

/// <summary>
/// 목적: 충돌체가 있는 오브젝트 구분용 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-07-31
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CCollisionObject::CCollisionObject(int nMeshes) : CBaseObject(nMeshes)
{
	ResetCollisionLevel();
}

CCollisionObject::~CCollisionObject()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
bool CCollisionObject::CheckEnemyState(CCollisionObject * other)
{
	if (!other) return false;
	if (other->GetState() == States::Die) return false;
	if (other->GetState() == States::Remove) return false;
	if (m_TeamType == other->GetTeam()) return false;
	return true;
}

bool CCollisionObject::Attackable(CCollisionObject * other)
{
	float dstSqr = Vector3::DistanceSquare(GetPosition(), other->GetPosition());
	return (dstSqr < m_attackRange * m_attackRange);
}

bool CCollisionObject::AttackableFarRange(CCollisionObject * other)
{
	float dstSqr = Vector3::DistanceSquare(GetPosition(), other->GetPosition());
	return (dstSqr < m_farAttackRange * m_farAttackRange);
}
