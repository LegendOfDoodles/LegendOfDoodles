#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"

class CSkyBox : public CBaseObject
{
public: // 생성자, 소멸자
	CSkyBox(CCreateMgr *pCreateMgr);
	virtual ~CSkyBox();

public: // 공개 함수
	virtual void Render(CCamera *pCamera, UINT istanceCnt = 1);
};

class CFloor : public CBaseObject
{
public: // 생성자, 소멸자
	CFloor(CCreateMgr *pCreateMgr);
	virtual ~CFloor();

public: // 공개 함수
};