#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CBillboardObject;
class CUIObject;
class CMaterial;

struct CB_TEXTURE_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
	int	m_index;
};

class CUIObjectShader : public CShader
{
public: // 생성자, 소멸자
	CUIObjectShader(CCreateMgr *pCreateMgr);
	virtual ~CUIObjectShader();

public: // 공개 함수
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	virtual void GetCamera(CCamera *pCamera);

	virtual void OnStatus(int ObjectType);
	virtual void OffStatus() { if (OnOFF) OnOFF = false; };

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState();
	
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr);
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers = 1);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseShaderVariables();
	virtual void ReleaseObjects();

protected: // 변수
	CBaseObject **m_ppObjects{ NULL };
	int m_nObjects = 0;

	CMaterial	**m_ppMaterials{ NULL };

	CCamera *m_pCamera;

	bool OnOFF = false;

#if USE_INSTANCING
	CB_GAMEOBJECT_INFO *m_pMappedObjects{ NULL };
	CMaterial						*m_pMaterial{ NULL };
#else
	UINT8 *m_pMappedObjects{ NULL };
#endif
};

