#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

typedef std::list<CCollisionObject*> CollisionObjectList;

class CMaterial;
class CCollisionManager;

struct FlyingObjectIndices
{
	int m_begIndex;
	int m_endIndex;
};

class CFlyingShader : public CShader
{
public: // 생성자, 소멸자
	CFlyingShader();
	virtual ~CFlyingShader();

public: // 공개 함수

	virtual void AnimateObjects(float timeElapsed);

	void SpawnFlyingObject(const XMFLOAT3& position, const XMFLOAT3& direction, TeamType teamType, FlyingObjectType objectType);

	void SetColManagerToObject(shared_ptr<CCollisionManager> manager);

	void Pause() { m_Paused = true; }
	void Resume() { m_Paused = false; };

protected: // 내부 함수

	virtual void BuildObjects(void *pContext = NULL);

	virtual void ReleaseObjects();

	int GetPossibleIndex(FlyingObjectType type);
	void ResetPossibleIndex(int idx) { m_objectsPossibleIndices[idx] = false; }

protected: // 변수
	static const int m_nMesh{ 3 };

	std::unordered_map<FlyingObjectType, UINT> m_objectsMaxCount;
	std::unordered_map<FlyingObjectType, FlyingObjectIndices> m_objectsIndices;
	std::unique_ptr<bool[]> m_objectsPossibleIndices;

	CollisionObjectList m_dumbelList;
	CollisionObjectList m_arrowList;
	CollisionObjectList m_magicList;

	bool m_Paused{ false };
};