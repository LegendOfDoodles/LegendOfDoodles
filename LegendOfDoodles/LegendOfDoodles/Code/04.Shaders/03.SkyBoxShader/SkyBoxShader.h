#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"
#include "05.Objects/05.SkyBox/SkyBox.h"

class CSkyBoxShader : public CShader
{
public: // ������, �Ҹ���
	CSkyBoxShader(CCreateMgr *pCreateMgr);
	virtual ~CSkyBoxShader();

public: // ���� �Լ�
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();

	virtual void Render(CCamera *pCamera);

protected: // ���� �Լ�
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false);
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers = 1);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // ����
	CSkyBox *m_pSkyBox{ NULL };
	CFloor *m_pFloor{ NULL };
	CB_GAMEOBJECT_INFO *m_pMappedSkyBox{ NULL };
};

