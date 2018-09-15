#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"

struct CB_SPRITE_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
	float m_percentage;
};

class CSprite : public CBaseObject
{
public:	// ������, �Ҹ���
	CSprite(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CSprite();

public: // �����Լ�
	virtual void Render(CCamera *pCamera, UINT istanceCnt = 1);

protected: // ����
};