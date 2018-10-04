#pragma once
#include "05.Objects/08.Player/Player.h"

typedef std::list<CCollisionObject*> CollisionObjectList;

class CPlayerAI : public CPlayer
{
public:	// ������, �Ҹ���
	CPlayerAI(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes);
	virtual ~CPlayerAI();

public:	// �ܺ� �Լ�
	virtual void Animate(float timeElapsed);

	virtual void PrepareData();

	int CaculateUtility();

	virtual void SetPathes(Path* pathes) { m_pathes = pathes; }
	
protected: // ���� �Լ�
	void PlayAction(int index, float timeElapsed);

	void PushLine(float timeElapsed);
	void AttackPlayer(float timeElapsed);
	void FallBack(float timeElapsed);
	void SupportLine(float timeElapsed);

	float GetPushLineUtility();
	float GetAttackPlayerUtility();
	float GetFallBackUtility();
	float GetSupportLineUtility();

	float GetFrontLineValue();
	void ReceiveEnemyList();
	void ReceiveTeamList();
	CollisionObjectList EnemyWithinRange();

protected: // ����
	CollisionObjectList* m_plstEnemy{ NULL };
	CollisionObjectList* m_plstMyTeam{ NULL };

	CPlayer* m_pTarget_Enemy{ NULL };

	float m_refreshTimeChecker{ TIME_REFRESH_ACTION };

	int m_currentActionIndex{ 0 };

	PlayerLine m_ownLine;
	PlayerLine m_otherLine;

	Path* m_pathes;
};

