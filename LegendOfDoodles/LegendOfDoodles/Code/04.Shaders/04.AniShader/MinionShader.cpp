#include "stdafx.h"
#include "MinionShader.h"
#include "05.Objects/06.Minion/Minion.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"
#include "05.Objects/99.Material/Material.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"
#include "00.Global/01.Utility/07.ThrowingManager/ThrowingMgr.h"
#include "00.Global/01.Utility/06.HPGaugeManager/HPGaugeManager.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"
#include "00.Global/02.AI/00.FSMMgr/FSMMgr.h"
#include "07.Network/Network.h"

/// <summary>
/// 목적: 미니언 관리 및 그리기 용도
/// 최종 수정자:  김나단
/// 수정자 목록:  정휘현, 김나단
/// 최종 수정 날짜: 2018-10-01
/// </summary>

#define SwordMinionMesh m_ppMinionMeshes[0]
#define BowMinionMesh m_ppMinionMeshes[1]
#define StaffMinionMesh m_ppMinionMeshes[2]

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CMinionShader::CMinionShader(shared_ptr<CCreateMgr> pCreateMgr) : CShader(pCreateMgr)
{
	m_pCreateMgr = pCreateMgr;
}

CMinionShader::~CMinionShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CMinionShader::Initialize(shared_ptr<CCreateMgr> pCreateMgr, void *pContext)
{
	pCreateMgr->ResetCommandList();
	CreateShader(pCreateMgr, RENDER_TARGET_BUFFER_CNT, true, true);
	BuildObjects(pCreateMgr, pContext);
	pCreateMgr->ExecuteCommandList();
}

void CMinionShader::ReleaseUploadBuffers()
{
#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
#endif

	if (m_ppMinionMeshes)
	{
		for (int i = 0; i < m_nMeshes; ++i)
		{
			m_ppMinionMeshes[i]->ReleaseUploadBuffers();
		}
	}

	m_bBulidFinished = true;
}

void CMinionShader::UpdateShaderVariables(int opt)
{
	UNREFERENCED_PARAMETER(opt);
	static UINT elementBytes = ((sizeof(CB_ANIOBJECT_INFO) + 255) & ~255);

	CollisionObjectList* curObjectList{ NULL };
	for (int i = 0; i < 6; ++i)
	{
		switch (i)
		{
		case 0: // Blue Sword
			curObjectList = &m_blueSwordMinions;
			break;
		case 1: // Blue Staff
			curObjectList = &m_blueStaffMinions;
			break;
		case 2: // Blue Bow
			curObjectList = &m_blueBowMinions;
			break;
		case 3: // Red Sword
			curObjectList = &m_redSwordMinions;
			break;
		case 4: // Red Staff
			curObjectList = &m_redStaffMinions;
			break;
		case 5: // Red Bow
			curObjectList = &m_redBowMinions;
			break;
		}

		for (auto iter = curObjectList->begin(); iter != curObjectList->end(); ++iter)
		{
			CB_ANIOBJECT_INFO *pMappedObject = (CB_ANIOBJECT_INFO *)(m_pMappedObjects + ((*iter)->GetIndex() * elementBytes));
			XMFLOAT4X4 tmp[31];
			memcpy(tmp, (*iter)->GetFrameMatrix(), sizeof(XMFLOAT4X4) * 31);
			memcpy(pMappedObject->m_xmf4x4Frame, tmp, sizeof(XMFLOAT4X4) * 31);

			XMStoreFloat4x4(&pMappedObject->m_xmf4x4World0,
				XMMatrixTranspose(XMLoadFloat4x4((*iter)->GetWorldMatrix())));
		}
	}
}

