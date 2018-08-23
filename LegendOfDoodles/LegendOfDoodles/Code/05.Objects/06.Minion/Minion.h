#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"

class CMinion : public CAnimatedObject
{
public: // ������, �Ҹ���
	CMinion(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes = 1);
	virtual ~CMinion();

public:	// �ܺ� �Լ�
	virtual void Animate(float timeElapsed = 0.0f);
	virtual void Render(CCamera *pCamera, UINT instanceCnt = 1);

	virtual void SetState(StatesType newState, shared_ptr<CWayFinder> pWayFinder = nullptr);

	virtual void PlayWalk(float timeElapsed, shared_ptr<CWayFinder> pWayFinder);
	virtual void PlayChase(float timeElapsed, shared_ptr<CWayFinder> pWayFinder);

	virtual void ReceiveDamage(float damage)
	{
		// �̹� ����� ������ ��� ����� ó���� ���� �ʴ´�.
		if (m_curState == States::Die || m_curState == States::Remove) { return; }

		m_StatusInfo.HP -= damage * Compute_Defence(m_StatusInfo.Def);
		if (m_StatusInfo.HP <= 0) {
			SetState(States::Die);
		}

	}

	virtual CommonInfo* GetCommonStatus() { return &m_StatusInfo; };
	virtual void SetHP(float maxHP, float curHP) { m_StatusInfo.maxHP = maxHP;  m_StatusInfo.HP = curHP; }

protected:	// ���� �Լ�
	virtual void AdjustAnimationIndex();

protected:	// ����

			/*
			0. Idle		1.Attack	2.Attack2	3.StartWalk		4.Walking	5.Die
			*/

	/*
	0. SwordPlayer, 1. StaffPlayer,   2. BowPlayer,
	3. SwordMinion,	4. StaffMinion,   5. BowMinion,
	6. Roider,   	7. CAM,     	  8. GOLEM,
	9. FirstTower,  10. SecnondTower, 11. Nexus
	*/

	CommonInfo m_StatusInfo;
};

class CSwordMinion : public CMinion
{
public: // ������, �Ҹ���
	CSwordMinion(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes = 1);
	virtual ~CSwordMinion();

public:	// �ܺ� �Լ�
	virtual void Animate(float timeElapsed = 0.0f);
	/*
	0. Idle		1.Attack	2.Attack2	3.StartWalk		4.Walking	5.Die
	*/
};


class CMagicMinion : public CMinion
{
public: // ������, �Ҹ���
	CMagicMinion(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes = 1);
	virtual ~CMagicMinion();

public:	// �ܺ� �Լ�
	virtual void Animate(float timeElapsed = 0.0f);
	/*
	0. Idle		1.Attack	2.Attack2	3.StartWalk		4.Walking	5.Die
	*/
};


class CBowMinion : public CMinion
{
public: // ������, �Ҹ���
	CBowMinion(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes = 1);
	virtual ~CBowMinion();

public:	// �ܺ� �Լ�
	virtual void Animate(float timeElapsed = 0.0f);

protected:	// ���� �Լ�
	virtual void AdjustAnimationIndex();
	/*
	0. Idle		1.Attack	2.Attack2	3.StartWalk		4.Walking	5.Die
	*/
};
