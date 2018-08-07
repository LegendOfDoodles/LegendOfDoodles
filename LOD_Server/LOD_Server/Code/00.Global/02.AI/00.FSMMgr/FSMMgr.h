#pragma once
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"

class CCollisionObject;

class CFSMMgr
{
public:	// ������, �Ҹ���
	CFSMMgr(shared_ptr<CWayFinder> pWayFinder);
	virtual ~CFSMMgr();

public: // ���� �Լ�
	void Update(float timeElapsed, CCollisionObject* obj);

protected: // ���� �Լ�
	void PlayIdle(float timeElapsed, CCollisionObject* obj) { timeElapsed;	obj; }
	void PlayWalk(float timeElapsed, CCollisionObject* obj) { timeElapsed;	obj; }
	void PlayChase(float timeElapsed, CCollisionObject* obj) { timeElapsed;	obj; }
	void PlayAttack(float timeElapsed, CCollisionObject* obj) { timeElapsed;	obj; }
	void PlayDie(float timeElapsed, CCollisionObject* obj) { timeElapsed;	obj; }
	void PlayRemove(float timeElapsed, CCollisionObject* obj) { timeElapsed;	obj; }

protected:	// ����
	shared_ptr<CWayFinder> m_pWayFinder;
};

