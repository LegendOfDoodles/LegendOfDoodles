#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"
#include "00.Global/01.Utility/Enumerations.h"

class CPlayer : public CAnimatedObject
{
public:
	CPlayer();
	virtual ~CPlayer();

public:	// �ܺ� �Լ�
	virtual void Animate(float timeElapsed);
	
	virtual void ActiveSkill(AnimationsType act);

	virtual void SetState(StatesType newState);

	//virtual void SetObjectType(ObjectType type) { m_StatusInfo.WeaponType = type; };
	virtual void ReceiveDamage(float damage) { m_StatusInfo.HP -= damage * Compute_Defence(m_StatusInfo.Def); }

	virtual PlayerInfo* GetPlayerStatus() { return &m_StatusInfo; }


protected: // ���� �Լ�
	virtual void AdjustAnimationIndex();
	/*
	0. Idle		1.StartWalk		2.Walking	3.Smash		4.Slash		5.Dispute 
	6. Win		7.Defeat
	*/

protected: // ����
	PlayerInfo m_StatusInfo;
};

