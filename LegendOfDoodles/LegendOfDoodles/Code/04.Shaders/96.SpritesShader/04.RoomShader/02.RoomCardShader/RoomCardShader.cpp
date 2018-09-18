#include "stdafx.h"
#include "RoomCardShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/95.Sprites/Sprite.h"

/// <summary>
/// 목적: 로딩 바 출력용 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-09-18
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CRoomCardShader::CRoomCardShader(shared_ptr<CCreateMgr> pCreateMgr) : CShader(pCreateMgr)
{
	m_hWnd = pCreateMgr->GetHwnd();

	// Warning! 서버에서 최초 정보 얻어와야 함
	m_EachCardType[0] = CardType::Blue_Player;
	m_EachCardType[1] = CardType::Blue_AI;
	m_EachCardType[2] = CardType::Red_AI;
	m_EachCardType[3] = CardType::Red_AI;
}

CRoomCardShader::~CRoomCardShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CRoomCardShader::ReleaseUploadBuffers()
{
	for (int i = 0; i < 4; ++i)
	{
		if (m_RoomCards[i])m_RoomCards[i]->ReleaseUploadBuffers();
	}

#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
#endif
}

void CRoomCardShader::UpdateShaderVariables(int opt)
{
	UNREFERENCED_PARAMETER(opt);
	static UINT elementBytes = ((sizeof(CB_SPRITE_INFO) + 255) & ~255);

	for (int i = 0; i < 4; ++i)
	{
		CB_SPRITE_INFO *pMappedObject = (CB_SPRITE_INFO *)(m_pMappedObjects + elementBytes * i);

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_RoomCards[i]->GetWorldMatrix())));
		pMappedObject->m_percentage = (float)m_EachCardType[i];
	}
}

void CRoomCardShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera, 0);
	m_ppMaterials[0]->UpdateShaderVariables();

	for (int j = 0; j < 4; j++)
	{
		m_RoomCards[j]->Render(pCamera);
	}
}

bool CRoomCardShader::OnProcessMouseInput(WPARAM pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);

	POINT cursorPos;
	::GetCursorPos(&cursorPos);
	ScreenToClient(m_hWnd, &cursorPos);

	cursorPos.y = FRAME_BUFFER_HEIGHT - cursorPos.y;

	for (int i = 0; i < 4; ++i)
	{
		if (m_EachCardType[i] == CardType::Blue_Player || m_EachCardType[i] == CardType::Red_Player) continue;

		if (m_RoomCards[i]->IsInRect(FRAME_BUFFER_WIDTH / 5.f, FRAME_BUFFER_WIDTH / 5.f, cursorPos))
		{
			if (m_EachCardType[i] == CardType::Blue_AI)
			{
				m_EachCardType[i] = CardType::Blue_Player;
			}
			else if (m_EachCardType[i] == CardType::Red_AI)
			{
				m_EachCardType[i] = CardType::Red_Player;
			}

			if (m_EachCardType[m_myId] == CardType::Blue_Player)
			{
				m_EachCardType[m_myId] = CardType::Blue_AI;
			}
			else if (m_EachCardType[m_myId] == CardType::Red_Player)
			{
				m_EachCardType[m_myId] = CardType::Red_AI;
			}
			m_myId = i;
			// Warning! 서버에 플레이어 위치 바꾸는 것 요청 필요
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CRoomCardShader::CreateInputLayout()
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

D3D12_BLEND_DESC CRoomCardShader::CreateBlendState()
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

D3D12_SHADER_BYTECODE CRoomCardShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/SpriteShader.hlsl",
		"VSSprite",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CRoomCardShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/SpriteShader.hlsl",
		"PSCards",
		"ps_5_1",
		pShaderBlob));
}

void CRoomCardShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CRoomCardShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void * pContext)
{
	UNREFERENCED_PARAMETER(pContext);

	UINT ncbElementBytes = ((sizeof(CB_SPRITE_INFO) + 255) & ~255);
	CreateShaderVariables(pCreateMgr, ncbElementBytes, 4);
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 4, 1, 0);
	CreateConstantBufferViews(pCreateMgr, 4, m_pConstBuffer.Get(), ncbElementBytes, 0, 0);

	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];

	m_ppMaterials[0] = Materials::CreateRoomCardsMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);

	CMesh* pTitleButtonMesh{ new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 5.f, FRAME_BUFFER_WIDTH / 5.f * 1.6f, 0.f, 0.f, 0.f, 1.f) };

	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };

	for (int i = 0; i < 4; ++i)
	{
		CSprite *pCard{ new CSprite(pCreateMgr) };
		pCard->SetMesh(0, pTitleButtonMesh);
		pCard->SetPosition(FRAME_BUFFER_WIDTH * (i + 1) / 5.f, FRAME_BUFFER_HEIGHT * 0.6f, 9.f);
		pCard->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + incrementSize * i);
		m_RoomCards[i] = pCard;
	}
}

void CRoomCardShader::ReleaseObjects()
{
	for (int i = 0; i < 4; ++i)
	{
		if (m_RoomCards[i]) Safe_Delete(m_RoomCards[i]);
	}
}
