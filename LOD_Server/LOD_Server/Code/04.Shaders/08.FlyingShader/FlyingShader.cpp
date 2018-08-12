#include "stdafx.h"
#include "FlyingShader.h"
#include "05.Objects/11.FlyingObject/FlyingObject.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"

/// <summary>
/// 목적: 날아다니는(화살 등) 오브젝트 그리기 용도의 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-08-07
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
	m_dumbelList.remove_if(removeFunc);
	m_arrowList.remove_if(removeFunc);
	m_magicList.remove_if(removeFunc);
}

void CFlyingShader::SpawnFlyingObject(const XMFLOAT3& position, const XMFLOAT3& direction, TeamType teamType, FlyingObjectType objectType)
{
	int idx{ GetPossibleIndex(objectType) };

	if (idx != NONE)
	{
		m_ppObjects[idx]->ResetWorldMatrix();
		m_ppObjects[idx]->SaveIndex(idx);
		m_ppObjects[idx]->SetTeam(teamType);
		m_ppObjects[idx]->SetFlyingObjectsType(objectType);
		m_ppObjects[idx]->SetDirection(direction);
		m_ppObjects[idx]->ResetCollisionLevel();
		m_ppObjects[idx]->Activate();
		int adjIdx{ idx - m_objectsIndices[objectType].m_begIndex };
		UNREFERENCED_PARAMETER(adjIdx);
		if (objectType == FlyingObjectType::Roider_Dumbel)
		{
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(8), position.z));
			//m_ppObjects[idx]->SetMesh(0, m_pMeshes[0]);
			//m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (m_srvIncrementSize * adjIdx));
			m_ppObjects[idx]->Rotate(0, RandInRange(0.0f, 360.0f), 0);
			m_dumbelList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::Minion_Arrow)
		{
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(4), position.z));
			//m_ppObjects[idx]->SetMesh(0, m_pMeshes[1]);
			//m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[1].ptr + (m_srvIncrementSize * adjIdx));
			m_arrowList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::Minion_Magic)
		{
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(4), position.z));
			//m_ppObjects[idx]->SetMesh(0, m_pMeshes[2]);
			//m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[2].ptr + (m_srvIncrementSize * adjIdx));
			m_magicList.emplace_back(m_ppObjects[idx]);
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
	UNREFERENCED_PARAMETER(pContext);

	CTransformImporter monsterTransformImporter;
	monsterTransformImporter.LoadMeshData("Resource//Data//MonsterSetting.txt");

	// 오브젝트 순서 설정
	FlyingObjectType objectOrder[]{
		FlyingObjectType::Roider_Dumbel,
		FlyingObjectType::Minion_Arrow,
		FlyingObjectType::Minion_Magic
	};

	// 각 오브젝트의 최대 개수 설정
	m_nObjects += m_objectsMaxCount[FlyingObjectType::Roider_Dumbel] = monsterTransformImporter.m_iKindMeshCnt[0];
	m_nObjects += m_objectsMaxCount[FlyingObjectType::Minion_Arrow] = MAX_ARROW;
	m_nObjects += m_objectsMaxCount[FlyingObjectType::Minion_Magic] = MAX_MAGIC;

	// 각 오브젝트 개수 만큼 Possible Index 생성
	m_objectsPossibleIndices = std::unique_ptr<bool[]>(new bool[m_nObjects]);
	
	// 설정된 Possible Indices를 0(false)로 초기화
	memset(m_objectsPossibleIndices.get(), false, m_nObjects * sizeof(bool));

	m_ppObjects = new CCollisionObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	UNREFERENCED_PARAMETER(ncbElementBytes);
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

		m_ppObjects[j] = pObject;
	}
}

void CFlyingShader::ReleaseObjects()
{
	if (!m_dumbelList.empty()) m_dumbelList.clear();
	if (!m_arrowList.empty()) m_arrowList.clear();
	if (!m_magicList.empty()) m_magicList.clear();

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