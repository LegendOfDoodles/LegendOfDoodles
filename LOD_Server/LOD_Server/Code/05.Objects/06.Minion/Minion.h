#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"

class CMinion : public CAnimatedObject
{
public: // ������, �Ҹ���
	CMinion();
	virtual ~CMinion();

public:	// �ܺ� �Լ�
	virtual void Animate(float timeElapsed);
	
	virtual void SetState(StatesType newState);

	virtual void PlayIdle(float timeElapsed);
	virtual void PlayWalk(float timeElapsed, shared_ptr<CWayFinder> pWayFinder);
	virtual void PlayChase(float timeElapsed, shared_ptr<CWayFinder> pWayFinder);
	virtual void PlayAttack(float timeElapsed, shared_ptr<CWayFinder> pWayFinder);

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

protected:	// ���� �Լ�
	virtual void AdjustAnimationIndex();

protected:	// ����

			/*
			0. Idle		1.Attack	2.Attack2	3.StartWalk		4.Walking	5.Die
			*/

	ObjectType m_ObjectType{ ObjectType::SwordMinion };

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
	CSwordMinion();
	virtual ~CSwordMinion();

public:	// �ܺ� �Լ�
	virtual void Animate(float timeElapsed);
	/*
	0. Idle		1.Attack	2.Attack2	3.StartWalk		4.Walking	5.Die
	*/
};


class CMagicMinion : public CMinion
{
public: // ������, �Ҹ���
	CMagicMinion();
	virtual ~CMagicMinion();

public:	// �ܺ� �Լ�
	virtual void Animate(float timeElapsed);
	/*
	0. Idle		1.Attack	2.Attack2	3.StartWalk		4.Walking	5.Die
	*/
};


class CBowMinion : public CMinion
{
public: // ������, �Ҹ���
	CBowMinion();
	virtual ~CBowMinion();

public:	// �ܺ� �Լ�
	virtual void Animate(float timeElapsed);

protected:	// ���� �Լ�
	virtual void AdjustAnimationIndex();
	/*
	0. Idle		1.Attack	2.Attack2	3.StartWalk		4.Walking	5.Die
	*/
};
