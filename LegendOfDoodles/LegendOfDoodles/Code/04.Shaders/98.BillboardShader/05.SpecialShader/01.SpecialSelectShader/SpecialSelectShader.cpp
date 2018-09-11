#include "stdafx.h"
#include "SpecialSelectShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"
#include "05.Objects/96.Billboard/01.FrameObject/UIFrameObject.h"
#include "05.Objects/08.Player/Player.h"
#include "04.Shaders/05.PlayerShader/PlayerShader.h"
#include "07.Network/Network.h"

/// <summary>
/// ����: ���õ� Ư��
/// ���� ������:  �質��
/// ������ ���:  �̿뼱, �質��
/// ���� ���� ��¥: 2018-08-22
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CSpecialSelectShader::CSpecialSelectShader(shared_ptr<CCreateMgr> pCreateMgr)
	: CBillboardShader(pCreateMgr)
{
}

CSpecialSelectShader::~CSpecialSelectShader()
{
}

////////////////////////////////////////////////////////////////////////
//
void CSpecialSelectShader::UpdateShaderVariables(int opt)
{
	UNREFERENCED_PARAMETER(opt);
	static UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CSpecialSelectShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CSpecialSelectShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera, 0);

	for (int j = 0; j < m_nObjects; j++)
	{
		UIFrameType type = (UIFrameType)((CUIFrameObject*)m_ppObjects[j])->GetType();

		if (type == SpecialFrame)
		{
			if (ShowWindow[0] == true) {
				isRendering = true;
				m_ppMaterials[0]->UpdateShaderVariable(0);
			}
			else if (ShowWindow[1] == true) {
				isRendering = true;
				m_ppMaterials[0]->UpdateShaderVariable(1);
			}
		}

		if (isRendering) m_ppObjects[j]->Render(pCamera);
	}
}

bool CSpecialSelectShader::OnProcessKeyInput(UCHAR * pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);

	return true;
}

