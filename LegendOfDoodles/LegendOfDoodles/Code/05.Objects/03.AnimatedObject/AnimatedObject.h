#pragma once
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"
#include "00.Global/01.Utility/04.WayFinder/01.Edge/Edge.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"

typedef std::list<CPathEdge> Path;

class CWayFinder;

struct CB_ANIOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World0;
	XMFLOAT4X4 m_xmf4x4Frame[128];
};

class CAnimatedObject : public CCollisionObject
{
public:	// ������, �Ҹ���
	CAnimatedObject(CCreateMgr *pCreateMgr, int nMeshes = 1);
	virtual ~CAnimatedObject();

public: // ���� �Լ�
	virtual void Animate(float timeElapsed);
	virtual void Render(CCamera *pCamera, UINT instanceCnt = 1);

	void SetSkeleton(CSkeleton *skeleton);

	virtual void MoveUp(float fDistance = 1.0f);
	virtual void MoveForward(float fDistance = 1.0f);

	void LookAt(XMFLOAT3 objPosition);
	void LookAt(XMFLOAT2 objPosition);

	virtual XMFLOAT3 GetLook();
	virtual XMFLOAT3 GetUp();

	virtual void SetPosition(float x, float z);

	void SetPathToGo(Path *path);
	ProcessType MoveToDestination(float timeElapsed);
	void MoveToEnemy(float timeElapsed, CWayFinder* pWayFinder);

	virtual void RegenerateLookAt();

	virtual bool Attackable(CCollisionObject* other);
	virtual bool Chaseable(CCollisionObject* other);

	void SetAnimation(AnimationsType newAnimation) { m_nCurrAnimation = newAnimation; }

	void SetTerrain(CHeightMapTerrain *pTerrain) { m_pTerrain = pTerrain; }

	void  SetSpeed(float speed) { m_speed = speed; }

	int GetAnimTimeRemain() { return m_nAniLength[m_nCurrAnimation] - m_fFrameTime; }
	float GetAnimTimeRemainRatio() { return (m_nAniLength[m_nCurrAnimation] - m_fFrameTime) / (float)m_nAniLength[m_nCurrAnimation]; }

protected: // ���� �Լ�
	bool IsArrive(float dst);

	virtual void AdjustAnimationIndex() = 0;

	bool Walkable();

	void ResetDestination() { m_destination.x = NONE; }
	bool NoneDestination() { return m_destination.x == NONE;	}

protected: // ����
	CSkeleton	m_pSkeleton[20];

	int m_nAniLength[20];
	int m_nAniIndex{ 0 };
	AnimationsType m_nCurrAnimation{ Animations::Idle };

	int m_nAniCnt{ 0 };

	float m_fPreFrameTime{ 0 };
	float m_fFrameTime{ 0 };

	float m_fAnimationSpeed{ 1 };

	float m_speed{ 0.0f };

	CHeightMapTerrain * m_pTerrain{ NULL };

	XMFLOAT2 m_destination{ NONE, NONE };
	Path *m_pathToGo{ NULL };
};

