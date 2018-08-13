#pragma once
#include "00.Global/01.Utility/04.WayFinder/01.Edge/Edge.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"

typedef std::list<CPathEdge> Path;
typedef std::list<CCollisionObject*> CollisionObjectList;

class CMaterial;
class CHeightMapTerrain;
class CCollisionManager;
class CThrowingMgr;
class CUIObjectManager;
class CFSMMgr;

class CMinionShader : public CShader
{
public: // 생성자, 소멸자
	CMinionShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CMinionShader();

public: // 공개 함수
	virtual void Initialize(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables(int opt = 0);
	virtual void UpdateBoundingBoxShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);
	virtual void RenderBoundingBox(CCamera *pCamera);
	virtual void RenderShadow(CCamera *pCamera);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);

	void SpawnMinion(ObjectType minionKind = ObjectType::SwordMinion, short tag = 0);

	void SetCollisionManager(shared_ptr<CCollisionManager> pManger) { m_pColManager = pManger; }
	void SetThrowingManager(shared_ptr<CThrowingMgr> pManger) { m_pThrowingMgr = pManger; }
	void SetGaugeManger(shared_ptr<CUIObjectManager> pManger) { m_pGaugeManger = pManger; }
	void SetFSMManager(shared_ptr<CFSMMgr> pManger) { m_pFSMMgr = pManger; }
	void SetEffectManager(shared_ptr<CEffectMgr> pManger) { m_pEffectMgr = pManger; }

	CollisionObjectList* GetMinionList(ObjectType Minion_Type, TeamType Team_Type);

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual D3D12_SHADER_BYTECODE CreateShadowVertexShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

	void CreatePathes();

	int GetPossibleIndex();

	void SetPossibleIndex(int idx) { m_indexArr[idx] = true; }
	void ResetPossibleIndex(int idx) { m_indexArr[idx] = false; }

protected: // 변수
	CollisionObjectList m_blueSwordMinions;
	CollisionObjectList m_blueStaffMinions;
	CollisionObjectList m_blueBowMinions;
	CollisionObjectList m_redSwordMinions;
	CollisionObjectList m_redStaffMinions;
	CollisionObjectList m_redBowMinions;

	bool m_indexArr[MAX_MINION];

	Path m_pathes[4];

	shared_ptr<CCollisionManager> m_pColManager;
	shared_ptr<CThrowingMgr> m_pThrowingMgr;
	shared_ptr<CUIObjectManager> m_pGaugeManger;
	CHeightMapTerrain * m_pTerrain{ NULL };

	shared_ptr<CCreateMgr> m_pCreateMgr{ NULL };
	shared_ptr<CFSMMgr> m_pFSMMgr;
	shared_ptr<CEffectMgr> m_pEffectMgr;

};
