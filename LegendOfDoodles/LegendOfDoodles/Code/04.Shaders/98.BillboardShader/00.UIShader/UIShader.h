#pragma once
#include "04.Shaders/98.BillboardShader/99.BIllboard/BillboardShader.h"

class CBillboardObject;
class CUIFrameObject;
class CMaterial;

class CUIObjectShader : public CBillboardShader
{
public: // 생성자, 소멸자
	CUIObjectShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CUIObjectShader();

public: // 공개 함수
	virtual void UpdateShaderVariables(int opt = 0);

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);
protected: // 변수
	CCamera * m_pCamera;

	bool isRendering{ true };
};

