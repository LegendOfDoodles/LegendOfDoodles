#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"
#include "05.Objects/96.Arrow/Arrow.h"

class CArrowShader : public CShader
{
public:	// ������, �Ҹ���
	CArrowShader(CCreateMgr* pCreateMgr);
	virtual ~CArrowShader();

public: // ���� �Լ�
	virtual void ReleaseUploadBuffers();
	virtual void UpdateShaderVariables();

	virtual void Render(CCamera *pCamera);

protected: // ���� �Լ�
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr);
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers = 1);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // ����
	CArrow * m_pArrow{ NULL };
	CB_GAMEOBJECT_INFO *m_pMappedArrow{ NULL };
};

