#include "stdafx.h"
#include "NexusTower.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"

/// <summary>
/// 목적: 넥서스 및 타워 관리 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-10-02
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CNexusTower::CNexusTower(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CCollisionObject(pCreateMgr, nMeshes)
{
	m_StatusInfo.HP = m_StatusInfo.maxHP = 3136;
	m_StatusInfo.Atk = 295;
	m_StatusInfo.Def = 100;
	m_StatusInfo.Exp = 840;
	m_sightRange = CONVERT_PaperUnit_to_InG(160.0f);
	m_detectRange = CONVERT_PaperUnit_to_InG(80.0f);

	m_attackRange = CONVERT_PaperUnit_to_InG(78.0f);
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

void CNexusTower::SetState(StatesType newState, shared_ptr<CWayFinder> pWayFinder)
{
	if (m_curState == States::Attack && newState == States::Idle)
	{
		SetEnemy(NULL);
	}

	if (newState == States::Die)
	{
		m_StatusInfo.HP = 0.f;
	}

	if (newState == States::Defeat)
		m_curState = States::Die;
	else
		m_nextState = m_curState = newState;
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
		if (m_fEndTime > 100)
		{
			SetState(StatesType::Remove);
		}
	}
}

void CNexusTower::AttackEnemy()
{
	if (m_pEnemy)
	{
		XMFLOAT3 curPos{ GetPosition() };
		curPos.y += m_fCollisionSize * 2;
		if (m_TeamType == TeamType::Blue && m_pEnemy)
			m_pThrowingMgr->RequestSpawn(curPos, Vector3::Subtract(m_pEnemy->GetPosition(), curPos, true), m_TeamType, FlyingObjectType::BlueTower_Attack, m_StatusInfo.Atk);
		else if (m_TeamType == TeamType::Red && m_pEnemy)
			m_pThrowingMgr->RequestSpawn(curPos, Vector3::Subtract(m_pEnemy->GetPosition(), curPos, true), m_TeamType, FlyingObjectType::RedTower_Attack, m_StatusInfo.Atk);
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수