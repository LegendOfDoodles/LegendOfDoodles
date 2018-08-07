#pragma once
#include "05.Objects/02.CollisionObject/CollisionObject.h"

class CShader
{
public:	// ������, �Ҹ���
	CShader();
	virtual ~CShader();

public: // ���� �Լ�
	virtual void Initialize(void *pContext = NULL);
	virtual void Finalize();

	virtual void AnimateObjects(float timeElapsed);

	CCollisionObject **GetCollisionObjects() { return m_ppObjects; }
	int GetObjectCount() { return m_nObjects; }

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected: // ���� �Լ�
	virtual void BuildObjects(void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // ����
	int m_nReferences{ 0 };

	CCollisionObject **m_ppObjects{ NULL };
	int m_nObjects{ 0 };
};

