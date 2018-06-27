#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"
#include "05.Objects/05.SkyBox/SkyBox.h"

class CSkyBoxShader : public CShader
{
public: // 생성자, 소멸자
	CSkyBoxShader(CCreateMgr *pCreateMgr);
	virtual ~CSkyBoxShader();

public: // 공개 함수
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();

	virtual void Render(CCamera *pCamera);

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false);
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers = 1);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // 변수
	CSkyBox *m_pSkyBox{ NULL };
	CFloor *m_pFloor{ NULL };
	CB_GAMEOBJECT_INFO *m_pMappedSkyBox{ NULL };
};

