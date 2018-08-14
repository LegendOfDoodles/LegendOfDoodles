#pragma once
#include "05.Objects/02.CollisionObject/CollisionObject.h"

class CCollisionManager;

class CNexusTower : public CCollisionObject
{
public:
	CNexusTower();
	virtual ~CNexusTower();

public:	// 외부 함수
	virtual void Animate(float timeElapsed);

	virtual void SetState(StatesType newState, shared_ptr<CWayFinder> pWayFinder = nullptr);

	virtual void PlayIdle(float timeElapsed);
	virtual void PlayAttack(float timeElapsed, shared_ptr<CWayFinder> pWayFinder);
	virtual void PlayDie(float timeElapsed);

	virtual void SetMaster(CCollisionObject* masterObject);

	virtual void ReceiveDamage(float damage);

	virtual StaticInfo* GetNexusAndTowerStatus() { return &m_StatusInfo; }

protected: // 내부 함수
		   /*
		   0. Idle		1.StartWalk		2.Walking	3.Smash		4.Slash		5.Dispute
		   */

protected: // 변수
	float m_fEndTime{ 0 };

	StaticInfo m_StatusInfo;

	float m_atkCoolTime{ 0.0f };

	CCollisionObject* m_masterObject{ NULL };
};

