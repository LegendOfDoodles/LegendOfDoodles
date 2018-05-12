#pragma once
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"

class CCollisionObject;

class CFSMMgr
{
public:	// 생성자, 소멸자
	CFSMMgr(CWayFinder* pWayFinder);
	virtual ~CFSMMgr();

public: // 공개 함수
	void Update(float timeElapsed, CCollisionObject* obj);

protected: // 내부 함수
	void PlayIdle(float timeElapsed, CCollisionObject* obj);
	void PlayWalk(float timeElapsed, CCollisionObject* obj);
	void PlayChase(float timeElapsed, CCollisionObject* obj);
	void PlayAttack(float timeElapsed, CCollisionObject* obj);
	void PlayDie(float timeElapsed, CCollisionObject* obj);
	void PlayRemove(float timeElapsed, CCollisionObject* obj);

protected:	// 변수
	CWayFinder * m_pWayFinder;
};

