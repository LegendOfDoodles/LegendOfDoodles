#include "stdafx.h"
#include "NexusTowerShader.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"
#include "05.Objects/09.NexusTower/NexusTower.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"


/// <summary>
/// 목적: 스테틱 오브젝트 그리기 용도의 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-05-11
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CNexusTowerShader::CNexusTowerShader(): CShader()
{
}

CNexusTowerShader::~CNexusTowerShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CNexusTowerShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

bool CNexusTowerShader::OnProcessKeyInput(UCHAR* pKeyBuffer)
{
	if (GetAsyncKeyState('U') & 0x0001)
	{
		((CCollisionObject*)m_ppObjects[0])->SetState(States::Die);

	}
	return true;
}

void CNexusTowerShader::SetColManagerToObject(CCollisionManager * manager)
{
	for (int i = 0; i < m_nObjects; ++i) {

		dynamic_cast<CCollisionObject*>(m_ppObjects[i])->SetCollisionManager(manager);
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CNexusTowerShader::BuildObjects(void *pContext)
{
	CTransformImporter transformInporter;

	transformInporter.LoadMeshData("Resource/Data/NexusTowerSetting.txt");

	m_nObjects = transformInporter.m_iTotalCnt;
	m_ppObjects = new CBaseObject*[m_nObjects];

	CNexusTower *pBuild{ NULL };
	
	int cnt = 0;
	for (int i = 0; i < 4; ++i) {
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
				pBuild->type = ObjectType::Nexus;
			}
			else {
				pBuild->SetCollisionSize(CONVERT_PaperUnit_to_InG(8));
				pBuild->type = ObjectType::FirstTower;
			}
			pBuild->Rotate(0, 180, 0);
			pBuild->Rotate(-rot.x, rot.y, -rot.z);
			pBuild->ResetCollisionLevel();
			pBuild->SetStatic(StaticType::Static);

			m_ppObjects[cnt++] = pBuild;
		}
	}
}

void CNexusTowerShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			delete m_ppObjects[j];
		}
		Safe_Delete_Array(m_ppObjects);
	}
}
