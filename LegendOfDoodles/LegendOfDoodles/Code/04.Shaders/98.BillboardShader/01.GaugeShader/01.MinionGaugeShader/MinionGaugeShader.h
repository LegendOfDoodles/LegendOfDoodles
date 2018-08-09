#pragma once
#include "04.Shaders/98.BillboardShader/99.BIllboard/BillboardShader.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "05.Objects/96.Billboard/02.GaugeObject/GaugeObject.h"

typedef std::list<CCollisionObject*> CollisionObjectList;
typedef std::list<CGaugeObject*> HPGaugeObjectList;

class CMaterial;
class CUIObjectManager;

class CMinionHPGaugeShader : public CBillboardShader
{
public: // 생성자, 소멸자
	CMinionHPGaugeShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CMinionHPGaugeShader();

public: // 공개 함수
	virtual void UpdateShaderVariables(int opt = 0);

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	virtual void SetCamera(CCamera *pCamera);

	void SetUIObjectsManager(shared_ptr<CUIObjectManager> pManger);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

	int GetPossibleIndex();
	void SpawnGauge();

	void SetPossibleIndex(int idx) { m_indexArr[idx] = true; }
	void ResetPossibleIndex(int idx) { m_indexArr[idx] = false; }

protected: // 변수
	shared_ptr<CCreateMgr> m_pCreateMgr;
	shared_ptr<CUIObjectManager> m_pGaugeManger;

	CCamera *m_pCamera;

	CollisionObjectList *m_MinionObjectList;
	HPGaugeObjectList m_HPGaugeObjectList;

	bool m_indexArr[MAX_MINION]{ false };
};

