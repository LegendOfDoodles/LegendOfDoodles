#include "stdafx.h"
#include "NexusTower.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"

/// <summary>
/// 목적: 넥서스 및 타워 관리 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-08-07
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CNexusTower::CNexusTower() : CCollisionObject()
{
	m_StatusInfo.maxHP = 1000;
	m_StatusInfo.HP = 1000;
	m_StatusInfo.Atk = 1000;
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
	m_hpSyncCoolTime += timeElapsed;
	m_atkCoolTime -= timeElapsed;
}

void CNexusTower::SetState(StatesType newState, shared_ptr<CWayFinder> pWayFinder)
{
	if (m_curState == States::Attack && newState == States::Idle)
	{
		SetEnemy(NULL);
	}

	m_nextState = m_curState = newState;

	if (m_curState == States::Remove) return;
	else if (m_curState == States::Die)
	{
		if(m_masterObject) m_masterObject->MakeMortal();
	}

	for (int i = 0; i < MAX_USER; ++i)
	{
		if (g_clients[i].m_isconnected) {
			SC_Msg_Set_Nexus_State p;
			p.Building_State = (BYTE)newState;
			p.Building_Tag = (short)m_tag;
			p.size = sizeof(p);
			p.type = SC_NEXUS_STATE;
			SendPacket(i, &p);
		}
	}
}

void CNexusTower::PlayIdle(float timeElapsed)
{
	UNREFERENCED_PARAMETER(timeElapsed);

	if (m_ObjectType == ObjectType::FirstTower)
	{
		CCollisionObject* enemy{ m_pColManager->RequestNearObject(this, m_detectRange, m_TeamType) };

		if (!enemy) return;

		if (Attackable(enemy))
		{
			SetEnemy(enemy);
			for (int i = 0; i < MAX_USER; ++i)
			{
				if (g_clients[i].m_isconnected)
				{
					SC_Msg_Enemy_Tag_Nexus p;
					p.Building_Tag = (short)m_tag;
					p.Enemy_Tag = (short)enemy->GetTag();
					p.size = sizeof(p);
					p.type = SC_BUILDING_SET_ENEMY;
					SendPacket(i, &p);
				}
			}
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
		else if (m_atkCoolTime < 0.0f)
		{
			XMFLOAT3 curPos{ GetPosition() };
			curPos.y += m_fCollisionSize * 2;
			m_atkCoolTime = COOLTIME_TOWER_ATTACK;
			if (m_TeamType == TeamType::Blue)
				m_pThrowingMgr->RequestSpawn(curPos, Vector3::Subtract(m_pEnemy->GetPosition(), curPos, true), m_TeamType, FlyingObjectType::BlueTower_Attack, m_StatusInfo.Atk);
			else if (m_TeamType == TeamType::Red)
				m_pThrowingMgr->RequestSpawn(curPos, Vector3::Subtract(m_pEnemy->GetPosition(), curPos, true), m_TeamType, FlyingObjectType::RedTower_Attack, m_StatusInfo.Atk);
			for (int i = 0; i < MAX_USER; ++i)
			{
				if (g_clients[i].m_isconnected)
				{
					SC_Msg_Building_Attack_Enemy p;
					p.Building_Tag = (short)m_tag;
					p.size = sizeof(p);
					p.type = SC_BUILDING_ATTACK;
					SendPacket(i, &p);
				}
			}
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
		if (m_fEndTime > 100)
		{
			SetState(StatesType::Remove);
		}
	}
}

void CNexusTower::SetMaster(CCollisionObject * masterObject)
{
	m_masterObject = masterObject;
	m_masterObject->MakeImmortal();
}

void CNexusTower::ReceiveDamage(float damage)
{
	// 이미 사망한 상태인 경우 대미지 처리를 하지 않는다.
	if (m_bImmortal || m_curState == States::Die || m_curState == States::Remove) { return; }

	m_StatusInfo.HP -= damage * Compute_Defence(m_StatusInfo.Def);

	if (m_StatusInfo.HP <= 0 && m_pEnemy) {
		PlayerInfo* PlayerStatus{ m_pEnemy->GetPlayerStatus() };
		if (m_pEnemy->GetTag() >= 10000 && m_pEnemy->GetTag() < 20000)
		{
			PlayerStatus->Exp += 100 + (g_GameTime / 60000) * 5;
			if (PlayerStatus->Level * 110 + 170 <= PlayerStatus->Exp) {
				PlayerStatus->Exp -= PlayerStatus->Level * 110 + 170;
				m_pEnemy->LevelUP(m_pEnemy);
				SC_Msg_Level_Up p;
				p.Target_Tag = m_pEnemy->GetTag();
				p.level = PlayerStatus->Level;
				p.size = sizeof(p);
				p.type = SC_LEVEL_UP;
				for (int j = 0; j < MAX_USER; ++j) {
					if (g_clients[j].m_isconnected == true) {
						SendPacket(j, &p);
					}
				}
			}
			SC_Msg_Exp_Up p;
			p.Target_Tag = m_pEnemy->GetTag();
			p.exp = 100 + (g_GameTime / 60000) * 5;
			p.size = sizeof(p);
			p.type = SC_EXP_UP;
			for (int j = 0; j < MAX_USER; ++j) {
				if (g_clients[j].m_isconnected == true) {
					SendPacket(j, &p);
				}
			}
		}
	}

	if (m_hpSyncCoolTime > COOLTIME_HP_SYNC)
	{
		SC_Msg_Hp_Sync hpPacket;
		hpPacket.curhp = m_StatusInfo.HP;
		hpPacket.maxhp = m_StatusInfo.maxHP;
		hpPacket.size = sizeof(hpPacket);
		hpPacket.type = SC_HP_SYNC;
		hpPacket.Target_Tag = (short)m_tag;
		hpPacket.updatetime = g_GameTime;
		for (int j = 0; j < MAX_USER; ++j) {
			if (g_clients[j].m_isconnected == true) {
				SendPacket(j, &hpPacket);
			}
		}
		m_hpSyncCoolTime = 0.0f;
	}

	if (m_StatusInfo.HP <= 0 && m_curState != States::Die) {
		SetState(States::Die);
		if (m_ObjectType == ObjectType::Nexus) {
			m_pColManager->GameOver(m_TeamType);
			for (int i = 0; i < MAX_USER; ++i)
			{
				if (g_clients[i].m_isconnected) {
					SC_Msg_Game_Over p;
					p.Team_Type = (BYTE)m_TeamType;
					p.size = sizeof(p);
					p.type = SC_GAME_OVER;
					SendPacket(i, &p);
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수