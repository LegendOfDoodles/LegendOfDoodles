#pragma once
#include "04.Shaders/98.BillboardShader/99.BIllboard/BillboardShader.h"

class CBillboardObject;
class CMaterial;
class CPlayer;
class CPlayerShader;

class CSpecialSelectShader : public CBillboardShader
{
public: // ������, �Ҹ���
	CSpecialSelectShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CSpecialSelectShader();

public: // ���� �Լ�
	virtual void UpdateShaderVariables(int opt = 0);

	virtual void AnimateObjects(float timeElapsed);
	virtual void Render(CCamera *pCamera);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);

	virtual void SetPlayer(CBaseObject *pPlayer) { m_pPlayer = (CPlayer*)pPlayer; };
	void SetShader(CPlayerShader* shader) { m_pPlayerShader = shader; }

protected: // ���� �Լ�
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

protected: // ����
	CCamera * m_pCamera;

	CPlayer *m_pPlayer;

	CPlayerShader *m_pPlayerShader;

	bool isRendering = false;
	bool ShowWindow[2]{ false };
};

