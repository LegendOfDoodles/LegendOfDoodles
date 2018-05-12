#pragma once
#include "05.Objects/02.CollisionObject/CollisionObject.h"

class CCreateMgr;

class CNexus : public CCollisionObject
{
public: // 생성자, 소멸자
	CNexus(CCreateMgr *pCreateMgr);
	virtual ~CNexus();

public:	// 외부 함수

protected:	// 내부 함수

protected:	// 변수
};

