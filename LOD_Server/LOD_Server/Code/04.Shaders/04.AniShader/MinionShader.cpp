#include "stdafx.h"
#include "MinionShader.h"
#include "05.Objects/06.Minion/Minion.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"
//#include "05.Objects/99.Material/Material.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"
#include "00.Global/01.Utility/07.ThrowingManager/ThrowingMgr.h"
//#include "00.Global/01.Utility/06.HPGaugeManager/HPGaugeManager.h"
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
CMinionShader::CMinionShader()// : CShader()
{
	
}

CMinionShader::~CMinionShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CMinionShader::Initialize(void *pContext)
{
	
	BuildObjects(pContext);
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
////////////////////////////////////////////////////////////////////////
// 내부 함수

void CMinionShader::BuildObjects(void *pContext)
{
	if (pContext) m_pTerrain = (CHeightMapTerrain*)pContext;

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
//CHECK!
void CMinionShader::SpawnMinion()
{
	static bool dataPrepared{ false };
	//
	static CSkinnedMesh swordMinionMesh("Resource//3D//Minion//Mesh//Sword Minion.meshinfo");
	static CSkinnedMesh bowMinionMesh(m_pCreateMgr, "Resource//3D//Minion//Mesh//Bow Minion.meshinfo");
	static CSkinnedMesh staffMinionMesh(m_pCreateMgr, "Resource//3D//Minion//Mesh//Magic Minion.meshinfo");
	//*static UINT incrementSize{ m_pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	//*static CCubeMesh boundingBoxMesh(m_pCreateMgr,
		//*CONVERT_PaperUnit_to_InG(3.0f), CONVERT_PaperUnit_to_InG(1.5f), CONVERT_PaperUnit_to_InG(7.0f),
		//*0, 0, -CONVERT_PaperUnit_to_InG(4.0f));
	//*boundingBoxMesh.AddRef();

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
