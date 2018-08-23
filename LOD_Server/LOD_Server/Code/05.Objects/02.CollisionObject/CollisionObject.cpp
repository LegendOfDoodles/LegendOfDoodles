#include "stdafx.h"
#include "CollisionObject.h"

/// <summary>
/// ����: �浹ü�� �ִ� ������Ʈ ���п� Ŭ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-07-31
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CCollisionObject::CCollisionObject() : CBaseObject()
{
	ResetCollisionLevel();
}

CCollisionObject::~CCollisionObject()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
bool CCollisionObject::CheckEnemyState(CCollisionObject * other)
{
	if (!other) return false;
	if (other->GetState() == States::Die) {return false;}
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
	PlayerStatus->Exp -= PlayerStatus->Level * 110 + 170;
	PlayerStatus->Level++;

	if (PlayerStatus->Level == 7 || PlayerStatus->Level == 12 || PlayerStatus->Level == 17 || PlayerStatus->Level == 21) {
		PlayerStatus->SpecialPoint += 1;
	}
	if (PlayerStatus->Weapon == 1)
	{
		PlayerStatus->maxHP += 164;
		PlayerStatus->maxMP += 30;
		PlayerStatus->HP = PlayerStatus->maxHP;
		PlayerStatus->MP = PlayerStatus->maxMP;
		PlayerStatus->Atk += 9.f;
		PlayerStatus->Def += 6;
	}
	else if (PlayerStatus->Weapon == 2)
	{
		PlayerStatus->maxHP += 180;
		PlayerStatus->maxMP += 120;
		PlayerStatus->HP = PlayerStatus->maxHP;
		PlayerStatus->MP = PlayerStatus->maxMP;
		PlayerStatus->Atk += 5.f;
		PlayerStatus->Def += 6.f;
	}
	else if (PlayerStatus->Weapon == 3)
	{
		PlayerStatus->maxHP += 162;
		PlayerStatus->maxMP += 100;
		PlayerStatus->HP = PlayerStatus->maxHP;
		PlayerStatus->MP = PlayerStatus->maxMP;
		PlayerStatus->Atk += 4.41f;
		PlayerStatus->Def += 6.25f;
	}
}
