#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CRoomCardShader : public CShader
{
public:	// ������, �Ҹ���
	CRoomCardShader();
	virtual ~CRoomCardShader();

public: // ���� �Լ�
	virtual void SetPlayerByIndex(int index);

	virtual CardType* GetCardType() { return m_EachCardType; }

protected: // ���� �Լ�

protected: // ����
	CardType m_EachCardType[4];
};