void CMinionShader::UpdateBoundingBoxShaderVariables()
{
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CollisionObjectList* curObjectList{ NULL };
	for (int i = 0; i < 6; ++i)
	{
		switch (i)
		{
		case 0: // Blue Sword
			curObjectList = &m_blueSwordMinions;
			break;
		case 1: // Blue Staff
			curObjectList = &m_blueStaffMinions;
			break;
		case 2: // Blue Bow
			curObjectList = &m_blueBowMinions;
			break;
		case 3: // Red Sword
			curObjectList = &m_redSwordMinions;
			break;
		case 4: // Red Staff
			curObjectList = &m_redStaffMinions;
			break;
		case 5: // Red Bow
			curObjectList = &m_redBowMinions;
			break;
		}

		for (auto iter = curObjectList->begin(); iter != curObjectList->end(); ++iter)
		{
			CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedBoundingBoxes + ((*iter)->GetIndex() * boundingBoxElementBytes));

			XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
				XMMatrixTranspose(XMLoadFloat4x4((*iter)->GetWorldMatrix())));
		}
	}
}

void CMinionShader::AnimateObjects(float timeElapsed)
{
	// 리스트에서 제거할 조건 함수
	static auto removeFunc = [this](CCollisionObject* obj) {
		if (obj->GetState() == StatesType::Remove)
		{
			ResetPossibleIndex(obj->GetIndex(), (ObjectType)obj->GetType());
			delete obj;
			return true;
		}
		return false;
	};

	CollisionObjectList* curObjectList{ NULL };
	for (int i = 0; i < 6; ++i)
	{
		switch (i)
		{
		case 0: // Blue Sword
			curObjectList = &m_blueSwordMinions;
			break;
		case 1: // Blue Staff
			curObjectList = &m_blueStaffMinions;
			break;
		case 2: // Blue Bow
			curObjectList = &m_blueBowMinions;
			break;
		case 3: // Red Sword
			curObjectList = &m_redSwordMinions;
			break;
		case 4: // Red Staff
			curObjectList = &m_redStaffMinions;
			break;
		case 5: // Red Bow
			curObjectList = &m_redBowMinions;
			break;
		}

		for (auto iter = curObjectList->begin(); iter != curObjectList->end(); ++iter)
		{
			m_pFSMMgr->Update(timeElapsed, (*iter));
		}
		curObjectList->remove_if(removeFunc);
	}
}

