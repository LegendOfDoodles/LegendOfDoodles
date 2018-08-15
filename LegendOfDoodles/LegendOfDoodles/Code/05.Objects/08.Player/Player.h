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

	virtual void SetState(StatesType newState, shared_ptr<CWayFinder> pWayFinder = nullptr);
	virtual void SetHP(float maxHP, float curHP);
	virtual void SetLevel(int level, int maxexp, int exp) { m_StatusInfo.Level = level; m_StatusInfo.MaxExp = maxexp; m_StatusInfo.Exp = exp;}
	virtual void ChangeSkillSet(CSkeleton** ppskill);

	void SaveCurrentState();
	virtual void Respawn();

	virtual void SetCollisionManager(shared_ptr<CCollisionManager> manager);

	virtual void RequestSpawnMissile(FlyingObjectType type);

	virtual PlayerInfo* GetPlayerStatus() { return &m_StatusInfo; }
	virtual void ReceiveSpecial(BYTE idx,  SpecialType type, bool myid = false);

	void SetWeaponChangeTriger(bool triger) { m_ChangeWeapon = triger; }
	bool GetWeaponChangeTriger() { return m_ChangeWeapon; }
	
	//virtual void SetObjectType(ObjectType type) { m_StatusInfo.WeaponType = type; };
	
	//virtual void ReceiveDamage(float damage) { m_StatusInfo.HP -= damage * Compute_Defence(m_StatusInfo.Def); }
	UINT GetWeaponType() { return m_StatusInfo.Weapon; }
	UINT GetWeaponNum() { return m_StatusInfo.WeaponNum; }

	UINT GetSpecialCnt() { return m_StatusInfo.SpecialPoint; }
	SpecialType* GetSpecialIndext() { return m_StatusInfo.Special; }
	UINT GetEquipCnt() { return EquipCnt; }

	UINT* GetEquipIndex() { return m_nEquipIndex; }


	void AddEquipCnt(UINT equiptype, UINT specialnum) {
		m_nEquipIndex[EquipCnt++] = equiptype * 4 + specialnum;
		//0123 방어 4567 특수
	}

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
	bool m_ChangeWeapon{ false };

	UINT m_nEquipIndex[4];
	UINT EquipCnt{ 0 };

	XMFLOAT4X4 m_xmf4x4SpawnWorld;	// 생성시 월드 변환 행렬
	XMFLOAT3 m_spawnLocation;	// 생성 위치
};

