#include "stdafx.h"
#include "AniShader.h"
#include "05.Objects/06.Minion/Minion.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"
#include "00.Global/02.AI/00.FSMMgr/FSMMgr.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"

/// <summary>
/// 목적: 움직이는 오브젝트 관리 및 그리기 용도
/// 최종 수정자:  김나단
/// 수정자 목록:  정휘현, 김나단
/// 최종 수정 날짜: 2018-05-11
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CAniShader::CAniShader() : CShader()
{
}

CAniShader::~CAniShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CAniShader::AnimateObjects(float timeElapsed)
{
	if (m_pColManager->IsGameEnded()) return;

	m_spawnTime += timeElapsed;

	if (m_spawnTime >= 0.0f && m_spawnTime <= 5.0f)
	{
		bool spawned{ false };
		for (float time = m_spawnTime - m_preSpawnTime; time >= 0.25; time -= 0.25)
		{
			spawned = true;
			SpawnMinion();
		}
		if(spawned) m_preSpawnTime = m_spawnTime;
	}

	if (m_spawnTime >= 30.0f)
	{
		m_spawnTime -= 30.0f;
		m_preSpawnTime = -0.25f;
	}

	for (auto& iter = m_blueObjects.begin(); iter != m_blueObjects.end();)
	{
		if ((*iter)->GetState() == States::Remove)
		{
			CCollisionObject* temp{ *iter };
			ResetPossibleIndex(temp->GetIndex());
			Safe_Delete(temp);

			iter = m_blueObjects.erase(iter);
		}
		else
		{
			m_pFSMMgr->Update(timeElapsed, (*iter));
			++iter;
		}
	}

	for (auto& iter = m_redObjects.begin(); iter != m_redObjects.end();)
	{
		if ((*iter)->GetState() == States::Remove)
		{
			CCollisionObject* temp{ *iter };
			ResetPossibleIndex(temp->GetIndex());
			Safe_Delete(temp);

			iter = m_redObjects.erase(iter);
		}
		else
		{
			m_pFSMMgr->Update(timeElapsed, (*iter));
			++iter;
		}
	}
}