void CMinionShader::Render(CCamera *pCamera)
{
	CollisionObjectList* curObjectList{ NULL };
	for (int i = 0; i < 6; ++i)
	{
		switch (i)
		{
		case 0: // Blue Sword
			CShader::Render(pCamera, 0);
			curObjectList = &m_blueSwordMinions;
			if (m_ppMaterials) m_ppMaterials[0]->UpdateShaderVariables();
			break;
		case 1: // Blue Staff
			CShader::Render(pCamera, 1);
			curObjectList = &m_blueStaffMinions;
			if (m_ppMaterials) m_ppMaterials[1]->UpdateShaderVariables();
			break;
		case 2: // Blue Bow
			CShader::Render(pCamera, 2);
			curObjectList = &m_blueBowMinions;
			if (m_ppMaterials) m_ppMaterials[2]->UpdateShaderVariables();
			break;
		case 3: // Red Sword
			CShader::Render(pCamera, 0);
			curObjectList = &m_redSwordMinions;
			if (m_ppMaterials) m_ppMaterials[3]->UpdateShaderVariables();
			break;
		case 4: // Red Staff
			CShader::Render(pCamera, 1);
			curObjectList = &m_redStaffMinions;
			if (m_ppMaterials) m_ppMaterials[4]->UpdateShaderVariables();
			break;
		case 5: // Red Bow
			CShader::Render(pCamera, 2);
			curObjectList = &m_redBowMinions;
			if (m_ppMaterials) m_ppMaterials[5]->UpdateShaderVariables();
			break;
		}

		for (auto iter = curObjectList->begin(); iter != curObjectList->end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
}

void CMinionShader::RenderBoundingBox(CCamera * pCamera)
{
	CShader::RenderBoundingBox(pCamera);

	CollisionObjectList* curObjectList{ NULL };
	for (int i = 0; i < 6; ++i)
	{
		switch (i)
		{
		case 0: // Blue Sword
			curObjectList = &m_blueSwordMinions;
			break;
		case 1: // Blue Staff
			curObjectList = &m_blueStaffMinions;
			break;
		case 2: // Blue Bow
			curObjectList = &m_blueBowMinions;
			break;
		case 3: // Red Sword
			curObjectList = &m_redSwordMinions;
			break;
		case 4: // Red Staff
			curObjectList = &m_redStaffMinions;
			break;
		case 5: // Red Bow
			curObjectList = &m_redBowMinions;
			break;
		}

		for (auto iter = curObjectList->begin(); iter != curObjectList->end(); ++iter)
		{
			(*iter)->RenderBoundingBox(pCamera);
		}
	}
}

void CMinionShader::RenderShadow(CCamera * pCamera)
{
	CollisionObjectList* curObjectList{ NULL };
	for (int i = 0; i < 6; ++i)
	{
		switch (i)
		{
		case 0: // Blue Sword
			CShader::Render(pCamera, 0, 2);
			curObjectList = &m_blueSwordMinions;
			break;
		case 1: // Blue Staff
			CShader::Render(pCamera, 1, 2);
			curObjectList = &m_blueStaffMinions;
			break;
		case 2: // Blue Bow
			CShader::Render(pCamera, 2, 2);
			curObjectList = &m_blueBowMinions;
			break;
		case 3: // Red Sword
			CShader::Render(pCamera, 0, 2);
			curObjectList = &m_redSwordMinions;
			break;
		case 4: // Red Staff
			CShader::Render(pCamera, 1, 2);
			curObjectList = &m_redStaffMinions;
			break;
		case 5: // Red Bow
			CShader::Render(pCamera, 2, 2);
			curObjectList = &m_redBowMinions;
			break;
		}

		for (auto iter = curObjectList->begin(); iter != curObjectList->end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
}

bool CMinionShader::OnProcessKeyInput(UCHAR* pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);

	return true;
}

void CMinionShader::SpawnMinion(ObjectType minionKind, short tag)
{
	static UINT incrementSize{ m_pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	static CCubeMesh boundingBoxMesh(m_pCreateMgr,
		CONVERT_PaperUnit_to_InG(3.0f), CONVERT_PaperUnit_to_InG(1.5f), CONVERT_PaperUnit_to_InG(7.0f),
		0, 0, -CONVERT_PaperUnit_to_InG(4.0f));
	boundingBoxMesh.AddRef();

	static CSkeleton SIdle("Resource//3D//Minion//Animation//Sword//Minion_S_Idle.aniinfo");
	static CSkeleton SAtk1("Resource//3D//Minion//Animation//Sword//Minion_S_Attack1.aniinfo");
	static CSkeleton SAtk2("Resource//3D//Minion//Animation//Sword//Minion_S_Attack2.aniinfo");
	static CSkeleton SWalkStart("Resource//3D//Minion//Animation//Sword//Minion_S_WalkStart.aniinfo");
	static CSkeleton SWalk("Resource//3D//Minion//Animation//Sword//Minion_S_Walk.aniinfo");
	static CSkeleton SDie("Resource//3D//Minion//Animation//Sword//Minion_S_Die.aniinfo");

	static CSkeleton BIdle("Resource//3D//Minion//Animation//Bow//Minion_B_Idle.aniinfo");
	static CSkeleton BAtk("Resource//3D//Minion//Animation//Bow//Minion_B_Attack.aniinfo");
	static CSkeleton BWalkStart("Resource//3D//Minion//Animation//Bow//Minion_B_WalkStart.aniinfo");
	static CSkeleton BWalk("Resource//3D//Minion//Animation//Bow//Minion_B_Walk.aniinfo");
	static CSkeleton BDie("Resource//3D//Minion//Animation//Bow//Minion_B_Die.aniinfo");

	static CSkeleton MIdle("Resource//3D//Minion//Animation//Magic//Minion_M_Idle.aniinfo");
	static CSkeleton MAtk1("Resource//3D//Minion//Animation//Magic//Minion_M_Attack1.aniinfo");
	static CSkeleton MAtk2("Resource//3D//Minion//Animation//Magic//Minion_M_Attack2.aniinfo");
	static CSkeleton MWalkStart("Resource//3D//Minion//Animation//Magic//Minion_M_WalkStart.aniinfo");
	static CSkeleton MWalk("Resource//3D//Minion//Animation//Magic//Minion_M_Walk.aniinfo");
	static CSkeleton MDie("Resource//3D//Minion//Animation//Magic//Minion_M_Die.aniinfo");

	static CSkeleton Die("Resource//3D//Minion//Animation//Minion_Die.aniinfo");

	if (!m_bDataPrepared)
	{
		SwordMinionMesh = new CSkinnedMesh(m_pCreateMgr, "Resource//3D//Minion//Mesh//Sword Minion.meshinfo");
		BowMinionMesh = new CSkinnedMesh(m_pCreateMgr, "Resource//3D//Minion//Mesh//Bow Minion.meshinfo");
		StaffMinionMesh = new CSkinnedMesh(m_pCreateMgr, "Resource//3D//Minion//Mesh//Magic Minion.meshinfo");

		SwordMinionMesh->SetBoundingBox(
			XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(4.0f)),
			XMFLOAT3(CONVERT_PaperUnit_to_InG(1.5f), CONVERT_PaperUnit_to_InG(1.5f), CONVERT_PaperUnit_to_InG(3.5f)));
		SwordMinionMesh->AddRef();
		BowMinionMesh->SetBoundingBox(
			XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(4.0f)),
			XMFLOAT3(CONVERT_PaperUnit_to_InG(1.5f), CONVERT_PaperUnit_to_InG(1.5f), CONVERT_PaperUnit_to_InG(3.5f)));
		BowMinionMesh->AddRef();
		StaffMinionMesh->SetBoundingBox(
			XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(4.0f)),
			XMFLOAT3(CONVERT_PaperUnit_to_InG(1.5f), CONVERT_PaperUnit_to_InG(1.5f), CONVERT_PaperUnit_to_InG(3.5f)));
		StaffMinionMesh->AddRef();
		m_bDataPrepared = true;
		return;
	}

	m_pCreateMgr->ResetCommandList();

	int wayKind{ 0 };

	CollisionObjectList objectAdder;
	for (; wayKind < 4; ++wayKind)
	{
		int index{ GetPossibleIndex(minionKind) };

		if (index == NONE) break;
		CMinion *pMinionObject{ NULL };

		switch (minionKind)
		{
		case ObjectType::SwordMinion:
			pMinionObject = new CSwordMinion(m_pCreateMgr);
			pMinionObject->SetMesh(0, SwordMinionMesh);
			pMinionObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * index));
			pMinionObject->SetCbvGPUDescriptorHandlePtrForBB(m_pcbvGPUDescriptorStartHandle[m_nHeaps - 1].ptr + (incrementSize * index));
			break;
		case ObjectType::StaffMinion:
			pMinionObject = new CMagicMinion(m_pCreateMgr);
			pMinionObject->SetMesh(0, StaffMinionMesh);
			pMinionObject->SetThrowingManager(m_pThrowingMgr);
			pMinionObject->SetEffectManager(m_pEffectMgr);
			pMinionObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[1].ptr + (incrementSize * (index - MAX_MINION)));
			pMinionObject->SetCbvGPUDescriptorHandlePtrForBB(m_pcbvGPUDescriptorStartHandle[m_nHeaps - 1].ptr + (incrementSize * index));
			break;
		case ObjectType::BowMinion:
			pMinionObject = new CBowMinion(m_pCreateMgr);
			pMinionObject->SetMesh(0, BowMinionMesh);
			pMinionObject->SetThrowingManager(m_pThrowingMgr);
			pMinionObject->SetEffectManager(m_pEffectMgr);
			pMinionObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[2].ptr + (incrementSize * (index - MAX_MINION * 2)));
			pMinionObject->SetCbvGPUDescriptorHandlePtrForBB(m_pcbvGPUDescriptorStartHandle[m_nHeaps - 1].ptr + (incrementSize * index));
			break;
		}

		pMinionObject->SetTag(tag++);
		pMinionObject->SetBoundingMesh(&boundingBoxMesh);
		pMinionObject->SetCollisionSize(CONVERT_PaperUnit_to_InG(2));

		switch (minionKind)
		{
		case ObjectType::SwordMinion:
			pMinionObject->SetSkeleton(&SIdle);
			pMinionObject->SetSkeleton(&SAtk1);
			pMinionObject->SetSkeleton(&SAtk2);
			pMinionObject->SetSkeleton(&SWalkStart);
			pMinionObject->SetSkeleton(&SWalk);
			pMinionObject->SetSkeleton(&SDie);
			break;
		case ObjectType::StaffMinion:
			pMinionObject->SetSkeleton(&MIdle);
			pMinionObject->SetSkeleton(&MAtk1);
			pMinionObject->SetSkeleton(&MAtk2);
			pMinionObject->SetSkeleton(&MWalkStart);
			pMinionObject->SetSkeleton(&MWalk);
			pMinionObject->SetSkeleton(&MDie);
			break;
		case ObjectType::BowMinion:
			pMinionObject->SetSkeleton(&BIdle);
			pMinionObject->SetSkeleton(&BAtk);
			pMinionObject->SetSkeleton(&BAtk);
			pMinionObject->SetSkeleton(&BWalkStart);
			pMinionObject->SetSkeleton(&BWalk);
			pMinionObject->SetSkeleton(&BDie);
			break;
		}

		pMinionObject->SetTerrain(m_pTerrain);

		pMinionObject->Rotate(90, 0, 0);

		pMinionObject->SaveIndex(index);

		XMFLOAT2 firstPos{ m_pathes[wayKind].front().From() };
		pMinionObject->CBaseObject::SetPosition(XMFLOAT3(firstPos.x, 0, firstPos.y));

		pMinionObject->SetCollisionManager(m_pColManager);

		if (wayKind == Minion_Species::Blue_Up || wayKind == Minion_Species::Blue_Down)
		{
			pMinionObject->SetTeam(TeamType::Blue);
			switch (minionKind)
			{
			case ObjectType::SwordMinion:
				m_blueSwordMinions.emplace_back(pMinionObject);
				break;
			case ObjectType::StaffMinion:
				m_blueStaffMinions.emplace_back(pMinionObject);
				break;
			case ObjectType::BowMinion:
				m_blueBowMinions.emplace_back(pMinionObject);
				break;
			}
		}
		else if (wayKind == Minion_Species::Red_Up || wayKind == Minion_Species::Red_Down)
		{
			pMinionObject->SetTeam(TeamType::Red);
			switch (minionKind)
			{
			case ObjectType::SwordMinion:
				m_redSwordMinions.emplace_back(pMinionObject);
				break;
			case ObjectType::StaffMinion:
				m_redStaffMinions.emplace_back(pMinionObject);
				break;
			case ObjectType::BowMinion:
				m_redBowMinions.emplace_back(pMinionObject);
				break;
			}
		}
		pMinionObject->SetPathToGo(new Path(m_pathes[wayKind]));

		objectAdder.emplace_back(pMinionObject);
	}
	m_pCreateMgr->ExecuteCommandList();

	for (auto& d : objectAdder)
	{
		d->ReleaseUploadBuffers();
		m_pColManager->AddCollider(d);
		m_pGaugeManger->AddMinionObject(d);
	}
}

