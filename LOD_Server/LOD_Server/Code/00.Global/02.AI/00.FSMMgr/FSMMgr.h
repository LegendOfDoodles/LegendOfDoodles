#pragma once
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"

class CCollisionObject;

class CFSMMgr
{
public:	// ������, �Ҹ���
	CFSMMgr(CWayFinder* pWayFinder);
	virtual ~CFSMMgr();

public: // ���� �Լ�
	void Update(float timeElapsed, CCollisionObject* obj);

protected: // ���� �Լ�
	void PlayIdle(float timeElapsed, CCollisionObject* obj);
	void PlayWalk(float timeElapsed, CCollisionObject* obj);
	void PlayChase(float timeElapsed, CCollisionObject* obj);
	void PlayAttack(float timeElapsed, CCollisionObject* obj);
	void PlayDie(float timeElapsed, CCollisionObject* obj);
	void PlayRemove(float timeElapsed, CCollisionObject* obj);

protected:	// ����
	CWayFinder * m_pWayFinder;
};

