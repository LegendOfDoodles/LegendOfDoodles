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
/// 최종 수정 날짜: 2018-09-11
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

void CNexusTowerShader::UpdateActiveTowerStatus()
{
	for (int i = 0; i < m_nObjects; ++i)
	{
		StatesType curState{ m_ppObjects[i]->GetState() };
		if(curState != States::Die && curState != States::Remove)
			m_ppObjects[i]->UpdateTowerStatus();
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

	CCollisionObject* pBlueNexus{ NULL }, *pRedNexus{ NULL };
	CCollisionObject* pBlueUpTower{ NULL }, *pBlueDownTower{ NULL };
	CCollisionObject* pRedUpTower{ NULL }, *pRedDownTower{ NULL };
	float blueUpNear{ FLT_MAX }, blueDownNear{ FLT_MAX };
	float redUpNear{ 0.f }, redDownNear{ 0.f };

	int cnt = 0;
	for (int i = 0; i < 4; ++i) {
		m_meshCounts[i] = transformInporter.m_iKindMeshCnt[i];
		for (int j = 0; j < transformInporter.m_iKindMeshCnt[i]; ++j) {
			XMFLOAT3 pos = transformInporter.m_Transform[cnt].pos;
			XMFLOAT3 rot = transformInporter.m_Transform[cnt].rotation;
			pBuild = new CNexusTower();
			pBuild->SetPosition(CONVERT_Unit_to_InG(pos.x), CONVERT_Unit_to_InG(pos.y), CONVERT_Unit_to_InG(pos.z));

			if (i < 2) {
				pBuild->SetCollisionSize(CONVERT_PaperUnit_to_InG(40));
				pBuild->SetType(ObjectType::Nexus);
				if (i == 0 || i == 2)
				{
					pBuild->SetTeam(TeamType::Blue);
					pBlueNexus = pBuild;
				}
				else {
					pBuild->SetTeam(TeamType::Red);
					pRedNexus = pBuild;
				}
			}
			else {
				pBuild->SetCollisionSize(CONVERT_PaperUnit_to_InG(8));
				pBuild->SetType(ObjectType::FirstTower);
				if (i == 0 || i == 2)
				{
					pBuild->SetTeam(TeamType::Blue);
					if (CONVERT_Unit_to_InG(pos.z) > TERRAIN_SIZE_HEIGHT * 0.5f)
					{
						if (CONVERT_Unit_to_InG(pos.x) < blueUpNear)
						{
							blueUpNear = CONVERT_Unit_to_InG(pos.x);
							pBlueUpTower = pBuild;
						}
					}
					else
					{
						if (CONVERT_Unit_to_InG(pos.x) < blueDownNear)
						{
							blueDownNear = CONVERT_Unit_to_InG(pos.x);
							pBlueDownTower = pBuild;
						}
					}
				}
				else {
					pBuild->SetTeam(TeamType::Red);
					if (CONVERT_Unit_to_InG(pos.z) > TERRAIN_SIZE_HEIGHT * 0.5f)
					{
						if (CONVERT_Unit_to_InG(pos.x) > redUpNear)
						{
							redUpNear = CONVERT_Unit_to_InG(pos.x);
							pRedUpTower = pBuild;
						}
					}
					else
					{
						if (CONVERT_Unit_to_InG(pos.x) > redDownNear)
						{
							redDownNear = CONVERT_Unit_to_InG(pos.x);
							pRedDownTower = pBuild;
						}
					}
				}
			}
			pBuild->Rotate(0, 180, 0);
			pBuild->Rotate(-rot.x, rot.y, -rot.z);

			pBuild->ResetCollisionLevel();
			pBuild->SetStatic(StaticType::Static);
			pBuild->SetTag(30000 + cnt);

			m_ppObjects[cnt++] = pBuild;
		}
	}

	pBlueUpTower->SetMaster(pBlueNexus);
	pBlueDownTower->SetMaster(pBlueNexus);

	pRedUpTower->SetMaster(pRedNexus);
	pRedDownTower->SetMaster(pRedNexus);
}
