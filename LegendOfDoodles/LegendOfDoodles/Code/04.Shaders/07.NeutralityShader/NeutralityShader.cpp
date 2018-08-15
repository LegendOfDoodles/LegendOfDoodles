#include "stdafx.h"
#include "NeutralityShader.h"
#include "05.Objects/10.Neutrality/00.Roider/Roider.h"
#include "05.Objects/10.Neutrality/01.Golem/Golem.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"
#include "05.Objects/99.Material/Material.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"
#include "00.Global/01.Utility/06.HPGaugeManager/HPGaugeManager.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"
#include "00.Global/02.AI/00.FSMMgr/FSMMgr.h"
#include "07.Network/Network.h"

/// <summary>
/// 목적: 중립 몬스터 관리 및 렌더링용 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-08-03
/// </summary>

#define Roider_NetralMaterial m_ppMaterials[0]
#define Roider_BlueMaterial m_ppMaterials[1]
#define Roider_RedMaterial m_ppMaterials[2]
#define Golem_NetralMaterial m_ppMaterials[3]
#define Golem_BlueMaterial m_ppMaterials[4]
#define Golem_RedMaterial m_ppMaterials[5]

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CNeutralityShader::CNeutralityShader(shared_ptr<CCreateMgr> pCreateMgr) : CShader(pCreateMgr)
{
	m_pCreateMgr = pCreateMgr;
}

CNeutralityShader::~CNeutralityShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CNeutralityShader::Initialize(shared_ptr<CCreateMgr> pCreateMgr, void *pContext)
{
	CreateShader(pCreateMgr, RENDER_TARGET_BUFFER_CNT, true, true);
	BuildObjects(pCreateMgr, pContext);
}

void CNeutralityShader::UpdateShaderVariables(int opt)
{
	static UINT elementBytes = ((sizeof(CB_ANIOBJECT_INFO) + 255) & ~255);
	int beg{ 0 }, end{ 0 };

	switch (opt)
	{
	case 0:	// Roider Update
		beg = m_objectsIndices[ObjectType::Roider].m_begIndex;
		end = m_objectsIndices[ObjectType::Roider].m_endIndex;
		break;
	case 1:	// Golem Update
		beg = m_objectsIndices[ObjectType::GOLEM].m_begIndex;
		end = m_objectsIndices[ObjectType::GOLEM].m_endIndex;
		break;
	}

	for (int i = beg; i < end; ++i)
	{
		CB_ANIOBJECT_INFO *pMappedObject = (CB_ANIOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		XMFLOAT4X4 tmp[128];
		memcpy(tmp, m_ppObjects[i]->GetFrameMatrix(), sizeof(XMFLOAT4X4) * 128);
		memcpy(pMappedObject->m_xmf4x4Frame, tmp, sizeof(XMFLOAT4X4) * 128);

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World0,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CNeutralityShader::UpdateBoundingBoxShaderVariables()
{
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedBoundingBoxes + (i * boundingBoxElementBytes));

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CNeutralityShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_pFSMMgr->Update(timeElapsed, m_ppObjects[j]);
	}
}

void CNeutralityShader::Render(CCamera *pCamera)
{
	CShader::Render(pCamera);

	int beg, end;

	beg = m_objectsIndices[ObjectType::Roider].m_begIndex;
	end = m_objectsIndices[ObjectType::Roider].m_endIndex;
#if USE_BATCH_MATERIAL
	if (m_ppMaterials) Roider_NetralMaterial->UpdateShaderVariables();
#endif
	for (int j = beg; j < end; j++)
	{
		if(m_ppObjects[j]->GetTeam() == TeamType::Neutral)
			m_ppObjects[j]->Render(pCamera);
	}

#if USE_BATCH_MATERIAL
	if (m_ppMaterials) Roider_BlueMaterial->UpdateShaderVariables();
#endif
	for (int j = beg; j < end; j++)
	{
		if (m_ppObjects[j]->GetTeam() == TeamType::Blue)
			m_ppObjects[j]->Render(pCamera);
	}

#if USE_BATCH_MATERIAL
	if (m_ppMaterials) Roider_RedMaterial->UpdateShaderVariables();
#endif
	for (int j = beg; j < end; j++)
	{
		if (m_ppObjects[j]->GetTeam() == TeamType::Red)
			m_ppObjects[j]->Render(pCamera);
	}

	CShader::Render(pCamera, 1);

	beg = m_objectsIndices[ObjectType::GOLEM].m_begIndex;
	end = m_objectsIndices[ObjectType::GOLEM].m_endIndex;
	if (m_ppObjects[beg]->GetTeam() == TeamType::Neutral)
	{
#if USE_BATCH_MATERIAL
		if (m_ppMaterials) Golem_NetralMaterial->UpdateShaderVariables();
#endif
		m_ppObjects[beg]->Render(pCamera);
	}
	else if (m_ppObjects[beg]->GetTeam() == TeamType::Blue)
	{
#if USE_BATCH_MATERIAL
		if (m_ppMaterials) Golem_BlueMaterial->UpdateShaderVariables();
#endif
		m_ppObjects[beg]->Render(pCamera);
	}
	else if (m_ppObjects[beg]->GetTeam() == TeamType::Red)
	{
#if USE_BATCH_MATERIAL
		if (m_ppMaterials) Golem_RedMaterial->UpdateShaderVariables();
#endif
		m_ppObjects[beg]->Render(pCamera);
	}
}

void CNeutralityShader::RenderBoundingBox(CCamera * pCamera)
{
	CShader::RenderBoundingBox(pCamera);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]) m_ppObjects[j]->RenderBoundingBox(pCamera);
	}
}

