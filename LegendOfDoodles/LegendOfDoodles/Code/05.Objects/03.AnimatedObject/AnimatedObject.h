#pragma once
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"

class CWayFinder;

struct CB_ANIOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World0;
	XMFLOAT4X4 m_xmf4x4Frame[128];
};

class CAnimatedObject : public CCollisionObject
{
public:	// 생성자, 소멸자
	CAnimatedObject(CCreateMgr *pCreateMgr, int nMeshes = 1);
	virtual ~CAnimatedObject();

public: // 공개 함수
	virtual void Animate(float timeElapsed);
	virtual void Render(CCamera *pCamera, UINT instanceCnt = 1);

	void SetSkeleton(CSkeleton *skeleton);

	virtual void SetPosition(float x, float z);

	void SetAnimation(AnimationsType newAnimation, float curFrame);
	void RegenerateWorldMatrixWithLook(XMFLOAT3 look);

	void SetTerrain(CHeightMapTerrain *pTerrain) { m_pTerrain = pTerrain; }

	int GetAnimTimeRemain() { return m_nAniLength[m_nCurrAnimation] - m_fFrameTime; }
	float GetAnimTimeRemainRatio() { return (m_nAniLength[m_nCurrAnimation] - m_fFrameTime) / (float)m_nAniLength[m_nCurrAnimation]; }

protected: // 내부 함수
	virtual void AdjustAnimationIndex() = 0;

protected: // 변수
	CSkeleton	m_pSkeleton[20];

	int m_nAniLength[20];
	int m_nAniIndex{ 0 };
	AnimationsType m_nCurrAnimation{ Animations::Idle };

	int m_nAniCnt{ 0 };

	float m_fPreFrameTime{ 0 };
	float m_fFrameTime{ 0 };

	float m_fAnimationSpeed{ 1 };

	CHeightMapTerrain * m_pTerrain{ NULL };
};

