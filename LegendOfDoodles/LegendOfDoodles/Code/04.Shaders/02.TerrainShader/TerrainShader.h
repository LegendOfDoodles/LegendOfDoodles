#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"

class CTerrainShader : public CShader
{
public:	// 생성자, 소멸자
	CTerrainShader(CCreateMgr *pCreateMgr);
	virtual ~CTerrainShader();

public: // 공개 함수
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();

	virtual void Render(CCamera *pCamera);

	CHeightMapTerrain * GetTerrain() { return m_pTerrain; }

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateHullShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateDomainShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr, UINT nRenderTargets = 1);
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers = 1);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // 변수
	CHeightMapTerrain *m_pTerrain{ NULL };
	CB_GAMEOBJECT_INFO *m_pMappedTerrain{ NULL };
};