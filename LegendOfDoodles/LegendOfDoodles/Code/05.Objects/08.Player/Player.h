#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"
#include "00.Global/01.Utility/Enumerations.h"

class CSkinnedMesh;
class CPlayer : public CAnimatedObject
{
public:
	CPlayer(CCreateMgr *pCreateMgr, int nMeshes = 1);
	virtual ~CPlayer();

public:	// 외부 함수
	virtual void Animate(float timeElapsed);
	virtual void Render(CCamera *pCamera, UINT instanceCnt = 1);

	//virtual void SetObjectType(ObjectType type) { m_StatusInfo.WeaponType = type; };

	virtual PlayerInfo* GetPlayerStatus() { return &m_StatusInfo; }

	virtual void SetMaxHP(int maxHP, int HP) { m_StatusInfo.maxHP = maxHP; m_StatusInfo.HP = HP; }
	void SetSwordMesh(CSkinnedMesh** mesh) {
		m_ppSwordMesh = mesh;
	}
	void SetStickMesh(CSkinnedMesh* mesh) {
		m_pStickMesh = mesh;
	}
	void SetWeapon(int weapon); 
protected: // 내부 함수
	virtual void AdjustAnimationIndex();
	/*
	0. Idle		1.StartWalk		2.Walking	3.Smash		4.Slash		5.Dispute 
	6. Win		7.Defeat
	*/

protected: // 변수
	PlayerInfo m_StatusInfo;
	CSkinnedMesh** m_ppSwordMesh{ NULL };
	CSkinnedMesh* m_pStickMesh{ NULL };
	int m_WeaponState{ 0 };
};

