#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CScene
{
public:	// ������, �Ҹ���
	CScene();
	virtual ~CScene();

public: // ���� �Լ�
	virtual void Initialize();
	virtual void Finalize();

	virtual void AnimateObjects(float timeElapsed);

	virtual void GenerateLayEndWorldPosition(XMFLOAT3& pickPosition, int id) { pickPosition; id; }

	virtual CAnimatedObject** GetPlayerObject() { return NULL; }
	virtual CAnimatedObject** GetNeutralityObject() { return NULL; }
	virtual CCollisionObject** GetNexusTower() { return NULL; }

	CShader* GetShader(int num) { return m_ppShaders[num]; }

protected: // ���� �Լ�
	virtual void BuildObjects() {}
	virtual void ReleaseObjects();

protected: // ����
	CShader **m_ppShaders{ NULL };
	int m_nShaders{ 0 };
};

