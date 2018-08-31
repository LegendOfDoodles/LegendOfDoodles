#pragma once
#include "05.Objects/02.CollisionObject/CollisionObject.h"

class CShader
{
public:	// 생성자, 소멸자
	CShader();
	virtual ~CShader();

public: // 공개 함수
	virtual void Initialize(void *pContext = NULL);
	virtual void Finalize();
	virtual void AnimateObjects(float timeElapsed);
	virtual CollisionObjectList* GetMinionList(ObjectType, TeamType) { return nullptr; }

	CCollisionObject **GetCollisionObjects() { return m_ppObjects; }
	int GetObjectCount() { return m_nObjects; }

	virtual void SetPlayerAnimation(ObjectType type, int id) { type; id; }

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }


protected: // 내부 함수
	virtual void BuildObjects(void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // 변수
	int m_nReferences{ 0 };

	CCollisionObject **m_ppObjects{ NULL };
	int m_nObjects{ 0 };
};