bool CSpecialSelectShader::OnProcessMouseInput(WPARAM pKeyBuffer)
{
	POINT cursorPos;

	GetCursorPos(&cursorPos);
	ScreenToClient(m_pCamera->GetHwnd(), &cursorPos);


	if (pKeyBuffer == MK_LBUTTON)
	{
		// ĳ����â Ŭ��
		if ((cursorPos.x > SPECIAL_MINIMUM_X  && cursorPos.x < SPECIAL_MAXIMUM_X)
			&& (cursorPos.y > SPECIAL_MINIMUM_Y && cursorPos.y < SPECIAL_MAXIMUM_Y))
		{
			// Warning! ���� ���� ���� 2�� ���� �ʿ�
			if (m_pPlayer->GetPlayerStatus()->Weapon == 0
				&& m_pPlayer->GetPlayerStatus()->Level >= 0)
			{
				if (ShowWindow[0] == true)
				{
					ShowWindow[0] = false;
					isRendering = false;
				}
				else ShowWindow[0] = true;
			}
			else if (m_pPlayer->GetPlayerStatus()->SpecialPoint >= 1
				&& m_pPlayer->GetPlayerStatus()->Weapon != 0)
			{
				if (ShowWindow[1] == true)
				{
					ShowWindow[1] = false;
					isRendering = false;
				}
				else ShowWindow[1] = true;
			}

			return false;
		}

		if (ShowWindow[0] == true && m_pPlayer->GetPlayerStatus()->Weapon == 0) {
			// Sword
			if ((cursorPos.x > SELECT_SPECIAL_MINIMUM_X  && cursorPos.x < SELECT_SPECIAL_MAXIMUM_X)
				&& (cursorPos.y > SELECT_ATTACK_SPECIAL_MINIMUM_Y && cursorPos.y < SELECT_ATTACK_SPECIAL_MAXIMUM_Y))
			{
				// ���� ����
				m_pPlayer->GetPlayerStatus()->Weapon = 1;
				m_pPlayer->SetWeaponChangeTriger(true);
				m_pPlayerShader->SetChangeWeapon(m_pNetwork->m_myid);

				// ���� ����â �ݱ�
				ShowWindow[0] = false;
				isRendering = false;

				return false;
			}

			// Bow
			if ((cursorPos.x > SELECT_SPECIAL_MINIMUM_X  && cursorPos.x < SELECT_SPECIAL_MAXIMUM_X)
				&& (cursorPos.y > SELECT_DEFENCE_SPECIAL_MINIMUM_Y && cursorPos.y < SELECT_DEFENCE_SPECIAL_MAXIMUM_Y))
			{
				// ���� ����
				m_pPlayer->GetPlayerStatus()->Weapon = 3;
				m_pPlayer->SetWeaponChangeTriger(true);
				m_pPlayerShader->SetChangeWeapon(m_pNetwork->m_myid);

				// ���� ����â �ݱ�
				ShowWindow[0] = false;
				isRendering = false;

				return false;
			}

			// Magic
			if ((cursorPos.x > SELECT_SPECIAL_MINIMUM_X  && cursorPos.x < SELECT_SPECIAL_MAXIMUM_X)
				&& (cursorPos.y > SELECT_TECHNIC_SPECIAL_MINIMUM_Y && cursorPos.y < SELECT_TECHNIC_SPECIAL_MAXIMUM_Y))
			{
				// ���� ����
				m_pPlayer->GetPlayerStatus()->Weapon = 2;
				m_pPlayer->SetWeaponChangeTriger(true);
				m_pPlayerShader->SetChangeWeapon(m_pNetwork->m_myid);

				// ���� ����â �ݱ�
				ShowWindow[0] = false;
				isRendering = false;

				return false;
			}

		}
		// Ư��â�� �����ְ� (LButton == true) �÷��̾�(���� ��������)�� Ư�� ����Ʈ�� ���� ���
		// �� Ư���� �����Ѵ�. (������ ��ǥ��)
		else if (ShowWindow[1] == true && m_pPlayer->GetPlayerStatus()->SpecialPoint >= 1) {

			// Attack
			if ((cursorPos.x > SELECT_SPECIAL_MINIMUM_X  && cursorPos.x < SELECT_SPECIAL_MAXIMUM_X)
				&& (cursorPos.y > SELECT_ATTACK_SPECIAL_MINIMUM_Y && cursorPos.y < SELECT_ATTACK_SPECIAL_MAXIMUM_Y))
			{
				// ��� ã�Ƽ� �ֱ�
				for (int i = 0; i < 4; ++i) {
					if (m_pPlayer->GetPlayerStatus()->Special[i] == (SpecialType::NoSelected)) {
						//m_pPlayer->GetPlayerStatus()->Special[i] = (SpecialType::AttackSpecial);
						CS_Msg_Set_Speacial_Point p;
						p.Ability_Type = (BYTE)SpecialType::AttackSpecial;
						p.Character_id = (BYTE)m_pNetwork->m_myid;
						p.size = sizeof(p);
						p.type = CS_SET_ABILITY_POINT;
						m_pNetwork->SendPacket(&p);

						break;
					};
				}

				// Ư�� ����Ʈ ���

				// Ư��â �ݱ�
				ShowWindow[1] = false;
				isRendering = false;

				return false;
			}

			// Defence
			if ((cursorPos.x > SELECT_SPECIAL_MINIMUM_X  && cursorPos.x < SELECT_SPECIAL_MAXIMUM_X)
				&& (cursorPos.y > SELECT_DEFENCE_SPECIAL_MINIMUM_Y && cursorPos.y < SELECT_DEFENCE_SPECIAL_MAXIMUM_Y))
			{
				// ��� ã�Ƽ� �ֱ�
				for (int i = 0; i < 4; ++i) {
					if (m_pPlayer->GetPlayerStatus()->Special[i] == (SpecialType::NoSelected)) {
						CS_Msg_Set_Speacial_Point p;
						p.Ability_Type = (BYTE)SpecialType::DefenceSpecial;
						p.Character_id = (BYTE)m_pNetwork->m_myid;
						p.size = sizeof(p);
						p.type = CS_SET_ABILITY_POINT;
						m_pNetwork->SendPacket(&p);
						break;
					};
				}

				// Ư�� ����Ʈ ���

				// Ư��â �ݱ�
				ShowWindow[1] = false;
				isRendering = false;

				return false;
			}

			// Tech
			if ((cursorPos.x > SELECT_SPECIAL_MINIMUM_X  && cursorPos.x < SELECT_SPECIAL_MAXIMUM_X)
				&& (cursorPos.y > SELECT_TECHNIC_SPECIAL_MINIMUM_Y && cursorPos.y < SELECT_TECHNIC_SPECIAL_MAXIMUM_Y))
			{
				// ��� ã�Ƽ� �ֱ�
				for (int i = 0; i < 4; ++i) {
					if (m_pPlayer->GetPlayerStatus()->Special[i] == (SpecialType::NoSelected)) {
						CS_Msg_Set_Speacial_Point p;
						p.Ability_Type = (BYTE)SpecialType::TechnicSpecial;
						p.Character_id = (BYTE)m_pNetwork->m_myid;
						p.size = sizeof(p);
						p.type = CS_SET_ABILITY_POINT;
						m_pNetwork->SendPacket(&p);
						break;
					};
				}

				// Ư�� ����Ʈ ���


				// Ư��â �ݱ�
				ShowWindow[1] = false;
				isRendering = false;

				return false;
			}
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////
// �����Լ�
D3D12_INPUT_LAYOUT_DESC CSpecialSelectShader::CreateInputLayout()
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

D3D12_SHADER_BYTECODE CSpecialSelectShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	//./Code/04.Shaders/99.GraphicsShader/
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/DefaultShader.hlsl",
		"VSTextured",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CSpecialSelectShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/DefaultShader.hlsl",
		"PSTextured",
		"ps_5_1",
		pShaderBlob));
}

void CSpecialSelectShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CSpecialSelectShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void * pContext)
{
	m_pCamera = (CCamera*)pContext;

	m_nObjects = 1;
	m_ppObjects = new CBaseObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 2);
	CreateShaderVariables(pCreateMgr, ncbElementBytes, m_nObjects);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pConstBuffer.Get(), ncbElementBytes);

	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CUIFrameObject *pUIObject{ NULL };

	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = Materials::CreateSpecialSelectWindowsMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);

	for (int i = 0; i < m_nObjects; ++i)
	{
		pUIObject = new CUIFrameObject(pCreateMgr, (UIFrameType::SpecialFrame));
		pUIObject->SetCamera(m_pCamera);
		pUIObject->SetDistance(FRAME_BUFFER_WIDTH / 128.0128f);	 // distance 10
		pUIObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * i));

		m_ppObjects[i] = pUIObject;
	}
}
