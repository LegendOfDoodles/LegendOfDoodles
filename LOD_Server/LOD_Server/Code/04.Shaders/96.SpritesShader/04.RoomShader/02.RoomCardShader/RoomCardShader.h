#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CRoomCardShader : public CShader
{
public:	// 생성자, 소멸자
	CRoomCardShader();
	virtual ~CRoomCardShader();

public: // 공개 함수
	virtual void SetPlayerByIndex(int index);

	virtual CardType* GetCardType() { return m_EachCardType; }

protected: // 내부 함수

protected: // 변수
	CardType m_EachCardType[4];
};