CollisionObjectList * CMinionShader::GetMinionList(ObjectType Minion_Type, TeamType Team_Type)
{
	if (Team_Type == TeamType::Blue)
	{
		if (Minion_Type == ObjectType::SwordMinion)
		{
			return &m_blueSwordMinions;
		}
		if (Minion_Type == ObjectType::BowMinion)
		{
			return &m_blueBowMinions;
		}

		if (Minion_Type == ObjectType::StaffMinion)
		{
			return &m_blueStaffMinions;
		}

	}
	if (Team_Type == TeamType::Red)
	{
		if (Minion_Type == ObjectType::SwordMinion)
		{
			return &m_redSwordMinions;
		}
		if (Minion_Type == ObjectType::BowMinion)
		{
			return &m_redBowMinions;
		}

		if (Minion_Type == ObjectType::StaffMinion)
		{
			return &m_redStaffMinions;
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CMinionShader::CreateInputLayout()
{
	UINT nInputElementDescs = 6;
	D3D12_INPUT_ELEMENT_DESC *pInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pInputElementDescs[0] = {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[1] = {
		"NORMAL",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[2] = {
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	pInputElementDescs[3] = {
		"WEIGHTS",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[4] = {
		"BONEINDICES",
		0,
		DXGI_FORMAT_R8G8B8A8_UINT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	pInputElementDescs[5] = {
		"TANGENT",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CMinionShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/BoneShader.hlsl",
		"VSBone",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CMinionShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/BoneShader.hlsl",
		"PSBone",
		"ps_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CMinionShader::CreateShadowVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/ShadowShader.hlsl",
		"VSBone",
		"vs_5_1",
		pShaderBlob));
}

void CMinionShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 3;

	m_nHeaps = 3 + 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CMinionShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext)
{
	if (pContext) m_pTerrain = (CHeightMapTerrain*)pContext;

	UINT ncbElementBytes = ((sizeof(CB_ANIOBJECT_INFO) + 255) & ~255);
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	m_possibleIndicies[ObjectType::SwordMinion] = new bool[MAX_MINION] {};
	m_possibleIndicies[ObjectType::StaffMinion] = new bool[MAX_MINION] {};
	m_possibleIndicies[ObjectType::BowMinion] = new bool[MAX_MINION] {};

	CreateShaderVariables(pCreateMgr, ncbElementBytes, MAX_MINION * 3, true, boundingBoxElementBytes, MAX_MINION * 3);

	for (int i = 0; i < 3; ++i)
	{
		CreateCbvAndSrvDescriptorHeaps(pCreateMgr, MAX_MINION, 1, i);
		CreateConstantBufferViews(pCreateMgr, MAX_MINION, m_pConstBuffer.Get(), ncbElementBytes, MAX_MINION * i, i);
	}
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, MAX_MINION * 3, 0, m_nHeaps - 1);
	CreateConstantBufferViews(pCreateMgr, MAX_MINION * 3, m_pBoundingBoxBuffer.Get(), boundingBoxElementBytes, 0, m_nHeaps - 1);

	SaveBoundingBoxHeapNumber(m_nHeaps - 1);

#if USE_BATCH_MATERIAL
	m_nMaterials = 6;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	// Blue
	m_ppMaterials[0] = Materials::CreateSwordMinionMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[0]->SetAlbedo(XMFLOAT4(0.2f, 0.2f, 1.0f, 1.0f));

	m_ppMaterials[1] = Materials::CreateMagicMinionMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);
	m_ppMaterials[1]->SetAlbedo(XMFLOAT4(0.2f, 0.2f, 1.0f, 1.0f));

	m_ppMaterials[2] = Materials::CreateBowMinionMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[2], &m_psrvGPUDescriptorStartHandle[2]);
	m_ppMaterials[2]->SetAlbedo(XMFLOAT4(0.2f, 0.2f, 1.0f, 1.0f));

	// Red
	m_ppMaterials[3] = Materials::CreateSwordMinionMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[3]->SetAlbedo(XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f));

	m_ppMaterials[4] = Materials::CreateMagicMinionMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);
	m_ppMaterials[4]->SetAlbedo(XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f));

	m_ppMaterials[5] = Materials::CreateBowMinionMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[2], &m_psrvGPUDescriptorStartHandle[2]);
	m_ppMaterials[5]->SetAlbedo(XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f));
