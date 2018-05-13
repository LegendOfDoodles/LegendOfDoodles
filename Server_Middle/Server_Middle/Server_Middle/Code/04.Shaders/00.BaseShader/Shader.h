#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"

class CShader
{
public:	// ������, �Ҹ���
	CShader();
	virtual ~CShader();

public: // ���� �Լ�
	virtual void Initialize(void *pContext = NULL);
	virtual void Finalize();

	virtual void AnimateObjects(float timeElapsed);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	virtual CBaseObject * * GetCollisionObjects() { return nullptr; }

protected: // ���� �Լ�
	virtual void BuildObjects(void *pContext = NULL);
	virtual void ReleaseObjects();

protected: // ����
	int m_nReferences{ 0 };
};

