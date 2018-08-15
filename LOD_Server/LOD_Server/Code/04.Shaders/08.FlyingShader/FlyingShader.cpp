#include "stdafx.h"
#include "FlyingShader.h"
#include "05.Objects/11.FlyingObject/FlyingObject.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"

/// <summary>
/// 목적: 날아다니는(화살 등) 오브젝트 그리기 용도의 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-08-15
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CFlyingShader::CFlyingShader()
	: CShader()
{
}

CFlyingShader::~CFlyingShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CFlyingShader::AnimateObjects(float timeElapsed)
{
	// 리스트에서 제거할 조건 함수
	static auto removeFunc = [this](CCollisionObject* obj) {
		if (obj->GetState() == StatesType::Remove)
		{
			ResetPossibleIndex(obj->GetIndex());
			obj->Deactivate();
			return true;
		}
		return false;
	};

	if (m_Paused) return;
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}

	// 더 이상 업데이트 하면 안되는 오브젝트 리스트에서 제거
	m_spawnList.remove_if(removeFunc);
}

void CFlyingShader::SpawnFlyingObject(const XMFLOAT3& position, const XMFLOAT3& direction, TeamType teamType, FlyingObjectType objectType, float damage)
{
	FlyingObjectType adjObjectType{ objectType };

	if (objectType == FlyingObjectType::Player_ArrowSkill_Q ||
		objectType == FlyingObjectType::Player_ArrowSkill_W ||
		objectType == FlyingObjectType::Player_ArrowSkill_E)
	{
		adjObjectType = FlyingObjectType::Player_Arrow;
	}

	int idx{ GetPossibleIndex(adjObjectType) };

	if (idx != NONE)
	{
		m_ppObjects[idx]->ResetWorldMatrix();
		m_ppObjects[idx]->SaveIndex(idx);
		m_ppObjects[idx]->SetTeam(teamType);
		m_ppObjects[idx]->SetDirection(direction);
		m_ppObjects[idx]->SetFlyingObjectsType(objectType);
		m_ppObjects[idx]->SetDamage(damage);
		m_ppObjects[idx]->ResetCollisionLevel();
		m_ppObjects[idx]->Activate();
		int adjIdx{ idx - m_objectsIndices[adjObjectType].m_begIndex };
		if (objectType == FlyingObjectType::Roider_Dumbel)
		{
			m_ppObjects[idx]->SetPosition(Vector3::Add(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(8), position.z), Vector3::ScalarProduct(direction, CONVERT_PaperUnit_to_InG(4), false)));
			m_ppObjects[idx]->Rotate(0, RandInRange(0.0f, 360.0f), 0);
			m_spawnList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::Minion_Arrow)
		{
			m_ppObjects[idx]->SetPosition(Vector3::Add(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(5), position.z), Vector3::ScalarProduct(direction, CONVERT_PaperUnit_to_InG(2), false)));
			m_spawnList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::Minion_Magic)
		{
			m_ppObjects[idx]->SetPosition(Vector3::Add(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(5), position.z), Vector3::ScalarProduct(direction, CONVERT_PaperUnit_to_InG(2), false)));
			m_spawnList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::BlueTower_Attack)
		{
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y, position.z));
			m_spawnList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::RedTower_Attack)
		{
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y, position.z));
			m_spawnList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::Player_Arrow ||
			objectType == FlyingObjectType::Player_ArrowSkill_Q ||
			objectType == FlyingObjectType::Player_ArrowSkill_E)
		{
			m_ppObjects[idx]->SetPosition(Vector3::Add(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(7), position.z), Vector3::ScalarProduct(direction, CONVERT_PaperUnit_to_InG(3), false)));
			m_spawnList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::Player_MagicSkill_Q)
		{
			m_ppObjects[idx]->SetPosition(Vector3::Add(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(7), position.z), Vector3::ScalarProduct(direction, CONVERT_PaperUnit_to_InG(3), false)));
			m_spawnList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::Player_ArrowSkill_W)
		{
			m_ppObjects[idx]->SetPosition(Vector3::Add(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(7), position.z), Vector3::ScalarProduct(direction, CONVERT_PaperUnit_to_InG(4), false)));
			m_spawnList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::Player_ArrowSkill_R)
		{
			m_ppObjects[idx]->SetPosition(Vector3::Add(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(7), position.z), Vector3::ScalarProduct(direction, CONVERT_PaperUnit_to_InG(3), false)));
			m_spawnList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::Player_MagicSkill_R)
		{
			m_ppObjects[idx]->SetPosition(Vector3::Add(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(100), position.z), Vector3::ScalarProduct(direction, CONVERT_PaperUnit_to_InG(13), false)));
			m_spawnList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::Player_Magic)
		{
			m_ppObjects[idx]->SetPosition(Vector3::Add(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(7), position.z), Vector3::ScalarProduct(direction, CONVERT_PaperUnit_to_InG(3), false)));
			m_spawnList.emplace_back(m_ppObjects[idx]);
		}
	}
}

