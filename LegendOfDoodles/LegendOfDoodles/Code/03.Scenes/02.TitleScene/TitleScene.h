#pragma once
#include "03.Scenes/00.BaseScene/Scene.h"

class CTitleScene : public CScene
{
public:	// ������, �Ҹ���
	CTitleScene();
	virtual ~CTitleScene();

public: // ���� �Լ�
	virtual void AnimateObjects(float timeElapsed);
	
	virtual bool IsSceneDone();
	virtual bool IsExitScene();

protected: // ���� �Լ�
	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr);

protected: // ����
};