#else
	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#endif

	CreatePathes();
	SpawnMinion();
}

void CMinionShader::ReleaseObjects()
{
	// Blue Minions Release
	for (auto iter = m_blueSwordMinions.begin(); iter != m_blueSwordMinions.end();)
	{
		delete(*iter);
		iter = m_blueSwordMinions.erase(iter);
	}
	m_blueSwordMinions.clear();
	for (auto iter = m_blueStaffMinions.begin(); iter != m_blueStaffMinions.end();)
	{
		delete(*iter);
		iter = m_blueStaffMinions.erase(iter);
	}
	m_blueStaffMinions.clear();
	for (auto iter = m_blueBowMinions.begin(); iter != m_blueBowMinions.end();)
	{
		delete(*iter);
		iter = m_blueBowMinions.erase(iter);
	}
	m_blueBowMinions.clear();

	// Red Minions Release
	for (auto iter = m_redSwordMinions.begin(); iter != m_redSwordMinions.end(); )
	{
		delete(*iter);
		iter = m_redSwordMinions.erase(iter);
	}
	m_redSwordMinions.clear();
	for (auto iter = m_redStaffMinions.begin(); iter != m_redStaffMinions.end(); )
	{
		delete(*iter);
		iter = m_redStaffMinions.erase(iter);
	}
	m_redStaffMinions.clear();
	for (auto iter = m_redBowMinions.begin(); iter != m_redBowMinions.end(); )
	{
		delete(*iter);
		iter = m_redBowMinions.erase(iter);
	}
	m_redBowMinions.clear();

#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i < m_nMaterials; ++i)
		{
			Safe_Delete(m_ppMaterials[i]);
		}
		Safe_Delete_Array(m_ppMaterials);
	}
