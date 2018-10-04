#include "stdafx.h"
#include "PlayerAI.h"

/// <summary>
/// ����: �÷��̾� AI ���� Ŭ����
/// ���� ������:  �質��
/// ������ ���:  ������, �質��
/// ���� ���� ��¥: 2018-10-04
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CPlayerAI::CPlayerAI() : CPlayer()
{
}

CPlayerAI::~CPlayerAI()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CPlayerAI::Animate(float timeElapsed)
{
	CPlayer::Animate(timeElapsed);
	m_refreshTimeChecker += timeElapsed;

	// ����� ������ ��� ���� �ൿ�� ����� �ʿ䰡 �����Ƿ�
	if (m_curState == States::Die || m_curState == States::Remove) return;

	if (m_refreshTimeChecker >= TIME_REFRESH_ACTION) 
	{
		m_currentActionIndex = CaculateUtility();
		m_refreshTimeChecker = 0;
	}
	PlayAction(m_currentActionIndex, timeElapsed);
}

void CPlayerAI::PrepareData()
{
	XMFLOAT3 curPos{ GetPosition() };
	if (curPos.z > TERRAIN_SIZE_HEIGHT * 0.5f)
	{
		m_ownLine = PlayerLine::TopLine;
		m_otherLine = PlayerLine::BottomLine;
	}
	else
	{
		m_ownLine = PlayerLine::BottomLine;
		m_otherLine = PlayerLine::TopLine;
	}
}

