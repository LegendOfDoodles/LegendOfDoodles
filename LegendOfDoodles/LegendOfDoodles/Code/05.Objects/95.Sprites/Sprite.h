#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"

class CSprite : public CBaseObject
{
public:	// ������, �Ҹ���
	CSprite(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CSprite();

public: // �����Լ�
	virtual void Render(CCamera *pCamera, UINT istanceCnt = 1);

protected: // ����
};