void CFlyingShader::SetColManagerToObject(shared_ptr<CCollisionManager> manager)
{
	for (int i = 0; i < m_nObjects; ++i) {
		m_ppObjects[i]->SetCollisionManager(manager);
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CFlyingShader::BuildObjects(void *pContext)
{
	if (pContext) m_pTerrain = (CHeightMapTerrain*)pContext;

	CTransformImporter monsterTransformImporter;
	monsterTransformImporter.LoadMeshData("Resource//Data//MonsterSetting.txt");

	// 오브젝트 순서 설정
	FlyingObjectType objectOrder[]{
		FlyingObjectType::Roider_Dumbel,
		FlyingObjectType::Minion_Arrow,
		FlyingObjectType::Minion_Magic,
		FlyingObjectType::BlueTower_Attack,
		FlyingObjectType::RedTower_Attack,
		FlyingObjectType::Player_Arrow,
		FlyingObjectType::Player_MagicSkill_Q,
		FlyingObjectType::Player_ArrowSkill_R,
		FlyingObjectType::Player_MagicSkill_R,
		FlyingObjectType::Player_Magic
	};

	// 각 오브젝트의 최대 개수 설정
	m_nObjects += m_objectsMaxCount[FlyingObjectType::Roider_Dumbel] = monsterTransformImporter.m_iKindMeshCnt[0];
	m_nObjects += m_objectsMaxCount[FlyingObjectType::Minion_Arrow] = MAX_ARROW;
	m_nObjects += m_objectsMaxCount[FlyingObjectType::Minion_Magic] = MAX_MAGIC;
	m_nObjects += m_objectsMaxCount[FlyingObjectType::BlueTower_Attack] = MAX_EACH_TOWER_ATK;
	m_nObjects += m_objectsMaxCount[FlyingObjectType::RedTower_Attack] = MAX_EACH_TOWER_ATK;
	m_nObjects += m_objectsMaxCount[FlyingObjectType::Player_Arrow] = MAX_PLAYER_ARROW_ATK + MAX_PLAYER_SKILL + MAX_PLAYER_SKILL + MAX_PLAYER_SKILL;	// 화살 기본 공격 + Q스킬 + W스킬 + E스킬
	m_nObjects += m_objectsMaxCount[FlyingObjectType::Player_MagicSkill_Q] = MAX_PLAYER_MAGIC_ATK;
	m_nObjects += m_objectsMaxCount[FlyingObjectType::Player_ArrowSkill_R] = MAX_PLAYER_SKILL;
	m_nObjects += m_objectsMaxCount[FlyingObjectType::Player_MagicSkill_R] = MAX_PLAYER_SKILL;
	m_nObjects += m_objectsMaxCount[FlyingObjectType::Player_Magic] = MAX_PLAYER_MAGIC_ATK;

	// 각 오브젝트 개수 만큼 Possible Index 생성
	m_objectsPossibleIndices = std::unique_ptr<bool[]>(new bool[m_nObjects]);
	
	// 설정된 Possible Indices를 0(false)로 초기화
	memset(m_objectsPossibleIndices.get(), false, m_nObjects * sizeof(bool));

	m_ppObjects = new CCollisionObject*[m_nObjects];

	int accCnt{ 0 };

	for (int i = 0; i < m_nMesh; ++i)
	{
		m_objectsIndices[objectOrder[i]] = FlyingObjectIndices();
		m_objectsIndices[objectOrder[i]].m_begIndex = accCnt;
		accCnt += m_objectsMaxCount[objectOrder[i]];
		m_objectsIndices[objectOrder[i]].m_endIndex = accCnt;
	}

	// 오브젝트 생성
	CFlyingObject *pObject{ NULL };
	for (int j = 0; j < m_nObjects; ++j) {
		pObject = new CFlyingObject();

		pObject->SetTeam(TeamType::None);

		pObject->SetStatic(StaticType::Move);

		pObject->SetTerrainImage(m_pTerrain);

		m_ppObjects[j] = pObject;
	}
}

void CFlyingShader::ReleaseObjects()
{
	if (!m_spawnList.empty()) m_spawnList.clear();

	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			delete m_ppObjects[j];
		}
		Safe_Delete_Array(m_ppObjects);
	}
}

int CFlyingShader::GetPossibleIndex(FlyingObjectType type)
{
	for (int idx = m_objectsIndices[type].m_begIndex; idx < m_objectsIndices[type].m_endIndex; ++idx)
	{
		if (!m_objectsPossibleIndices[idx])
		{
			m_objectsPossibleIndices[idx] = true;
			return idx;
		}
	}
	return NONE;
}