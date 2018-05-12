#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"

class CArrow : public CBaseObject
{
public: // 持失切, 社瑚切
	CArrow(CCreateMgr *pCreateMgr, float length = 1000.0f);
	virtual ~CArrow();
};

