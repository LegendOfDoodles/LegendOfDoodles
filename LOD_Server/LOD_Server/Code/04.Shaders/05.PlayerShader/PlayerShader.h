#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CHeightMapTerrain;
class CCollisionManager;
class CThrowingMgr;
class CSkeleton;

class CPlayerShader : public CShader
{
public:
	CPlayerShader();
	~CPlayerShader();

public: // 공개 함수
	virtual void Initialize(void *pContext = NULL);

	virtual void AnimateObjects(float timeElapsed);

	virtual void SetPlayerAnimation(ObjectType type, int id);

	void SetColManagerToObject(shared_ptr<CCollisionManager> manager);
	void SetThrowingManagerToObject(shared_ptr<CThrowingMgr> manager);

protected: // 내부 함수

	virtual void BuildObjects(void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // 변수
	UINT m_nSword{ 0 };
	UINT m_nStaff{ 0 };
	UINT m_nBow{ 0 };
	UINT m_nArmor{ 0 };

	//애니메이션 정보
	CSkeleton** m_ppSwordAni{ NULL };
	CSkeleton** m_ppStaffAni{ NULL };
	CSkeleton** m_ppBowAni{ NULL };

	CHeightMapTerrain * m_pTerrain{ NULL };
};