void CNeutralityShader::RenderShadow(CCamera * pCamera)
{
	CShader::Render(pCamera, 0, 2);

	int beg, end;

	beg = m_objectsIndices[ObjectType::Roider].m_begIndex;
	end = m_objectsIndices[ObjectType::Roider].m_endIndex;
	for (int j = beg; j < end; j++)
	{
		m_ppObjects[j]->Render(pCamera);
	}

	CShader::Render(pCamera, 1, 2);

	beg = m_objectsIndices[ObjectType::GOLEM].m_begIndex;
	end = m_objectsIndices[ObjectType::GOLEM].m_endIndex;
	m_ppObjects[beg]->Render(pCamera);
}

bool CNeutralityShader::OnProcessKeyInput(UCHAR* pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);

	return true;
}

void CNeutralityShader::SetColManagerToObject(shared_ptr<CCollisionManager> manager)
{
	for (int i = 0; i < m_nObjects; ++i) 
	{
		m_ppObjects[i]->SetCollisionManager(manager);
	}
}

void CNeutralityShader::SetThrowingManagerToObject(shared_ptr<CThrowingMgr> manager)
{
	for (int i = 0; i < m_nObjects; ++i)
	{
		m_ppObjects[i]->SetThrowingManager(manager);
	}
}

void CNeutralityShader::SetEffectManagerToObject(shared_ptr<CEffectMgr> manager)
{
	for (int i = m_nObjects - 1; i < m_nObjects; ++i) {
		m_ppObjects[i]->SetEffectManager(manager);
	}
}

void CNeutralityShader::SetSoundManagerToObject(shared_ptr<CSoundManager> manager)
{
	for (int i = m_nObjects - 1; i < m_nObjects; ++i) {
		m_ppObjects[i]->SetSoundManager(manager);
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CNeutralityShader::CreateInputLayout()
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

D3D12_SHADER_BYTECODE CNeutralityShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", 
		"VSBone",
		"vs_5_1", 
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CNeutralityShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", 
		"PSBone",
		"ps_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CNeutralityShader::CreateShadowVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/ShadowShader.hlsl",
		"VSBone",
		"vs_5_1",
		pShaderBlob));
}

void CNeutralityShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 3;

	m_nHeaps = 3;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CNeutralityShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext)
{
	if (pContext) m_pTerrain = (CHeightMapTerrain*)pContext;

	CreatePathes();

	CTransformImporter transformInporter;

	transformInporter.LoadMeshData("Resource//Data//MonsterSetting.txt");

	m_objectsIndices[ObjectType::Roider] = NeutralObjectIndices(0, transformInporter.m_iKindMeshCnt[0]);
	m_objectsIndices[ObjectType::GOLEM] = NeutralObjectIndices(transformInporter.m_iKindMeshCnt[0], transformInporter.m_iKindMeshCnt[0] + 1);

	m_nObjects = transformInporter.m_iKindMeshCnt[0] + 1;
	m_ppObjects = new CCollisionObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_ANIOBJECT_INFO) + 255) & ~255);
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, transformInporter.m_iKindMeshCnt[0], 1, 0);
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 1, 1, 1);
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 0, 2);

	CreateShaderVariables(pCreateMgr, ncbElementBytes, m_nObjects, true, boundingBoxElementBytes, m_nObjects);

	CreateConstantBufferViews(pCreateMgr, transformInporter.m_iKindMeshCnt[0], m_pConstBuffer.Get(), ncbElementBytes, 0, 0);
	CreateConstantBufferViews(pCreateMgr, 1, m_pConstBuffer.Get(), ncbElementBytes, transformInporter.m_iKindMeshCnt[0], 1);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pBoundingBoxBuffer.Get(), boundingBoxElementBytes, 0, 2);

	SaveBoundingBoxHeapNumber(2);

