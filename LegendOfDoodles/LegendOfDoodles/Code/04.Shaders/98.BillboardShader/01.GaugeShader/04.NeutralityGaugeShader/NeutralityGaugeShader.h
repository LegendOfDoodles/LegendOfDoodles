#pragma once
#include "04.Shaders/98.BillboardShader/99.BIllboard/BillboardShader.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"

class CMaterial;

class CNeutralityGaugeShader : public CBillboardShader
{
public:
	CNeutralityGaugeShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CNeutralityGaugeShader();

public: // 공개 함수
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables(int opt = 0);

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);

	virtual void SetRoider(CCollisionObject **ppRoider) { m_ppRoider = ppRoider; };
	virtual void SetRoiderCnt(int cnt) { m_nRoider = cnt; };

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // 변수
	CCamera * m_pCamera;

	CCollisionObject **m_ppRoider;
	int m_nRoider = 0;

	shared_ptr<CCreateMgr> m_pCreateMgr{ NULL };
};