#endif

	if (m_ppMinionMeshes)
	{
		for (int i = 0; i < m_nMeshes; ++i)
		{
			m_ppMinionMeshes[i]->Release();
		}
	}

	if (m_possibleIndicies[ObjectType::SwordMinion]) Safe_Delete_Array(m_possibleIndicies[ObjectType::SwordMinion]);
	if (m_possibleIndicies[ObjectType::StaffMinion]) Safe_Delete_Array(m_possibleIndicies[ObjectType::StaffMinion]);
	if (m_possibleIndicies[ObjectType::BowMinion]) Safe_Delete_Array(m_possibleIndicies[ObjectType::BowMinion]);
}

void CMinionShader::CreatePathes()
{
	CTransformImporter transformInporter;
	transformInporter.LoadMeshData("Resource/Data/Pathes.txt");
	for (int i = 0, cnt = 0; i < 4; ++i)
	{
		for (int j = 0; j < transformInporter.m_iKindMeshCnt[i] - 1; ++j, ++cnt)
		{
			XMFLOAT3 from = transformInporter.m_Transform[cnt].pos;
			XMFLOAT3 to = transformInporter.m_Transform[cnt + 1].pos;
			m_pathes[i].push_back(CPathEdge(XMFLOAT2(CONVERT_Unit_to_InG(from.x), CONVERT_Unit_to_InG(from.z)), XMFLOAT2(CONVERT_Unit_to_InG(to.x), CONVERT_Unit_to_InG(to.z))));
		}
		++cnt;
	}
}

