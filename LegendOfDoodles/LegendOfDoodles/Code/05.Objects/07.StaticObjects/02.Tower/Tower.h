#pragma once
#include "05.Objects/02.CollisionObject/CollisionObject.h"

class CCreateMgr;

class CTower : public CCollisionObject
{
public: // 생성자, 소멸자
	CTower(CCreateMgr *pCreateMgr);
	virtual ~CTower();

public:	// 외부 함수

protected:	// 내부 함수

protected:	// 변수
};

