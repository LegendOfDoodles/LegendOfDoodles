#pragma once
#include "03.Scenes/00.BaseScene/Scene.h"

class CLoadingScene : public CScene
{
public:	// ������, �Ҹ���
	CLoadingScene();
	virtual ~CLoadingScene();

public: // ���� �Լ�

protected: // ���� �Լ�
	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr);

protected: // ����
};

