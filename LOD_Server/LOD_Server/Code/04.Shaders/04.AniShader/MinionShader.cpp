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

/// <summary>
/// 목적: 미니언 관리 및 그리기 용도
/// 최종 수정자:  김나단
/// 수정자 목록:  정휘현, 김나단
/// 최종 수정 날짜: 2018-08-01
/// </summary>

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
	CreateShader(pCreateMgr, RENDER_TARGET_BUFFER_CNT, true, true);
	BuildObjects(pCreateMgr, pContext);
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
			XMFLOAT4X4 tmp[128];
			memcpy(tmp, (*iter)->GetFrameMatrix(), sizeof(XMFLOAT4X4) * 128);
			memcpy(pMappedObject->m_xmf4x4Frame, tmp, sizeof(XMFLOAT4X4) * 128);

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
			ResetPossibleIndex(obj->GetIndex());
			delete obj;
			return true;
		}
		return false;
	};

	//m_spawnTime += timeElapsed;

	//if (m_spawnTime >= 0.0f && m_spawnTime <= 5.0f)
	//{
	//	bool spawned{ false };
	//	for (float time = m_spawnTime - m_preSpawnTime; time >= 0.25; time -= 0.25)
	//	{
	//		spawned = true;
	//		SpawnMinion();
	//	}
	//	if(spawned) m_preSpawnTime = m_spawnTime;
	//}

	//if (m_spawnTime >= 30.0f)
	//{
	//	m_spawnTime -= 30.0f;
	//	m_preSpawnTime = -0.25f;
	//	m_curSpawnCount = 0;
	//}

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
	CShader::Render(pCamera, 0);

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

		if (m_ppMaterials) m_ppMaterials[i]->UpdateShaderVariables();
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
	CShader::Render(pCamera, 0, 2);

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
			(*iter)->Render(pCamera);
		}
	}
}

CBaseObject *CMinionShader::PickObjectByRayIntersection(
	XMFLOAT3& pickPosition, XMFLOAT4X4& xmf4x4View, float &nearHitDistance)
{
	bool intersected = 0;

	nearHitDistance = FLT_MAX;
	float hitDistance = FLT_MAX;
	CCollisionObject *pSelectedObject{ NULL };

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
			intersected = (*iter)->PickObjectByRayIntersection(pickPosition, xmf4x4View, hitDistance);
			if (intersected && (hitDistance < nearHitDistance))
			{
				nearHitDistance = hitDistance;
				pSelectedObject = (*iter);
			}
		}
	}

	return(pSelectedObject);
}

bool CMinionShader::OnProcessKeyInput(UCHAR* pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);

	static float R = 0.0f;
	static float M = 0.0f;

	if (GetAsyncKeyState('N') & 0x0001)
	{
		SpawnMinion();
	}

	return true;
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
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSBone",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CMinionShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
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

	m_nHeaps = 2;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CMinionShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext)
{
	if (pContext) m_pTerrain = (CHeightMapTerrain*)pContext;

	UINT ncbElementBytes = ((sizeof(CB_ANIOBJECT_INFO) + 255) & ~255);
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateShaderVariables(pCreateMgr, ncbElementBytes, MAX_MINION, true, boundingBoxElementBytes, MAX_MINION);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, MAX_MINION, 1, 0);
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, MAX_MINION, 0, 1);

	CreateConstantBufferViews(pCreateMgr, MAX_MINION, m_pConstBuffer.Get(), ncbElementBytes, 0, 0);
	CreateConstantBufferViews(pCreateMgr, MAX_MINION, m_pBoundingBoxBuffer.Get(), boundingBoxElementBytes, 0, 1);

	SaveBoundingBoxHeapNumber(1);

#if USE_BATCH_MATERIAL
	m_nMaterials = 6;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	// Blue
	m_ppMaterials[0] = Materials::CreateSwordMinionMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[0]->SetAlbedo(XMFLOAT4(0.2f, 0.2f, 1.0f, 1.0f));
	m_ppMaterials[1] = Materials::CreateStaffMinionMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[1]->SetAlbedo(XMFLOAT4(0.2f, 0.2f, 1.0f, 1.0f));
	m_ppMaterials[2] = Materials::CreateBowMinionMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[2]->SetAlbedo(XMFLOAT4(0.2f, 0.2f, 1.0f, 1.0f));
	// Red
	m_ppMaterials[3] = Materials::CreateSwordMinionMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[3]->SetAlbedo(XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f));
	m_ppMaterials[4] = Materials::CreateStaffMinionMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[4]->SetAlbedo(XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f));
	m_ppMaterials[5] = Materials::CreateBowMinionMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
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

