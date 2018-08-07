#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"

class CSkyBox : public CBaseObject
{
public: // ������, �Ҹ���
	CSkyBox(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CSkyBox();

public: // ���� �Լ�
	virtual void Render(CCamera *pCamera, UINT istanceCnt = 1);
};

class CFloor : public CBaseObject
{
public: // ������, �Ҹ���
	CFloor(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CFloor();

public: // ���� �Լ�
};