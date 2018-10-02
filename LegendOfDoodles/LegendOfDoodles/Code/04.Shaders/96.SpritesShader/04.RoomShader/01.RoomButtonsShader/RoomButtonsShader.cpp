#include "stdafx.h"
#include "RoomButtonsShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/95.Sprites/Sprite.h"
#include "07.Network/Network.h"

/// <summary>
/// 목적: 로딩 바 출력용 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-10-02
/// </summary>

#define ReadyButton m_RoomButtons[0]
#define ExitButton m_RoomButtons[1]

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CRoomButtonsShader::CRoomButtonsShader(shared_ptr<CCreateMgr> pCreateMgr) : CShader(pCreateMgr)
{
	m_hWnd = pCreateMgr->GetHwnd();
}

CRoomButtonsShader::~CRoomButtonsShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CRoomButtonsShader::ReleaseUploadBuffers()
{
	for (int i = 0; i < 2; ++i)
	{
		if (m_RoomButtons[i])m_RoomButtons[i]->ReleaseUploadBuffers();
	}

#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
#endif
}

void CRoomButtonsShader::UpdateShaderVariables(int opt)
{
	UNREFERENCED_PARAMETER(opt);
	static UINT elementBytes = ((sizeof(CB_SPRITE_INFO) + 255) & ~255);

	for (int i = 0; i < 2; ++i)
	{
		CB_SPRITE_INFO *pMappedObject = (CB_SPRITE_INFO *)(m_pMappedObjects + elementBytes * i);

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_RoomButtons[i]->GetWorldMatrix())));
		pMappedObject->m_percentage = (float)i;
	}
}

void CRoomButtonsShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera, 0);
	m_ppMaterials[0]->UpdateShaderVariables();

	for (int j = 0; j < 2; j++)
	{
		m_RoomButtons[j]->Render(pCamera);
	}
}

bool CRoomButtonsShader::OnProcessMouseInput(WPARAM pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);

	POINT cursorPos;
	::GetCursorPos(&cursorPos);
	ScreenToClient(m_hWnd, &cursorPos);

	cursorPos.y = FRAME_BUFFER_HEIGHT - cursorPos.y;

	if (ReadyButton->IsInRect(FRAME_BUFFER_WIDTH / 3.f, FRAME_BUFFER_HEIGHT / 6.f, cursorPos))
	{
		CS_Msg_Prepare_Data p;
		p.Character_id = (BYTE)m_pNetwork->m_myid;
		p.size = sizeof(p);
		p.type = CS_PLAYER_READY;
		m_pNetwork->SendPacket(&p);
	}
	else if (ExitButton->IsInRect(FRAME_BUFFER_WIDTH / 3.f, FRAME_BUFFER_HEIGHT / 6.f, cursorPos))
	{
		m_ReadyButtonClicked = m_ExitButtonClicked = true;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CRoomButtonsShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pInputElementDescs[0] = {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[1] = {
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_BLEND_DESC CRoomButtonsShader::CreateBlendState()
{
	D3D12_BLEND_DESC blendDesc;
	::ZeroMemory(&blendDesc, sizeof(D3D12_BLEND_DESC));

	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(blendDesc);
}

D3D12_SHADER_BYTECODE CRoomButtonsShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/SpriteShader.hlsl",
		"VSSprite",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CRoomButtonsShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/SpriteShader.hlsl",
		"PSButtons",
		"ps_5_1",
		pShaderBlob));
}

void CRoomButtonsShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CRoomButtonsShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void * pContext)
{
	UNREFERENCED_PARAMETER(pContext);

	UINT ncbElementBytes = ((sizeof(CB_SPRITE_INFO) + 255) & ~255);
	CreateShaderVariables(pCreateMgr, ncbElementBytes, 2);
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 2, 1, 0);
	CreateConstantBufferViews(pCreateMgr, 2, m_pConstBuffer.Get(), ncbElementBytes, 0, 0);

	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];

	m_ppMaterials[0] = Materials::CreateRoomButtonMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);

	CMesh* pTitleButtonMesh{ new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 3.f, FRAME_BUFFER_HEIGHT / 6.f, 0.f, 0.f, 0.f, 1.f) };

	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };

	CSprite *pStartButton{ new CSprite(pCreateMgr) };
	pStartButton->SetMesh(0, pTitleButtonMesh);
	pStartButton->SetPosition(FRAME_BUFFER_WIDTH * 0.5f, FRAME_BUFFER_HEIGHT * 0.3f, 8.f);
	pStartButton->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr);
	m_RoomButtons[0] = pStartButton;

	CSprite *pExitButton{ new CSprite(pCreateMgr) };
	pExitButton->SetMesh(0, pTitleButtonMesh);
	pExitButton->SetPosition(FRAME_BUFFER_WIDTH * 0.5f, FRAME_BUFFER_HEIGHT * 0.15f, 8.f);
	pExitButton->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + incrementSize);
	m_RoomButtons[1] = pExitButton;
}

void CRoomButtonsShader::ReleaseObjects()
{
	for (int i = 0; i < 2; ++i)
	{
		if (m_RoomButtons[i]) Safe_Delete(m_RoomButtons[i]);
	}
}
