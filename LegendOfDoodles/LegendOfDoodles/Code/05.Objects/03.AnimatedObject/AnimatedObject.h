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
	XMFLOAT4X4 m_xmf4x4Frame[31];
};

class CAnimatedObject : public CCollisionObject
{
public:	// 생성자, 소멸자
	CAnimatedObject(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes = 1);
	virtual ~CAnimatedObject();

public: // 공개 함수
	virtual void Animate(float timeElapsed = 0.0f);
	virtual void Render(CCamera *pCamera, UINT instanceCnt = 1);

	void SetSkeleton(CSkeleton *skeleton);

	virtual void MoveUp(float fDistance = 1.0f);
	virtual void MoveForward(float fDistance = 1.0f);

	virtual void LookAt(XMFLOAT3 objPosition);
	virtual void LookAt(XMFLOAT2 objPosition);

	virtual XMFLOAT3 GetLook();
	virtual XMFLOAT3 GetUp();

	virtual void SetPosition(float x, float z);

	void SetPathToGo(Path *path);
	ProcessType MoveToDestination(float timeElapsed, shared_ptr<CWayFinder> pWayFinder = NULL);
	void MoveToSubDestination(float timeElapsed, shared_ptr<CWayFinder> pWayFinder = NULL);

	void GenerateSubPathToMainPath(shared_ptr<CWayFinder> pWayFinder);
	void GenerateSubPathToPosition(shared_ptr<CWayFinder> pWayFinder, XMFLOAT3 position);
	virtual void RegenerateLookAt();

	virtual bool Attackable(CCollisionObject* other);
	virtual bool AttackableFarRange(CCollisionObject* other);
	virtual bool Chaseable(CCollisionObject* other);

	void SyncAnimation(AnimationsType syncAnimation, int animtimeremain) { m_nCurrAnimation = syncAnimation; m_fFrameTime = static_cast<float>(animtimeremain); }
	virtual void SetAnimation(AnimationsType newAnimation) { m_nNextAnimation = m_nCurrAnimation = newAnimation; }
	virtual void SetNextAnimation(AnimationsType newAnimation) { m_nNextAnimation = newAnimation; }

	void SetTerrain(CHeightMapTerrain *pTerrain) { m_pTerrain = pTerrain; }

	void  SetSpeed(float speed) { m_speed = speed; }

	int GetAnimTimeRemain() { return static_cast<int>(m_nAniLength[m_nAniIndex] - m_fFrameTime); }
	float GetAnimTimeRemainRatio() { return (m_nAniLength[m_nAniIndex] - m_fFrameTime) / (float)m_nAniLength[m_nAniIndex]; }

protected: // 내부 함수
	bool IsArrive(float dst, PathType type = PathType::Main);

	virtual void AdjustAnimationIndex() = 0;

	bool Walkable();
	bool NoneDestination(PathType type = PathType::Main);
	void ResetDestination(PathType type = PathType::Main);
	void ResetSubPath();

	// 길 맞게 가고 있는지 확인 제대로 안가면 다시 방향 잡도록 한다.
	void CheckRightWay(PathType type, shared_ptr<CWayFinder> pWayFinder);

protected: // 변수
	CSkeleton	m_pSkeleton[20];

	int m_nAniLength[20];
	int m_nAniIndex{ 0 };
	AnimationsType m_nCurrAnimation{ Animations::Idle };
	AnimationsType m_nNextAnimation{ Animations::Idle };

	int m_nAniCnt{ 0 };

	float m_fPreFrameTime{ 0 };
	float m_fFrameTime{ 0 };

	float m_fAnimationSpeed{ 1 };

	float m_speed{ 0.0f };

	CHeightMapTerrain * m_pTerrain{ NULL };

	XMFLOAT2 m_destination{ NONE, NONE };
	XMFLOAT2 m_subDestination{ NONE, NONE };
	Path *m_mainPath{ NULL };
	Path *m_subPath{ NULL };

	float m_availableTime{ 0.0f };	// subPath가 유효한 시간 체크
};

