#pragma once
#include "04.Shaders/98.BillboardShader/99.BIllboard/BillboardShader.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"

class CMaterial;
class CPlayer;

class CPlayerHPGaugeShader : public CBillboardShader
{
public:
	CPlayerHPGaugeShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CPlayerHPGaugeShader();

public: // 공개 함수
	virtual void UpdateShaderVariables(int opt = 0);

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	virtual void SetCamera(CCamera *pCamera);

	virtual void SetPlayer(CCollisionObject **pPlayer) { m_pPlayer = (CPlayer**)pPlayer; };
	virtual void SetPlayerCnt(int cnt) { m_nPlayer = cnt; };

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // 변수
	CCamera * m_pCamera;

	CPlayer **m_pPlayer;
	int m_nPlayer;

	CCollisionObject **m_ppNexusAndTower{ NULL };
	int m_nNexusAndTower = 0;

	shared_ptr<CCreateMgr> m_pCreateMgr{ NULL };
};