#include "stdafx.h"
#include "MinionShader.h"
#include "05.Objects/06.Minion/Minion.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"
#include "00.Global/01.Utility/07.ThrowingManager/ThrowingMgr.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"
#include "00.Global/02.AI/00.FSMMgr/FSMMgr.h"
#include "07.Server/Main.h"

/// <summary>
/// 목적: 미니언 관리 및 그리기 용도
/// 최종 수정자:  김나단
/// 수정자 목록:  정휘현, 김나단
/// 최종 수정 날짜: 2018-10-02
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CMinionShader::CMinionShader()// : CShader()
{
	g_SwordMinionStat.HP = g_SwordMinionStat.maxHP = 425;
	g_SwordMinionStat.Atk = 12;
	g_SwordMinionStat.Def = 0;
	g_SwordMinionStat.Exp = 64;

	g_StaffMinionStat.HP = g_StaffMinionStat.maxHP = 280;
	g_StaffMinionStat.Atk = 23;
	g_StaffMinionStat.Def = 0;
	g_StaffMinionStat.Exp = 64;

	g_BowMinionStat.HP = g_BowMinionStat.maxHP = 300;
	g_BowMinionStat.Atk = 18;
	g_BowMinionStat.Def = 0;
	g_BowMinionStat.Exp = 64;
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
	auto removeFunc = [this](CCollisionObject* obj) {
		if (obj->GetState() == StatesType::Remove)
		{
			ResetPossibleIndex(obj->GetIndex());
			delete obj;
			return true;
		}
		return false;
	};

	m_spawnTime += timeElapsed;

	if (!m_neverSpawn && m_bSpawning)
	{
		if ((m_spawnTime - m_preSpawnTime) >= 0.25f)
		{
			for (float time = m_spawnTime - m_preSpawnTime; time >= 0.25f; time -= 0.25f)
			{
				int curTag{ m_minionId };
				SpawnMinion();
				for (int i = 0; i < MAX_USER; ++i)
				{
					if (g_clients[i].m_isconnected) {
						SC_Msg_Spawn_Minion p;
						p.Minion_Species = (BYTE)m_kind;
						p.Minion_Tag = (short)curTag;
						p.size = sizeof(p);
						p.type = SC_MINION_SPAWN;
						SendPacket(i, &p);
					}
				}
			}
			m_preSpawnTime = m_spawnTime;
		}
		if (m_curSpawnCount >= 5)
		{
			m_bSpawning = false;
			//m_neverSpawn = true;
		}
	}

	if (m_spawnTime >= 30.0f)
	{
		m_spawnTime -= 30.0f;
		m_preSpawnTime = 0.0f;
		m_curSpawnCount = 0;
		if (g_GameTime >= 60.0f)
		{
			m_bSpawning = true;
		}
	}

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

void CMinionShader::UpdateActiveMinionStatus()
{
	g_SwordMinionStat.maxHP += INCREASE_SWORD_MINION_HP;
	g_SwordMinionStat.Atk += INCREASE_SWORD_MINION_ATK;
	g_SwordMinionStat.Def += INCREASE_SWORD_MINION_DEF;
	g_SwordMinionStat.Exp += INCREASE_MINION_EXP;

	g_StaffMinionStat.maxHP += INCREASE_STAFF_MINION_HP;
	g_StaffMinionStat.Atk += INCREASE_STAFF_MINION_ATK;
	g_StaffMinionStat.Def += INCREASE_STAFF_MINION_DEF;
	g_StaffMinionStat.Exp += INCREASE_MINION_EXP;

	g_BowMinionStat.maxHP += INCREASE_BOW_MINION_HP;
	g_BowMinionStat.Atk += INCREASE_BOW_MINION_ATK;
	g_BowMinionStat.Def += INCREASE_BOW_MINION_DEF;
	g_BowMinionStat.Exp += INCREASE_MINION_EXP;

	for (auto iter = m_blueBowMinions.begin(); iter != m_blueBowMinions.end(); ++iter) {
		(*iter)->SetCommonStatus(&g_BowMinionStat);
	}
	for (auto iter = m_blueSwordMinions.begin(); iter != m_blueSwordMinions.end(); ++iter) {
		(*iter)->SetCommonStatus(&g_SwordMinionStat);
	}
	for (auto iter = m_blueStaffMinions.begin(); iter != m_blueStaffMinions.end(); ++iter) {
		(*iter)->SetCommonStatus(&g_StaffMinionStat);
	}
	for (auto iter = m_redBowMinions.begin(); iter != m_redBowMinions.end(); ++iter) {
		(*iter)->SetCommonStatus(&g_BowMinionStat);
	}
	for (auto iter = m_redSwordMinions.begin(); iter != m_redSwordMinions.end(); ++iter) {
		(*iter)->SetCommonStatus(&g_SwordMinionStat);
	}
	for (auto iter = m_redStaffMinions.begin(); iter != m_redStaffMinions.end(); ++iter) {
		(*iter)->SetCommonStatus(&g_StaffMinionStat);
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

void CMinionShader::SpawnMinion()
{
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

	if (!m_bDataPrepared)
	{
		m_bDataPrepared = true;
		return;
	}

	int wayKind{ 0 };

	//m_kind = ObjectType::BowMinion;

	// 전체 생성되는 수의 절반은 근접 미니언
	if (m_curSpawnCount < 10)
	{
		m_kind = ObjectType::SwordMinion;
	}
	// 나머지 절반 마법 / 원기리 미니언
	else if (m_curSpawnCount < 15)
	{
		m_kind = ObjectType::StaffMinion;
	}
	else
	{
		m_kind = ObjectType::BowMinion;
	}

	CollisionObjectList objectAdder;
	for (; wayKind < 4; ++wayKind)
	{
		int index{ GetPossibleIndex() };

		if (index == NONE) break;
		CMinion *pMinionObject{ NULL };

		switch (m_kind)
		{
		case ObjectType::SwordMinion:
			pMinionObject = new CSwordMinion();
			break;
		case ObjectType::StaffMinion:
			pMinionObject = new CMagicMinion();
			pMinionObject->SetThrowingManager(m_pThrowingMgr);
			break;
		case ObjectType::BowMinion:
			pMinionObject = new CBowMinion();
			pMinionObject->SetThrowingManager(m_pThrowingMgr);
			break;
		}

		pMinionObject->SetTag(m_minionId++);
		pMinionObject->SetCollisionSize(CONVERT_PaperUnit_to_InG(3.6f));

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

		XMFLOAT2 firstPos{ m_pathes[wayKind].front().From() };
		pMinionObject->CBaseObject::SetPosition(XMFLOAT3(firstPos.x, 0, firstPos.y));

		pMinionObject->SetCollisionManager(m_pColManager);

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
		pMinionObject->SetPathToGo(new Path(m_pathes[wayKind]));

		objectAdder.emplace_back(pMinionObject);
	}

	for (auto& d : objectAdder)
	{
		m_pColManager->AddCollider(d);
	}

	// 현재 웨이브에서 미니언이 생성된 개수
	++m_curSpawnCount;

	if (m_minionId >= 1000) m_minionId = 0;
}