bool CAniShader::OnProcessKeyInput(UCHAR* pKeyBuffer)
{
	if (GetAsyncKeyState('N') & 0x0001)
	{
		SpawnMinion();
	}

	return true;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CAniShader::BuildObjects(void *pContext)
{
	if (pContext) m_pTerrain = (CHeightMapTerrain*)pContext;

	CreatePathes();
	SpawnMinion();
}

void CAniShader::ReleaseObjects()
{
	for (auto& iter = m_blueObjects.begin(); iter != m_blueObjects.end();)
	{
		iter = m_blueObjects.erase(iter);
	}
	m_blueObjects.clear();
	for (auto& iter = m_redObjects.begin(); iter != m_redObjects.end(); )
	{
		iter = m_redObjects.erase(iter);
	}
	m_redObjects.clear();
}

void CAniShader::CreatePathes()
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

int CAniShader::GetPossibleIndex()
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

void CAniShader::SpawnMinion()
{
	static bool dataPrepared{ false };

	static CSkeleton SIdle("Resource//3D//Minion//Animation//Sword//Minion_S_Idle.aniinfo");
	static CSkeleton SAtk1("Resource//3D//Minion//Animation//Sword//Minion_S_Attack1.aniinfo");
	static CSkeleton SAtk2("Resource//3D//Minion//Animation//Sword//Minion_S_Attack2.aniinfo");
	static CSkeleton SWalkStart("Resource//3D//Minion//Animation//Sword//Minion_S_WalkStart.aniinfo");
	static CSkeleton SWalk("Resource//3D//Minion//Animation//Sword//Minion_S_Walk.aniinfo");

	static CSkeleton BIdle("Resource//3D//Minion//Animation//Bow//Minion_B_Idle.aniinfo");
	static CSkeleton BAtk("Resource//3D//Minion//Animation//Bow//Minion_B_Attack.aniinfo");
	static CSkeleton BWalkStart("Resource//3D//Minion//Animation//Bow//Minion_B_WalkStart.aniinfo");
	static CSkeleton BWalk("Resource//3D//Minion//Animation//Bow//Minion_B_Walk.aniinfo");

	static CSkeleton MIdle("Resource//3D//Minion//Animation//Magic//Minion_M_Idle.aniinfo");
	static CSkeleton MAtk1("Resource//3D//Minion//Animation//Magic//Minion_M_Attack1.aniinfo");
	static CSkeleton MAtk2("Resource//3D//Minion//Animation//Magic//Minion_M_Attack2.aniinfo");
	static CSkeleton MWalkStart("Resource//3D//Minion//Animation//Magic//Minion_M_WalkStart.aniinfo");
	static CSkeleton MWalk("Resource//3D//Minion//Animation//Magic//Minion_M_Walk.aniinfo");

	static CSkeleton Die("Resource//3D//Minion//Animation//Minion_Die.aniinfo");

	if (!dataPrepared)
	{
		dataPrepared = true;
		return;
	}

	int kind{ 0 };
	int makeCnt{ 0 };
	for (; kind < 4; ++kind)
	{
		int index = GetPossibleIndex();

		if (index == NONE) break;
		CMinion *pMinionObject{ NULL };

		switch (m_kind)
		{
		case ObjectType::SwordMinion:
			pMinionObject = new CSwordMinion();
			break;
		case ObjectType::StaffMinion:
			pMinionObject = new CMagicMinion();
			break;
		case ObjectType::BowMinion:
			pMinionObject = new CBowMinion();
			break;
		}

		pMinionObject->SetCollisionSize(CONVERT_PaperUnit_to_InG(2));

		switch (m_kind)
		{
		case ObjectType::SwordMinion:
			pMinionObject->SetSkeleton(&SIdle);
			pMinionObject->SetSkeleton(&SAtk1);
			pMinionObject->SetSkeleton(&SAtk2);
			pMinionObject->SetSkeleton(&SWalkStart);
			pMinionObject->SetSkeleton(&SWalk);
			break;
		case ObjectType::StaffMinion:
			pMinionObject->SetSkeleton(&MIdle);
			pMinionObject->SetSkeleton(&MAtk1);
			pMinionObject->SetSkeleton(&MAtk2);
			pMinionObject->SetSkeleton(&MWalkStart);
			pMinionObject->SetSkeleton(&MWalk);
			break;
		case ObjectType::BowMinion:
			pMinionObject->SetSkeleton(&BIdle);
			pMinionObject->SetSkeleton(&BAtk);
			pMinionObject->SetSkeleton(&BWalkStart);
			pMinionObject->SetSkeleton(&BWalk);
			break;
		}

		pMinionObject->SetSkeleton(&Die);
		pMinionObject->SetTerrain(m_pTerrain);

		pMinionObject->Rotate(90, 0, 0);

		pMinionObject->SaveIndex(index);

		pMinionObject->SetPathToGo(new Path(m_pathes[kind]));

		XMFLOAT2 firstPos{ m_pathes[kind].front().From() };
		pMinionObject->CBaseObject::SetPosition(XMFLOAT3(firstPos.x, 0, firstPos.y));

		pMinionObject->SetCollisionManager(m_pColManager);

		if (kind == Minion_Species::Blue_Up || kind == Minion_Species::Blue_Down)
		{
			pMinionObject->SetTeam(TeamType::Blue);
			m_blueObjects.emplace_back(pMinionObject);
		}
		else if (kind == Minion_Species::Red_Up || kind == Minion_Species::Red_Down)
		{
			pMinionObject->SetTeam(TeamType::Red);
			m_redObjects.emplace_back(pMinionObject);
		}

		makeCnt++;
	}

	if (!makeCnt) return;
	
	CollisionObjectList::reverse_iterator &blueBegin{ m_blueObjects.rbegin() };
	CollisionObjectList::reverse_iterator &redBegin{ m_redObjects.rbegin() };

	if (makeCnt > 1) blueBegin++;
	if (makeCnt > 3) redBegin++;

	for (kind = 0; makeCnt > 0; --makeCnt, ++kind)
	{
		if (kind == Minion_Species::Blue_Up || kind == Minion_Species::Blue_Down) {
			m_pColManager->AddCollider((*blueBegin));
			if(blueBegin != m_blueObjects.rbegin()) --blueBegin;
		}
		else if (kind == Minion_Species::Red_Up || kind == Minion_Species::Red_Down) {
			m_pColManager->AddCollider((*redBegin));
			if (redBegin != m_redObjects.rbegin()) --redBegin;
		}
	}
}
