#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"

class CMinion : public CAnimatedObject
{
public: // 생성자, 소멸자
	CMinion();
	virtual ~CMinion();

public:	// 외부 함수
	virtual void Animate(float timeElapsed);
	
	virtual void SetState(StatesType newState);

	virtual void PlayIdle(float timeElapsed);
	virtual void PlayWalk(float timeElapsed, shared_ptr<CWayFinder> pWayFinder);
	virtual void PlayChase(float timeElapsed, shared_ptr<CWayFinder> pWayFinder);
	virtual void PlayAttack(float timeElapsed, shared_ptr<CWayFinder> pWayFinder);

	virtual void ReceiveDamage(float damage, CCollisionObject * pCol);

	virtual CommonInfo* GetCommonStatus() { return &m_StatusInfo; };
	virtual void SetCommonStatus(CommonInfo* status);

protected:	// 내부 함수
	virtual void AdjustAnimationIndex();

protected:	// 변수

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
public: // 생성자, 소멸자
	CSwordMinion();
	virtual ~CSwordMinion();

public:	// 외부 함수
	virtual void Animate(float timeElapsed);
	/*
	0. Idle		1.Attack	2.Attack2	3.StartWalk		4.Walking	5.Die
	*/
};


class CMagicMinion : public CMinion
{
public: // 생성자, 소멸자
	CMagicMinion();
	virtual ~CMagicMinion();

public:	// 외부 함수
	virtual void Animate(float timeElapsed);
	/*
	0. Idle		1.Attack	2.Attack2	3.StartWalk		4.Walking	5.Die
	*/
};


class CBowMinion : public CMinion
{
public: // 생성자, 소멸자
	CBowMinion();
	virtual ~CBowMinion();

public:	// 외부 함수
	virtual void Animate(float timeElapsed);

protected:	// 내부 함수
	virtual void AdjustAnimationIndex();
	/*
	0. Idle		1.Attack	2.Attack2	3.StartWalk		4.Walking	5.Die
	*/
};