int CMinionShader::GetPossibleIndex()
{
	for (int idx = 0; idx < MAX_MINION; ++idx)
	{
		if (!m_indexArr[idx])
		{
			m_indexArr[idx] = true;
			return idx;
		}
	}
	return NONE;
}

void CMinionShader::SpawnMinion()
{
	static bool dataPrepared{ false };
	static CSkinnedMesh swordMinionMesh(m_pCreateMgr, "Resource//3D//Minion//Mesh//Sword Minion.meshinfo");
	static CSkinnedMesh bowMinionMesh(m_pCreateMgr, "Resource//3D//Minion//Mesh//Bow Minion.meshinfo");
	static CSkinnedMesh staffMinionMesh(m_pCreateMgr, "Resource//3D//Minion//Mesh//Magic Minion.meshinfo");
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

	if (!dataPrepared)
	{
		swordMinionMesh.SetBoundingBox(
			XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(4.0f)),
			XMFLOAT3(CONVERT_PaperUnit_to_InG(1.5f), CONVERT_PaperUnit_to_InG(1.5f), CONVERT_PaperUnit_to_InG(3.5f)));
		swordMinionMesh.AddRef();
		bowMinionMesh.SetBoundingBox(
			XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(4.0f)),
			XMFLOAT3(CONVERT_PaperUnit_to_InG(1.5f), CONVERT_PaperUnit_to_InG(1.5f), CONVERT_PaperUnit_to_InG(3.5f)));
		bowMinionMesh.AddRef();
		staffMinionMesh.SetBoundingBox(
			XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(4.0f)),
			XMFLOAT3(CONVERT_PaperUnit_to_InG(1.5f), CONVERT_PaperUnit_to_InG(1.5f), CONVERT_PaperUnit_to_InG(3.5f)));
		staffMinionMesh.AddRef();
		dataPrepared = true;
		return;
	}

	m_pCreateMgr->ResetCommandList();

	int wayKind{ 0 };

	m_kind = ObjectType::BowMinion;

	//// 전체 생성되는 수의 절반은 근접 미니언
	//if (m_curSpawnCount < 10)
	//{
	//	m_kind = ObjectType::SwordMinion;
	//}
	//// 나머지 절반 마법 / 원기리 미니언
	//else if (m_curSpawnCount < 15)
	//{
	//	m_kind = ObjectType::StaffMinion;
	//}
	//else
	//{
	//	m_kind = ObjectType::BowMinion;
	//}

	CollisionObjectList objectAdder;
	for (; wayKind < 4; ++wayKind)
	{
		int index{ GetPossibleIndex() };

		if (index == NONE) break;
		CMinion *pMinionObject{ NULL };

		switch (m_kind)
		{
		case ObjectType::SwordMinion:
			pMinionObject = new CSwordMinion(m_pCreateMgr);
			pMinionObject->SetMesh(0, &swordMinionMesh);
			break;
		case ObjectType::StaffMinion:
			pMinionObject = new CMagicMinion(m_pCreateMgr);
			pMinionObject->SetMesh(0, &staffMinionMesh);
			pMinionObject->SetThrowingManager(m_pThrowingMgr);
			break;
		case ObjectType::BowMinion:
			pMinionObject = new CBowMinion(m_pCreateMgr);
			pMinionObject->SetMesh(0, &bowMinionMesh);
			pMinionObject->SetThrowingManager(m_pThrowingMgr);
			break;
		}

		pMinionObject->SetBoundingMesh(&boundingBoxMesh);
		pMinionObject->SetCollisionSize(CONVERT_PaperUnit_to_InG(2));

		switch (m_kind)
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

		pMinionObject->SetPathToGo(new Path(m_pathes[wayKind]));

		XMFLOAT2 firstPos{ m_pathes[wayKind].front().From() };
		pMinionObject->CBaseObject::SetPosition(XMFLOAT3(firstPos.x, 0, firstPos.y));

		pMinionObject->SetCollisionManager(m_pColManager);

		pMinionObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * index));
		pMinionObject->SetCbvGPUDescriptorHandlePtrForBB(m_pcbvGPUDescriptorStartHandle[1].ptr + (incrementSize * index));

		if (wayKind == Minion_Species::Blue_Up || wayKind == Minion_Species::Blue_Down)
		{
			pMinionObject->SetTeam(TeamType::Blue);
			switch (m_kind)
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
			switch (m_kind)
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
		objectAdder.emplace_back(pMinionObject);
	}
	m_pCreateMgr->ExecuteCommandList();

	for (auto& d : objectAdder)
	{
		d->ReleaseUploadBuffers();
		m_pColManager->AddCollider(d);
		m_pGaugeManger->AddMinionObject(d);
	}

	// 현재 웨이브에서 미니언이 생성된 개수
	++m_curSpawnCount;
}