#if USE_BATCH_MATERIAL
	m_nMaterials = 6;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = Materials::CreatePlayerMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[1] = Materials::CreatePlayerMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[2] = Materials::CreatePlayerMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);

	m_ppMaterials[3] = Materials::CreatePlayerMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);
	m_ppMaterials[4] = Materials::CreatePlayerMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);
	m_ppMaterials[5] = Materials::CreatePlayerMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);

	Roider_NetralMaterial->SetAlbedo(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	Roider_BlueMaterial->SetAlbedo(XMFLOAT4(0.2f, 0.2f, 1.0f, 1.0f));
	Roider_RedMaterial->SetAlbedo(XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f));

	Golem_NetralMaterial->SetAlbedo(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	Golem_BlueMaterial->SetAlbedo(XMFLOAT4(0.2f, 0.2f, 1.0f, 1.0f));
	Golem_RedMaterial->SetAlbedo(XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f));
#else
	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#endif

	CSkinnedMesh *pRoiderMesh = new CSkinnedMesh(pCreateMgr, "Resource//3D//Monster//Mesh//Royde.meshinfo");

	CCubeMesh *pBoundingBoxMesh = new CCubeMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(4.0f), CONVERT_PaperUnit_to_InG(1.0f), CONVERT_PaperUnit_to_InG(11.0f),
		0, 0, -CONVERT_PaperUnit_to_InG(7.0f));

	CSkeleton *pSIdle = new CSkeleton("Resource//3D//Monster//Animation//Royde_Idle.aniinfo");
	CSkeleton *pSStartWalk = new CSkeleton("Resource//3D//Monster//Animation//Royde_Start_Walk.aniinfo");
	CSkeleton *pSWalk = new CSkeleton("Resource//3D//Monster//Animation//Royde_Walk.aniinfo");
	CSkeleton *pSSmash = new CSkeleton("Resource//3D//Monster//Animation//Royde_Attack1.aniinfo");
	CSkeleton *pSThrow = new CSkeleton("Resource//3D//Monster//Animation//Royde_Attack2.aniinfo");
	CSkeleton *pSDie = new CSkeleton("Resource//3D//Monster//Animation//Royde_Die.aniinfo");

	pRoiderMesh->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(7.0f)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(2.0f), CONVERT_PaperUnit_to_InG(1.0f), CONVERT_PaperUnit_to_InG(5.5f)));

	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CRoider *pRoider{ NULL };

	CTransformImporter nexusTransformInporter;
	nexusTransformInporter.LoadMeshData("Resource//Data//NexusTowerSetting.txt");

	XMFLOAT3 blueNexusPos{ 
		XMFLOAT3(
			CONVERT_Unit_to_InG(nexusTransformInporter.m_Transform[0].pos.x), 
			CONVERT_Unit_to_InG(nexusTransformInporter.m_Transform[0].pos.y), 
			CONVERT_Unit_to_InG(nexusTransformInporter.m_Transform[0].pos.z)) 
	};
	XMFLOAT3 redNexusPos{
		XMFLOAT3(
			CONVERT_Unit_to_InG(nexusTransformInporter.m_Transform[1].pos.x),
			CONVERT_Unit_to_InG(nexusTransformInporter.m_Transform[1].pos.y),
			CONVERT_Unit_to_InG(nexusTransformInporter.m_Transform[1].pos.z))
	};

	// Roider Setting
	int cnt{ 0 };
	for (int j = 0; j < transformInporter.m_iKindMeshCnt[0]; ++j, ++cnt) {
		XMFLOAT3 pos{ transformInporter.m_Transform[cnt].pos };
		XMFLOAT3 rot{ transformInporter.m_Transform[cnt].rotation };

		pRoider = new CRoider(pCreateMgr, 1);

		pRoider->SetMesh(0, pRoiderMesh);

		pRoider->SetType(ObjectType::Roider);
#if !USE_BATCH_MATERIAL
		pRotatingObject->SetMaterial(pCubeMaterial);
#endif
		pRoider->SetBoundingMesh(pBoundingBoxMesh);
		pRoider->SetCollisionSize(CONVERT_PaperUnit_to_InG(4));

		pRoider->CBaseObject::SetPosition(CONVERT_Unit_to_InG(pos.x), CONVERT_Unit_to_InG(pos.y), CONVERT_Unit_to_InG(pos.z));
		pRoider->SetTeam(TeamType::Neutral);
		
		pRoider->SetSkeleton(pSIdle);
		pRoider->SetSkeleton(pSStartWalk);
		pRoider->SetSkeleton(pSWalk);

		pRoider->SetSkeleton(pSSmash);
		pRoider->SetSkeleton(pSThrow);
		pRoider->SetSkeleton(pSDie);

		pRoider->SetTerrain(m_pTerrain);

		pRoider->Rotate(0, 180, 0);
		pRoider->Rotate(-rot.x, rot.y, -rot.z);

		pRoider->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * cnt));
		pRoider->SetCbvGPUDescriptorHandlePtrForBB(m_pcbvGPUDescriptorStartHandle[2].ptr + (incrementSize * cnt));
		
		pRoider->SetPathes(m_pathes);

		pRoider->SetNexusPoses(blueNexusPos, redNexusPos);
		pRoider->SaveCurrentState();
		pRoider->SetTag(static_cast<short>(20000 + cnt));

		m_ppObjects[cnt] = pRoider;
	}

	// Golem Setting
	{
		CGolem *pGolem{ NULL };
		XMFLOAT3 pos{ transformInporter.m_Transform[cnt].pos };
		XMFLOAT3 rot{ transformInporter.m_Transform[cnt].rotation };

		pGolem = new CGolem(pCreateMgr, 1);

		pGolem->SetType(ObjectType::GOLEM);
#if !USE_BATCH_MATERIAL
		pRotatingObject->SetMaterial(pCubeMaterial);
#endif
		pGolem->CBaseObject::SetPosition(
			CONVERT_Unit_to_InG(pos.x), 
			m_pTerrain->GetHeight(CONVERT_Unit_to_InG(pos.x),CONVERT_Unit_to_InG(pos.z)), 
			CONVERT_Unit_to_InG(pos.z));
		pGolem->SetTeam(TeamType::Neutral);

		pGolem->SetTerrain(m_pTerrain);

		pGolem->Rotate(0, 180, 0);
		pGolem->Rotate(-rot.x, rot.y, -rot.z);

		pGolem->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[1].ptr);
		pGolem->SetCbvGPUDescriptorHandlePtrForBB(m_pcbvGPUDescriptorStartHandle[2].ptr + (incrementSize * cnt));

		pGolem->SetPathes(m_pathes);

		pGolem->SetNexusPoses(blueNexusPos, redNexusPos);
		pGolem->SaveCurrentState();
		pGolem->SetTag(static_cast<short>(20000 + cnt));

		m_ppObjects[cnt] = pGolem;
		cnt++;
	}

	Safe_Delete(pSIdle);
	Safe_Delete(pSStartWalk);
	Safe_Delete(pSWalk);
	Safe_Delete(pSThrow);
	Safe_Delete(pSSmash);
	Safe_Delete(pSDie);
}

void CNeutralityShader::CreatePathes()
{
	CTransformImporter transformInporter;
	transformInporter.LoadMeshData("Resource/Data/Pathes.txt");
	for (int i = 0, cnt = 0; i < 6; ++i)
	{
		m_pathes[i].push_back(CPathEdge(XMFLOAT2(0, 0), XMFLOAT2(CONVERT_Unit_to_InG(transformInporter.m_Transform[cnt].pos.x), CONVERT_Unit_to_InG(transformInporter.m_Transform[cnt].pos.z))));
		for (int j = 0; j < transformInporter.m_iKindMeshCnt[i] - 1; ++j, ++cnt)
		{
			XMFLOAT3 from = transformInporter.m_Transform[cnt].pos;
			XMFLOAT3 to = transformInporter.m_Transform[cnt + 1].pos;
			m_pathes[i].push_back(CPathEdge(XMFLOAT2(CONVERT_Unit_to_InG(from.x), CONVERT_Unit_to_InG(from.z)), XMFLOAT2(CONVERT_Unit_to_InG(to.x), CONVERT_Unit_to_InG(to.z))));
		}
		++cnt;
	}
}
