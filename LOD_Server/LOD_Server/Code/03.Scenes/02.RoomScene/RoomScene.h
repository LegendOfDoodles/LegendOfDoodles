#pragma once
#include "03.Scenes/00.BaseScene/Scene.h"

class CRoomScene : public CScene
{
public:	// 생성자, 소멸자
	CRoomScene();
	virtual ~CRoomScene();

public: // 공개 함수
	virtual void ApplyPlayerIndex(int index);

protected: // 내부 함수
	virtual void BuildObjects();

protected: // 변수
};

