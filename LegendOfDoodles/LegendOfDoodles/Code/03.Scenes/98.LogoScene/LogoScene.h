#pragma once
#include "03.Scenes/00.BaseScene/Scene.h"

class CLogoScene : public CScene
{
public:	// ������, �Ҹ���
	CLogoScene();
	virtual ~CLogoScene();

public: // ���� �Լ�
	virtual bool IsSceneDone();

protected: // ���� �Լ�
	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr);

protected: // ����
};

