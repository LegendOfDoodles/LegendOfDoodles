#pragma once
#include "03.Scenes/00.BaseScene/Scene.h"

class CTitleScene : public CScene
{
public:	// 생성자, 소멸자
	CTitleScene();
	virtual ~CTitleScene();

public: // 공개 함수
	virtual void AnimateObjects(float timeElapsed);
	
	virtual bool IsSceneDone();
	virtual bool IsExitScene();

protected: // 내부 함수
	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr);

protected: // 변수
};

