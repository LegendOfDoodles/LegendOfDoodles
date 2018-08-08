#include "stdafx.h"
#include "NeutralityShader.h"
#include "05.Objects/10.Neutrality/00.Roider/Roider.h"
#include "05.Objects/10.Neutrality/01.Golem/Golem.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"
#include "00.Global/02.AI/00.FSMMgr/FSMMgr.h"

/// <summary>
/// 목적: 중립 몬스터 관리 및 렌더링용 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-08-03
/// </summary>

//#define Roider_NetralMaterial m_ppMaterials[0]
//#define Roider_BlueMaterial m_ppMaterials[1]
//#define Roider_RedMaterial m_ppMaterials[2]
//#define Golem_NetralMaterial m_ppMaterials[3]
//#define Golem_BlueMaterial m_ppMaterials[4]
//#define Golem_RedMaterial m_ppMaterials[5]

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CNeutralityShader::CNeutralityShader() : CShader()
{
	
}

CNeutralityShader::~CNeutralityShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CNeutralityShader::Initialize(void *pContext)
{
	
	BuildObjects(pContext);
}


void CNeutralityShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_pFSMMgr->Update(timeElapsed, m_ppObjects[j]);
	}
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

////////////////////////////////////////////////////////////////////////
// 내부 함수
//CHECK!
void CNeutralityShader::BuildObjects(void *pContext)
{
	if (pContext) m_pTerrain = (CHeightMapTerrain*)pContext;

	CreatePathes();

	CTransformImporter transformInporter;

	transformInporter.LoadMeshData("Resource//Data//MonsterSetting.txt");

	m_objectsIndices[ObjectType::Roider] = NeutralObjectIndices(0, transformInporter.m_iKindMeshCnt[0]);
	m_objectsIndices[ObjectType::GOLEM] = NeutralObjectIndices(transformInporter.m_iKindMeshCnt[0], transformInporter.m_iKindMeshCnt[0] + 1);

	m_nObjects = transformInporter.m_iKindMeshCnt[0] + 1;
	m_ppObjects = new CCollisionObject*[m_nObjects];

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
