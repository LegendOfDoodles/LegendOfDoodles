#pragma once
#include "00.Global/01.Utility/04.WayFinder/01.Edge/Edge.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"

typedef std::list<CPathEdge> Path;

class CMaterial;
class CHeightMapTerrain;
class CCollisionManager;
class CThrowingMgr;
class CUIObjectManager;
class CFSMMgr;

struct NeutralObjectIndices
{
	int m_begIndex;
	int m_endIndex;

	NeutralObjectIndices() {}
	NeutralObjectIndices(int a, int b):m_begIndex(a), m_endIndex(b){}
};

class CNeutralityShader : public CShader
{
public: // 생성자, 소멸자
	CNeutralityShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CNeutralityShader();

public: // 공개 함수
	virtual void Initialize(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	virtual void UpdateShaderVariables(int opt = 0);
	virtual void UpdateBoundingBoxShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);
	virtual void RenderBoundingBox(CCamera *pCamera);
	virtual void RenderShadow(CCamera *pCamera);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);

	void SetColManagerToObject(shared_ptr<CCollisionManager> manager);
	void SetThrowingManagerToObject(shared_ptr<CThrowingMgr> manager);
	void SetEffectManagerToObject(shared_ptr<CEffectMgr> manager);
	void SetSoundManagerToObject(shared_ptr<CSoundManager> manager);

	void SetCollisionManager(shared_ptr<CCollisionManager> pManger) { m_pColManager = pManger; }
	void SetGaugeManger(shared_ptr<CUIObjectManager> pManger) { m_pGaugeManger = pManger; }
	void SetFSMManager(shared_ptr<CFSMMgr> pManger) { m_pFSMMgr = pManger; }

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual D3D12_SHADER_BYTECODE CreateShadowVertexShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	void CreatePathes();

protected: // 변수
	shared_ptr<CCollisionManager> m_pColManager;
	shared_ptr<CUIObjectManager> m_pGaugeManger;
	CHeightMapTerrain * m_pTerrain{ NULL };

	shared_ptr<CCreateMgr> m_pCreateMgr{ NULL };
	shared_ptr<CFSMMgr> m_pFSMMgr;

	std::unordered_map<ObjectType, NeutralObjectIndices> m_objectsIndices;

	Path m_pathes[6];
};
