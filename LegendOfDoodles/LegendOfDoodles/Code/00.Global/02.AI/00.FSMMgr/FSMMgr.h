#pragma once
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"

class CCollisionObject;

class CFSMMgr
{
public:	// 생성자, 소멸자
	CFSMMgr(shared_ptr<CWayFinder> pWayFinder);
	virtual ~CFSMMgr();

public: // 공개 함수
	void Update(float timeElapsed, CCollisionObject* obj);

protected: // 내부 함수
	void PlayIdle(float timeElapsed, CCollisionObject* obj) { timeElapsed;	obj; }
	void PlayWalk(float timeElapsed, CCollisionObject* obj) { timeElapsed;	obj; }
	void PlayChase(float timeElapsed, CCollisionObject* obj) { timeElapsed;	obj; }
	void PlayAttack(float timeElapsed, CCollisionObject* obj) { timeElapsed;	obj; }
	void PlayDie(float timeElapsed, CCollisionObject* obj) { timeElapsed;	obj; }
	void PlayRemove(float timeElapsed, CCollisionObject* obj) { timeElapsed;	obj; }

protected:	// 변수
	shared_ptr<CWayFinder> m_pWayFinder;
};

