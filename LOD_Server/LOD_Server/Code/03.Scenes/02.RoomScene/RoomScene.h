#pragma once
#include "03.Scenes/00.BaseScene/Scene.h"

class CRoomScene : public CScene
{
public:	// ������, �Ҹ���
	CRoomScene();
	virtual ~CRoomScene();

public: // ���� �Լ�
	virtual void ApplyPlayerIndex(int index);

protected: // ���� �Լ�
	virtual void BuildObjects();

protected: // ����
};

