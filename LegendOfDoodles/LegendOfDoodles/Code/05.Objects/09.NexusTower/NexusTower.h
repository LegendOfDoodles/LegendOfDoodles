#pragma once
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "00.Global/01.Utility/Enumerations.h"

class CCollisionManager;

class CNexusTower : public CCollisionObject
{
public:
	CNexusTower(CCreateMgr *pCreateMgr, int nMeshes = 1);
	virtual ~CNexusTower();

public:	// 외부 함수
	virtual void Animate(float timeElapsed);

protected: // 내부 함수
	/*
	0. Idle		1.StartWalk		2.Walking	3.Smash		4.Slash		5.Dispute
	*/

protected: // 변수
	float m_fEndTime{ 0 };
	ObjectType type;
};

