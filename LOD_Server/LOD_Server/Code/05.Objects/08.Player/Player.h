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

	virtual ProcessType MoveToDestination(float timeElapsed, shared_ptr<CWayFinder> pWayFinder = NULL);
	virtual void MoveToSubDestination(float timeElapsed, shared_ptr<CWayFinder> pWayFinder = NULL);

	virtual void ActiveSkill(AnimationsType act);

	virtual void SetState(StatesType newState);

	virtual void ChangeSkillSet(CSkeleton** ppskill);

	void SaveCurrentState();
	void Respawn();

	virtual void ApplyExp(UINT addExp);

	virtual void SendCoolTime(int id);

	virtual PlayerInfo* GetPlayerStatus() { return &m_StatusInfo; }

	//virtual void SetObjectType(ObjectType type) { m_StatusInfo.WeaponType = type; };
	virtual void ReceiveDamage(float damage, CCollisionObject * pCol);

	virtual void ChangeWeapon(UINT weaponNum, ObjectType type);

	virtual void ApplySpecialStat(SpecialType curSP);

	void ApplySwordSP(SpecialType curSP);
	void ApplyBowSP(SpecialType curSP);
	void ApplyStaffSP(SpecialType curSP);

	void SendSpeedPacket(SpeedType type);
	void SendSightPacket();
	void SendRangePacket(RangeType type);
	
	//virtual void ReceiveDamage(float damage) { m_StatusInfo.HP -= damage * Compute_Defence(m_StatusInfo.Def); }
	UINT GetWeaponType() { return m_StatusInfo.Weapon; }
	UINT GetWeaponNum() { return m_StatusInfo.WeaponNum; }

	virtual void PrepareData() {}
	virtual void SetPathes(Path* pathes) { pathes; }

protected: // 내부 함수
	virtual void AdjustAnimationIndex();

	virtual bool Heal(float timeElapsed);
	/*
	0. Win		1.Defeat		2.Defeat
	3. Idle		4.StartWalk		5.Walking

	6.Smash		7.Slash		8.Dash		9.Dispute


	*/

protected: // 변수
	PlayerInfo m_StatusInfo;
	bool m_ChangeWeapon{ false };
	UINT m_nEquipIndex[4];

	XMFLOAT4X4 m_xmf4x4SpawnWorld;	// 생성시 월드 변환 행렬
	XMFLOAT3 m_spawnLocation;	// 생성 위치

	float m_spawnCoolTime{ 0 };	// 죽은 이후 다시 생성할 때 까지 시간

	float m_skillCoolTimeQ{ COOLTIME_SKILL_Q };
	float m_skillCoolTimeW{ COOLTIME_SKILL_W };
	float m_skillCoolTimeE{ COOLTIME_SKILL_E };
	float m_skillCoolTimeR{ COOLTIME_SKILL_R };
};

