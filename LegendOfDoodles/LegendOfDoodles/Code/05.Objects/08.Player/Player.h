#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"

class CPlayer : public CAnimatedObject
{
public:
	CPlayer(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes = 1);
	virtual ~CPlayer();

public:	// 외부 함수
	virtual void Animate(float timeElapsed);
	virtual void Render(CCamera *pCamera, UINT instanceCnt = 1);

	virtual void LookAt(XMFLOAT3 objPosition);
	virtual void LookAt(XMFLOAT2 objPosition);

	virtual void ActiveSkill(AnimationsType act);

	virtual void SetState(StatesType newState);
	virtual void SetHP(float maxHP, float curHP) { m_StatusInfo.maxHP = maxHP; m_StatusInfo.HP = curHP; }
	virtual void SetLevel(int level, int maxexp, int exp) { m_StatusInfo.Level = level; m_StatusInfo.MaxExp = maxexp; m_StatusInfo.Exp = exp;}
	virtual void ChangeSkillSet(CSkeleton** ppskill);

	virtual PlayerInfo* GetPlayerStatus() { return &m_StatusInfo; }

	//virtual void SetObjectType(ObjectType type) { m_StatusInfo.WeaponType = type; };
	virtual void ReceiveDamage(float damage)
	{
		// 이미 사망한 상태인 경우 대미지 처리를 하지 않는다.
		if (m_curState == States::Die || m_curState == States::Remove) { return; }
		m_StatusInfo.HP -= damage * Compute_Defence(m_StatusInfo.Def);
	}
	//virtual void ReceiveDamage(float damage) { m_StatusInfo.HP -= damage * Compute_Defence(m_StatusInfo.Def); }
	UINT GetWeaponType() { return m_StatusInfo.Weapon; }
	UINT GetWeaponNum() { return m_StatusInfo.WeaponNum; }

	void SetWeaponData(UINT type, UINT num) {
		m_StatusInfo.Weapon = type;
		m_StatusInfo.WeaponNum = num;
	}

protected: // 내부 함수
	virtual void AdjustAnimationIndex();
	/*
	0. Win		1.Defeat		2.Defeat
	3. Idle		4.StartWalk		5.Walking

	6.Smash		7.Slash		8.Dash		9.Dispute


	*/

protected: // 변수
	PlayerInfo m_StatusInfo;
};

