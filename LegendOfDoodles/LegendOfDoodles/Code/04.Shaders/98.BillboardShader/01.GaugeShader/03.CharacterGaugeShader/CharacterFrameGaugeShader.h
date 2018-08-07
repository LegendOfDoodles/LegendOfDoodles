#pragma once
#include "04.Shaders/98.BillboardShader/99.BIllboard/BillboardShader.h"

class CMaterial;
class CCollisionObject;

class CharacterFrameGaugeShader : public CBillboardShader
{
public:
	CharacterFrameGaugeShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CharacterFrameGaugeShader();

public: // 공개 함수
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables(int opt = 0);

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	virtual void GetCamera(CCamera *pCamera);
	virtual void SetPlayer(CBaseObject *pPlayer) { m_pPlayer = (CCollisionObject*)pPlayer; };

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // 변수
	CCamera *m_pCamera;

	CCollisionObject *m_pPlayer;
};