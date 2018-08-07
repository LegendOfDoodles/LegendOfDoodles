#pragma once
#include "05.Objects/99.Material/00.Texture/Texture.h"
#include "04.Shaders/00.BaseShader/Shader.h"

class CCreateMgr;

struct COLOR
{
	XMFLOAT4							m_xmf4Diffuse{ 1.0f, 1.0f, 1.0f, 1.0f };
	float								m_cSpecularPower{ 1.0f };
	float								m_cRoughness{ 1.0f };
	float								m_cMetalic{ 0.0f };
};

class CMaterial
{
public:	// 생성자, 소멸자
	CMaterial(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CMaterial();

public: // 공개 함수
	void Initialize(shared_ptr<CCreateMgr> pCreateMgr);
	void Finalize();

	void ReleaseUploadBuffers();

	void UpdateShaderVariables();
	void UpdateShaderVariable(int nIndex);

	void Render(CCamera *pCamera);

	void SetTexture(CTexture *pTexture);
	void SetShader(CShader *pShader);

	void SetAlbedo(XMFLOAT4 xmf4Albedo) { m_color.m_xmf4Diffuse = xmf4Albedo; }
	void SetSpecularPower(float SP) { m_color.m_cSpecularPower = SP; }
	void SetRoughness(float R) { m_color.m_cRoughness = 1 - CLAMP(R, 0, 1); }
	void SetMetalic(float M) { m_color.m_cMetalic = CLAMP(M, 0, 1); }

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected: // 내부 함수
	void CreateShaderVariables(shared_ptr<CCreateMgr> pCreateMgr);
	void ReleaseShaderVariables();
	void UpdateShaderVariable();

protected: // 변수
	int	m_nReferences{ 0 };

	ComPtr<ID3D12Resource>			m_pcbColor;
	COLOR							*m_pcbMappedColor{ NULL };

	COLOR							m_color;

	CTexture						*m_pTexture{ NULL };

	CShader							*m_pShader{ NULL };

	ComPtr<ID3D12GraphicsCommandList> m_pCommandList;
};

