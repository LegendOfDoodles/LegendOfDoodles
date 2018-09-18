#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CRoomCardShader : public CShader
{
public:	// ������, �Ҹ���
	CRoomCardShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CRoomCardShader();

public: // ���� �Լ�
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables(int opt = 0);
	virtual void Render(CCamera *pCamera);

	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);

	virtual int GetPlayerId() { return m_myId; }
	virtual CardType* GetCardType() { return m_EachCardType; }

protected: // ���� �Լ�
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);
	virtual void ReleaseObjects();

protected: // ����
	HWND m_hWnd{ NULL };

	int m_myId{ 0 };

	CBaseObject * m_RoomCards[4];
	CardType m_EachCardType[4];
};

