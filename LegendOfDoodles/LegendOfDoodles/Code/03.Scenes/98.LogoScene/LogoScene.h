#pragma once
#include "03.Scenes/00.BaseScene/Scene.h"

class CLogoScene : public CScene
{
public:	// 생성자, 소멸자
	CLogoScene();
	virtual ~CLogoScene();

public: // 공개 함수
	virtual bool IsSceneDone();

protected: // 내부 함수
	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr);

protected: // 변수
};

