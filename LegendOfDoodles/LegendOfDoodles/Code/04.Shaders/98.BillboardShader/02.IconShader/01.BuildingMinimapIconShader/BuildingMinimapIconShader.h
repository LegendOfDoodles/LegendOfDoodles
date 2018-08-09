#pragma once
#include "04.Shaders/98.BillboardShader/99.BIllboard/BillboardShader.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "05.Objects/96.Billboard/99.BillboardObject/Billboard.h"

class CMaterial;

class CBuildingMinimapIconShader : public CBillboardShader
{
public:
	CBuildingMinimapIconShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CBuildingMinimapIconShader();

public: // 공개 함수
	virtual void UpdateShaderVariables(int opt = 0);

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	virtual void GetCamera(CCamera *pCamera);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);

	virtual void SetNexusAndTower(CCollisionObject **ppObjects) { m_ppNexusAndTower = ppObjects; }
	virtual void SetNexusAndTowerCnt(int nexusCnt, int towerCnt) { m_nNexus = nexusCnt; m_nTower = towerCnt; }

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // 변수
	// 카메라
	CCamera *m_pCamera;

	// Tower And Nexus
	CCollisionObject **m_ppNexusAndTower{ NULL };
	int m_nNexus{ 0 };
	int m_nTower{ 0 };
};