int CMinionShader::GetPossibleIndex(ObjectType type)
{
	for (int idx = 0; idx < MAX_MINION; ++idx)
	{
		if (!m_possibleIndicies[type][idx])
		{
			m_possibleIndicies[type][idx] = true;
			if (type == ObjectType::SwordMinion)
				return idx;
			else if (type == ObjectType::StaffMinion)
				return idx + MAX_MINION;
			else if (type == ObjectType::BowMinion)
				return idx + MAX_MINION * 2;
		}
	}
	return NONE;
}

void CMinionShader::SetPossibleIndex(int idx, ObjectType type)
{
	if (type == ObjectType::SwordMinion)
		m_possibleIndicies[type][idx] = true;
	else if (type == ObjectType::StaffMinion)
		m_possibleIndicies[type][idx - MAX_MINION] = true;
	else if (type == ObjectType::BowMinion)
		m_possibleIndicies[type][idx - MAX_MINION * 2] = true;
}

void CMinionShader::ResetPossibleIndex(int idx, ObjectType type)
{
	if (type == ObjectType::SwordMinion)
		m_possibleIndicies[type][idx] = false;
	else if (type == ObjectType::StaffMinion)
		m_possibleIndicies[type][idx - MAX_MINION] = false;
	else if (type == ObjectType::BowMinion)
		m_possibleIndicies[type][idx - MAX_MINION * 2] = false;
}