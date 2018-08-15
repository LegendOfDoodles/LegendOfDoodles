#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"

class CPlayer : public CAnimatedObject
{
public:
	CPlayer();
	virtual ~CPlayer();

public:	// 외부 함수
	virtual void Animate(float timeElapsed);

	virtual void LookAt(XMFLOAT3 objPosition);
	virtual void LookAt(XMFLOAT2 objPosition);

	virtual void ActiveSkill(AnimationsType act);

	virtual void SetState(StatesType newState);

	virtual void ChangeSkillSet(CSkeleton** ppskill);

	void SaveCurrentState();
	void Respawn();

	virtual PlayerInfo* GetPlayerStatus() { return &m_StatusInfo; }

	//virtual void SetObjectType(ObjectType type) { m_StatusInfo.WeaponType = type; };
	virtual void ReceiveDamage(float damage, CCollisionObject * pCol);
	
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

	void SendMissilePacket(FlyingObjectType type);

protected: // 변수
	PlayerInfo m_StatusInfo;
	bool m_ChangeWeapon{ false };
	UINT m_nEquipIndex[4];

	XMFLOAT4X4 m_xmf4x4SpawnWorld;	// 생성시 월드 변환 행렬
	XMFLOAT3 m_spawnLocation;	// 생성 위치

	float m_spawnCoolTime{ 0 };	// 죽은 이후 다시 생성할 때 까지 시간
};

