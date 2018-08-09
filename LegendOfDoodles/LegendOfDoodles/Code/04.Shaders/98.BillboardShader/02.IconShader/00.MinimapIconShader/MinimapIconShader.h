#pragma once
#include "04.Shaders/98.BillboardShader/99.BIllboard/BillboardShader.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "05.Objects/96.Billboard/03.IconObject/IconObject.h"

typedef std::list<CCollisionObject*> CollisionObjectList;
typedef std::list<CIconObject*> MinionIconObjectList;

class CMaterial;
class CPlayer;
class CIconObject;
class CUIObjectManager;

class CMinimapIconShader : public CBillboardShader
{
public:
	CMinimapIconShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CMinimapIconShader();

public: // 공개 함수
	virtual void UpdateShaderVariables(int opt = 0);

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	virtual void SetCamera(CCamera *pCamera);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);
	
	void SetUIObjectsManager(shared_ptr<CUIObjectManager> pManger);

	virtual void SetPlayer(CCollisionObject **ppPlayer) { m_ppPlayer = ppPlayer; };
	virtual void SetPlayerCnt(int cnt) { m_nPlayer = cnt; };

	virtual void SetRoider(CCollisionObject **ppRoider) { m_ppRoider = ppRoider; };
	virtual void SetRoiderCnt(int cnt) { m_nRoider = cnt; };

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

	int GetPossibleIndex();
	void SpawnMinionIcon();

	void SetPossibleIndex(int idx) { m_indexArr[idx] = true; }
	void ResetPossibleIndex(int idx) { m_indexArr[idx] = false; }

protected: // 변수
	// 카메라
	CCamera *m_pCamera;

	// Players
	CCollisionObject **m_ppPlayer;
	int m_nPlayer = 0;

	CCollisionObject **m_ppRoider;
	int m_nRoider = 0;

	//동적생성
	bool m_indexArr[MAX_MINION]{ false };

	CollisionObjectList *m_MinionObjectList;
	MinionIconObjectList m_MinionIconObjectList;

	shared_ptr<CUIObjectManager> m_pIconManger;
	shared_ptr<CCreateMgr> m_pCreateMgr;
};