int CPlayerAI::CaculateUtility()
{
	ReceiveEnemyList();
	ReceiveTeamList();

	// ���� ū ȿ�� Ȯ�ο� ����
	PlayerAIActions biggestUtility{ PlayerAIActions::Push_Line };
	float maxUtility{ GetPushLineUtility() };
	float curUtility{ GetAttackPlayerUtility() };

	if (maxUtility < curUtility)
	{
		biggestUtility = PlayerAIActions::Attack_Player;
		maxUtility = curUtility;
	}
	
	curUtility = GetFallBackUtility();
	if (maxUtility < curUtility)
	{
		biggestUtility = PlayerAIActions::Fall_Back;
		maxUtility = curUtility;
	}

	curUtility = GetSupportLineUtility();
	if (maxUtility < curUtility)
	{
		biggestUtility = PlayerAIActions::Support_Line;
	}

	return static_cast<int>(biggestUtility);
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CPlayerAI::PlayAction(int index, float timeElapsed)
{
	switch (index) {
	case Push_Line:
		PushLine(timeElapsed);
		break;
	case Attack_Player:
		AttackPlayer(timeElapsed);
		break;
	case Fall_Back:
		FallBack(timeElapsed);
		break;
	case Support_Line:
		SupportLine(timeElapsed);
		break;
	}
}

void CPlayerAI::PushLine(float timeElapsed)
{
	UNREFERENCED_PARAMETER(timeElapsed);
}

void CPlayerAI::AttackPlayer(float timeElapsed)
{
	UNREFERENCED_PARAMETER(timeElapsed);
}

void CPlayerAI::FallBack(float timeElapsed)
{
	UNREFERENCED_PARAMETER(timeElapsed);
}

void CPlayerAI::SupportLine(float timeElapsed)
{
	UNREFERENCED_PARAMETER(timeElapsed);
}

float CPlayerAI::GetPushLineUtility()
{
	return 0.0f;
}

float CPlayerAI::GetAttackPlayerUtility()
{
	//m_pTarget_Enemy = NULL;

	//CollisionObjectList EnemiesInRange = EnemyWithinRange();
	//if (EnemiesInRange.size() == 0) {
	//	m_arrUtilities[biggestUtilityIndex++] = 0;
	//}
	//else {
	//	for (auto i = EnemiesInRange.begin(); i != EnemiesInRange.end(); ++i) {
	//		if (!m_pTarget_Enemy) {
	//			m_pTarget_Enemy = dynamic_cast<CPlayer*>(*i);
	//		}
	//		else if (m_pTarget_Enemy->GetPlayerStatus()->HP > dynamic_cast<CPlayer*>(*i)->GetPlayerStatus()->HP) {
	//			m_pTarget_Enemy = dynamic_cast<CPlayer*>(*i);
	//		}
	//	}
	//	XMFLOAT2 apos = XMFLOAT2(m_pTarget_Enemy->GetPosition().x, m_pTarget_Enemy->GetPosition().z);
	//	XMFLOAT2 bpos = XMFLOAT2(GetPosition().x, GetPosition().z);

	//	m_arrUtilities[biggestUtilityIndex++] = CLAMP(2 * m_StatusInfo.Atk / m_pTarget_Enemy->GetPlayerStatus()->HP, 0, 1);
	//}
	return 0.0f;
}

float CPlayerAI::GetFallBackUtility()
{
	//float fighting_value = 0;
	//if (EnemiesInRange.size() == 0) {
	//	m_arrUtilities[biggestUtilityIndex++] = CLAMP((m_StatusInfo.maxHP - m_StatusInfo.HP) / m_StatusInfo.maxHP, 0, 1);
	//}
	//else {
	//	int cnt = 0;
	//	for (auto i = m_plstEnemy->begin(); i != m_plstEnemy->end(); ++i) {
	//		if ((*i)->GetType() < 4) {
	//			PlayerInfo* info = (*i)->GetPlayerStatus();
	//			fighting_value += Compute_Fighting_Value(info->HP, info->Atk, info->Def);
	//			cnt++;
	//		}
	//		else if ((*i)->GetType() < 12) {
	//			CommonInfo* info = (*i)->GetCommonStatus();
	//			fighting_value += Compute_Fighting_Value(info->HP, info->Atk, info->Def);
	//			cnt++;
	//		}
	//	}
	//	fighting_value = (fighting_value / cnt)*(1 + cnt * 0.1f);
	return 0.0f;
}

float CPlayerAI::GetSupportLineUtility()
{
	//	m_arrUtilities[biggestUtilityIndex++] = CLAMP((m_StatusInfo.maxHP - m_StatusInfo.HP) / m_StatusInfo.maxHP, 0, 1)
	//		* CLAMP(Compute_Fighting_Value(m_StatusInfo.HP, m_StatusInfo.Atk, m_StatusInfo.Def), 0, 1);
	//}

	//float lineDistance = GetFrontLineValue();

	//float otherlinesupport{ (abs(lineDistance) / 3000.0f) }; // x��ǥ�� 3000���� �̻��� �� �ִ�
	return 0.0f;
}

float CPlayerAI::GetFrontLineValue()
{
	//������ ��� bottom���� �켼
	XMFLOAT2 curLineFrontPos{ m_pColManager->GetFrontLinePosition(m_ownLine, m_TeamType) };
	XMFLOAT2 otherLineFrontPos{ m_pColManager->GetFrontLinePosition(m_otherLine, m_TeamType) };

	float lineDistance{ 0.f };

	if (m_TeamType == TeamType::Blue) 
	{
		lineDistance = (curLineFrontPos.x) - (otherLineFrontPos.x);
	}
	else if (m_TeamType == TeamType::Red)
	{
		lineDistance = (TERRAIN_SIZE_WIDTH - curLineFrontPos.x) - (TERRAIN_SIZE_WIDTH - otherLineFrontPos.x);
	}

	return lineDistance;
}

void CPlayerAI::ReceiveEnemyList()
{
	m_plstEnemy = m_pColManager->GetEnemyList(m_TeamType);
}

void CPlayerAI::ReceiveTeamList()
{
	m_plstMyTeam = m_pColManager->GetTeamList(this, m_TeamType);
}

std::list<CCollisionObject*> CPlayerAI::EnemyWithinRange()
{
	std::list<CCollisionObject*>::iterator i;
	std::list<CCollisionObject*> returnList;
	returnList.clear();
	for (i = m_plstEnemy->begin(); i != m_plstEnemy->end(); ++i)
	{
		if ((*i)->GetType() < 4) {
			XMFLOAT2 apos = XMFLOAT2((*i)->GetPosition().x, (*i)->GetPosition().z);
			XMFLOAT2 bpos = XMFLOAT2(GetPosition().x, GetPosition().z);
			float distance = Vector2::Distance(apos, bpos);
			if(distance<= m_sightRange)
				returnList.push_back((*i));
		}
	}
	return returnList;
}
