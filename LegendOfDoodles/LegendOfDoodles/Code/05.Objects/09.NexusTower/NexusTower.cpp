#include "stdafx.h"
#include "NexusTower.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"

/// <summary>
/// 목적: 넥서스 및 타워 관리 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-08-04
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CNexusTower::CNexusTower(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CCollisionObject(pCreateMgr, nMeshes)
{
	m_StatusInfo.maxHP = 1000;
	m_StatusInfo.HP = 1000;
	m_StatusInfo.Atk = 0;
	m_StatusInfo.Def = 0;
	m_StatusInfo.Exp = 0;
	m_sightRange = CONVERT_PaperUnit_to_InG(160.0f);
	m_detectRange = CONVERT_PaperUnit_to_InG(80.0f);

	m_attackRange = CONVERT_PaperUnit_to_InG(60.0f);
}

CNexusTower::~CNexusTower()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CNexusTower::Animate(float timeElapsed)
{
	m_atkCoolTime -= timeElapsed;
}

void CNexusTower::PlayIdle(float timeElapsed)
{
	UNREFERENCED_PARAMETER(timeElapsed);

	if (m_ObjectType == ObjectType::FirstTower)
	{
		CCollisionObject* enemy{ m_pColManager->RequestNearObject(this, m_detectRange) };

		if (!enemy) return;

		if (Attackable(enemy))
		{
			SetEnemy(enemy);
			SetState(States::Attack);
		}
	}
}

void CNexusTower::PlayAttack(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	UNREFERENCED_PARAMETER(timeElapsed);

	if (m_ObjectType == ObjectType::FirstTower)
	{
		if (!CheckEnemyState(m_pEnemy) || !Attackable(m_pEnemy))
		{
			SetEnemy(NULL);
			SetState(States::Idle);
		}

		if (m_atkCoolTime < 0.0f)
		{
			XMFLOAT3 curPos{ GetPosition() };
			curPos.y += m_fCollisionSize * 2;
			m_atkCoolTime = COOLTIME_TOWER_ATTACK;
			m_pThrowingMgr->RequestSpawn(curPos, 0, Vector3::Subtract(m_pEnemy->GetPosition(), curPos, true), m_TeamType, FlyingObjectType::Roider_Dumbel);
		}
	}
}

void CNexusTower::PlayDie(float timeElapsed)
{
	if (m_fEndTime <= 100)
	{
		m_fEndTime += 60 * timeElapsed;
		Translate(new XMFLOAT3(0, -1, 0));
		Translate(new XMFLOAT3(0, sin(m_fEndTime * 2)*1.f, 0));
		Translate(new XMFLOAT3(sin(m_fEndTime * 2)*1.f, 0, 0));
		Translate(new XMFLOAT3(0, 0, sin(m_fEndTime * 2)*1.f));
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수