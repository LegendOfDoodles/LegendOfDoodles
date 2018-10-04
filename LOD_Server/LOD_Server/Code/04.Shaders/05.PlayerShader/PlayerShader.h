#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"
#include "00.Global/01.Utility/04.WayFinder/01.Edge/Edge.h"

typedef std::list<CPathEdge> Path;

class CHeightMapTerrain;
class CCollisionManager;
class CThrowingMgr;
class CSkeleton;

class CPlayerShader : public CShader
{
public:
	CPlayerShader();
	~CPlayerShader();

public: // ���� �Լ�
	virtual void Initialize(void *pContext = NULL);

	virtual void AnimateObjects(float timeElapsed);

	virtual void SetPlayerAnimation(ObjectType type, int id);

	void SetColManagerToObject(shared_ptr<CCollisionManager> manager);
	void SetThrowingManagerToObject(shared_ptr<CThrowingMgr> manager);
	virtual void SetWayFinderToObject(shared_ptr<CWayFinder> pWayFinder);

protected: // ���� �Լ�
	virtual void BuildObjects(void *pContext = NULL);
	virtual void ReleaseObjects();

	void CreatePathes();

protected: // ����
	UINT m_nSword{ 0 };
	UINT m_nStaff{ 0 };
	UINT m_nBow{ 0 };
	UINT m_nArmor{ 0 };

	//�ִϸ��̼� ����
	CSkeleton** m_ppSwordAni{ NULL };
	CSkeleton** m_ppStaffAni{ NULL };
	CSkeleton** m_ppBowAni{ NULL };

	CHeightMapTerrain * m_pTerrain{ NULL };

	Path m_pathes[4];
};
