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
void CNexusTowerShader::BuildObjects(void *pContext)
{
	UNREFERENCED_PARAMETER(pContext);

	CTransformImporter transformInporter;

	transformInporter.LoadMeshData("Resource//Data//NexusTowerSetting.txt");

	m_nObjects = transformInporter.m_iTotalCnt;
	m_ppObjects = new CCollisionObject*[m_nObjects];

	CNexusTower *pBuild{ NULL };

	m_nNexus = transformInporter.m_iKindMeshCnt[0] + transformInporter.m_iKindMeshCnt[1];
	m_nTower = transformInporter.m_iKindMeshCnt[2] + transformInporter.m_iKindMeshCnt[3];

	int cnt = 0;
	for (int i = 0; i < 4; ++i) {
		m_meshCounts[i] = transformInporter.m_iKindMeshCnt[i];
		for (int j = 0; j < transformInporter.m_iKindMeshCnt[i]; ++j) {
			XMFLOAT3 pos = transformInporter.m_Transform[cnt].pos;
			XMFLOAT3 rot = transformInporter.m_Transform[cnt].rotation;
			pBuild = new CNexusTower();
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

			pBuild->ResetCollisionLevel();
			pBuild->SetStatic(StaticType::Static);

			m_ppObjects[cnt++] = pBuild;
		}
	}
}
