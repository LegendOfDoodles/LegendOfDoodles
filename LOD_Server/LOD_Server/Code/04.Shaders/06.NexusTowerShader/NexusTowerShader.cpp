#include "stdafx.h"
#include "NexusTowerShader.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"
#include "05.Objects/09.NexusTower/NexusTower.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"
#include "00.Global/02.AI/00.FSMMgr/FSMMgr.h"

/// <summary>
/// 목적: 넥서스 및 타워 그리기 용도의 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-08-04
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CNexusTowerShader::CNexusTowerShader()
	: CShader()
{
}

CNexusTowerShader::~CNexusTowerShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CNexusTowerShader::Initialize(void *pContext)
{
	
	BuildObjects(pContext);
}



void CNexusTowerShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_pFSMMgr->Update(timeElapsed, m_ppObjects[j]);
	}
}


void CNexusTowerShader::SetColManagerToObject(shared_ptr<CCollisionManager> manager)
{
	for (int i = 0; i < m_nObjects; ++i) {

		m_ppObjects[i]->SetCollisionManager(manager);
	}
}

void CNexusTowerShader::SetThrowingManagerToObject(shared_ptr<CThrowingMgr> manager)
{
	for (int i = 0; i < m_nObjects; ++i)
	{
		m_ppObjects[i]->SetThrowingManager(manager);
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
//CHECK!
void CNexusTowerShader::BuildObjects(void *pContext)
{
	UNREFERENCED_PARAMETER(pContext);

	CTransformImporter transformInporter;

	transformInporter.LoadMeshData("Resource//Data//NexusTowerSetting.txt");

	m_nObjects = transformInporter.m_iTotalCnt;
	m_ppObjects = new CCollisionObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	int accCnt{ 0 };

	/*for (int i = 0; i < m_nHeaps - 1; ++i)
	{
		CreateCbvAndSrvDescriptorHeaps(pCreateMgr, transformInporter.m_iKindMeshCnt[i], 1, i);
		CreateConstantBufferViews(pCreateMgr, transformInporter.m_iKindMeshCnt[i], m_pConstBuffer.Get(), ncbElementBytes, accCnt, i);
		accCnt += transformInporter.m_iKindMeshCnt[i];
	}
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 0, m_nHeaps - 1);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pBoundingBoxBuffer.Get(), ncbElementBytes, 0, m_nHeaps - 1);

	SaveBoundingBoxHeapNumber(m_nHeaps - 1);*/

//#if USE_BATCH_MATERIAL
//	m_nMaterials = m_nHeaps - 1;
//	m_ppMaterials = new CMaterial*[m_nMaterials];
//	m_ppMaterials[0] = Materials::CreateTresureBoxMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
//	m_ppMaterials[1] = Materials::CreateShellMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);
//	m_ppMaterials[2] = Materials::CreateRoundSoapDispenserMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[2], &m_psrvGPUDescriptorStartHandle[2]);
//	m_ppMaterials[3] = Materials::CreateSquareSoapDispenserMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[3], &m_psrvGPUDescriptorStartHandle[3]);
//#else
//	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
//#endif

	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CStaticMesh *pMeshes[4];
	pMeshes[0] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//NexusTower//Treasure Box Nexus(UV).meshinfo");
	pMeshes[0]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(16)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(25), CONVERT_PaperUnit_to_InG(18), CONVERT_PaperUnit_to_InG(16)));

	pMeshes[1] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//NexusTower//Shell Nexus (UV).meshinfo");
	pMeshes[1]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(10)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(18), CONVERT_PaperUnit_to_InG(18), CONVERT_PaperUnit_to_InG(10)));

	pMeshes[2] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//NexusTower//Circle Soap Dispenser (UV).meshinfo");
	pMeshes[2]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(10)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(5), CONVERT_PaperUnit_to_InG(7), CONVERT_PaperUnit_to_InG(10)));

	pMeshes[3] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//NexusTower//Square Soap Dispenser ver.2 (UV).meshinfo");
	pMeshes[3]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(10)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(5), CONVERT_PaperUnit_to_InG(7), CONVERT_PaperUnit_to_InG(10)));
	CNexusTower *pBuild = NULL;

	m_nNexus = transformInporter.m_iKindMeshCnt[0] + transformInporter.m_iKindMeshCnt[1];
	m_nTower = transformInporter.m_iKindMeshCnt[2] + transformInporter.m_iKindMeshCnt[3];

	int cnt = 0;
	/*for (int i = 0; i < m_nMaterials; ++i) {
		m_meshCounts[i] = transformInporter.m_iKindMeshCnt[i];
		for (int j = 0; j < transformInporter.m_iKindMeshCnt[i]; ++j) {
			XMFLOAT3 pos = transformInporter.m_Transform[cnt].pos;
			XMFLOAT3 rot = transformInporter.m_Transform[cnt].rotation;
			pBuild = new CNexusTower(pCreateMgr);
			pBuild->SetPosition(CONVERT_Unit_to_InG(pos.x), CONVERT_Unit_to_InG(pos.y), CONVERT_Unit_to_InG(pos.z));
			if (i == 0 || i == 2)
			{
				pBuild->SetTeam(TeamType::Blue);
			}
			else {
				pBuild->SetTeam(TeamType::Red);
			}
			if (i < 2) {
				pBuild->SetCollisionSize(CONVERT_PaperUnit_to_InG(40));
				pBuild->SetType(ObjectType::Nexus);
			}
			else {
				pBuild->SetCollisionSize(CONVERT_PaperUnit_to_InG(8));
				pBuild->SetType(ObjectType::FirstTower);

			}
			pBuild->Rotate(0, 180, 0);
			pBuild->Rotate(-rot.x, rot.y, -rot.z);
			pBuild->SetMesh(0, pMeshes[i]);
			SetBoundingBoxMeshByIndex(pCreateMgr, pBuild, i);

			pBuild->ResetCollisionLevel();
			pBuild->SetStatic(StaticType::Static);

			pBuild->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[i].ptr + (incrementSize * j));
			pBuild->SetCbvGPUDescriptorHandlePtrForBB(m_pcbvGPUDescriptorStartHandle[m_nHeaps - 1].ptr + (incrementSize * cnt));
			m_ppObjects[cnt++] = pBuild;
		}
	}*/
}
//
//void CNexusTowerShader::SetBoundingBoxMeshByIndex( CBaseObject * target, int index)
//{
//	static CCubeMesh towerBBMesh(pCreateMgr,
//		CONVERT_PaperUnit_to_InG(10), CONVERT_PaperUnit_to_InG(7), CONVERT_PaperUnit_to_InG(20),
//		0, 0, -CONVERT_PaperUnit_to_InG(10));
//	towerBBMesh.AddRef();
//
//	switch (index)
//	{
//	case 0: // 보물 상자
//		target->SetBoundingMesh(new CCubeMesh(pCreateMgr,
//			CONVERT_PaperUnit_to_InG(50), CONVERT_PaperUnit_to_InG(18), CONVERT_PaperUnit_to_InG(32),
//			0, 0, -CONVERT_PaperUnit_to_InG(16)));
//		break;
//	case 1: // 진주 조개
//		target->SetBoundingMesh(new CCubeMesh(pCreateMgr,
//			CONVERT_PaperUnit_to_InG(36), CONVERT_PaperUnit_to_InG(18), CONVERT_PaperUnit_to_InG(20),
//			0, 0, -CONVERT_PaperUnit_to_InG(10)));
//		break;
//	case 2: // 동글 비누 타워
//	case 3: // 네모 비누 타워
//		target->SetBoundingMesh(&towerBBMesh);
//		break;
//	}
//}
