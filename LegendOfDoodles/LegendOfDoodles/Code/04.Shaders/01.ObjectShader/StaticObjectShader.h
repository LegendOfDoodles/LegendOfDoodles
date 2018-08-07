#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CMaterial;
class CHeightMapTerrain;

class CStaticObjectShader : public CShader
{
public: // ������, �Ҹ���
	CStaticObjectShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CStaticObjectShader();

public: // ���� �Լ�
	virtual void Initialize(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	virtual void UpdateShaderVariables(int opt = 0);
	virtual void UpdateBoundingBoxShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);
	virtual void RenderBoundingBox(CCamera *pCamera);
	virtual void RenderShadow(CCamera *pCamera);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);

protected: // ���� �Լ�
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual D3D12_SHADER_BYTECODE CreateShadowVertexShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	void SetBoundingBoxMeshByIndex(shared_ptr<CCreateMgr> pCreateMgr, CBaseObject* target, int index);

protected: // ����
	int m_meshCounts[17];
	CBaseObject **m_ppObjects{ NULL };

	CHeightMapTerrain * m_pTerrain{ NULL };
};