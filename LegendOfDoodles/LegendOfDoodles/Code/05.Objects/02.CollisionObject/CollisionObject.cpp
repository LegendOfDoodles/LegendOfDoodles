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
CCollisionObject::CCollisionObject(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CBaseObject(pCreateMgr, nMeshes)
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

void CCollisionObject::LevelUP(CCollisionObject* player)
{
	PlayerInfo* PlayerStatus{ player->GetPlayerStatus() };
	PlayerStatus->Level++;

	player->GetEffectManager()->RequestSpawn(player->GetPosition(), player->GetLook(), 30, EffectObjectType::Player_LevelUp_CircleEffect);
	player->GetEffectManager()->RequestSpawn(player->GetPosition(), player->GetLook(), 30, EffectObjectType::Player_LevelUp_ArrowEffect);

	if (PlayerStatus->Level == 7 || PlayerStatus->Level == 12 || PlayerStatus->Level == 17 || PlayerStatus->Level == 21) {
		PlayerStatus->SpecialPoint += 1;
	}
	if (PlayerStatus->Weapon == 1)
	{
		PlayerStatus->maxHP += 84;
		PlayerStatus->maxMP += 30;
		PlayerStatus->HP = PlayerStatus->maxHP;
		PlayerStatus->MP = PlayerStatus->maxMP;
		PlayerStatus->Atk += 4.5f;
		PlayerStatus->Def += 3;
	}
	else if (PlayerStatus->Weapon == 2)
	{
		PlayerStatus->maxHP += 90;
		PlayerStatus->maxMP += 60;
		PlayerStatus->HP = PlayerStatus->maxHP;
		PlayerStatus->MP = PlayerStatus->maxMP;
		PlayerStatus->Atk += 2.9f;
		PlayerStatus->Def += 3.4f;
	}
	else if (PlayerStatus->Weapon == 3)
	{
		PlayerStatus->maxHP += 82;
		PlayerStatus->maxMP += 50;
		PlayerStatus->HP = PlayerStatus->maxHP;
		PlayerStatus->MP = PlayerStatus->maxMP;
		PlayerStatus->Atk += 2.41f;
		PlayerStatus->Def += 3.25f;
	}
}
