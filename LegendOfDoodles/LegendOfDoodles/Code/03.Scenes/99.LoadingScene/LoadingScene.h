#pragma once
#include "03.Scenes/00.BaseScene/Scene.h"

class CLoadingScene : public CScene
{
public:	// 생성자, 소멸자
	CLoadingScene();
	virtual ~CLoadingScene();

public: // 공개 함수

protected: // 내부 함수
	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr);

protected: // 변수
};

