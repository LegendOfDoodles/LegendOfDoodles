#include "stdafx.h"
#include "PlayerShader.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "05.Objects/06.Minion/Minion.h"
#include "05.Objects/08.Player/Player.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"

/// <summary>
/// 목적: 플레이어 관리 및 렌더링 용도
/// 최종 수정자:  김나단
/// 수정자 목록:  정휘현, 김나단
/// 최종 수정 날짜: 2018-10-05
/// </summary>

//////////////////////////////////////////////////////////////////////////////////////////////////
CPlayerShader::CPlayerShader() : CShader()
{
}

CPlayerShader::~CPlayerShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CPlayerShader::Initialize(void *pContext)
{
	BuildObjects(pContext);
}


void CPlayerShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CPlayerShader::SetPlayerAnimation(ObjectType type, int id)
{
	// Sword Player는 기본 세팅 값 사용
	if (type == ObjectType::BowPlayer)
	{
		dynamic_cast<CPlayer*>(m_ppObjects[id])->ChangeSkillSet(m_ppBowAni);
	}
	else if (type == ObjectType::StaffPlayer)
	{
		dynamic_cast<CPlayer*>(m_ppObjects[id])->ChangeSkillSet(m_ppStaffAni);
	}
}

void CPlayerShader::SetColManagerToObject(shared_ptr<CCollisionManager> manager)
{
	for (int i = 0; i < 4; ++i) {
		m_ppObjects[i]->SetCollisionManager(manager);
	}
}

void CPlayerShader::SetThrowingManagerToObject(shared_ptr<CThrowingMgr> manager)
{
	for (int i = 0; i < m_nObjects; ++i)
	{
		m_ppObjects[i]->SetThrowingManager(manager);
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CPlayerShader::BuildObjects(void *pContext)
{
	if (pContext) m_pTerrain = (CHeightMapTerrain*)pContext;

	m_nObjects = 4;
	m_ppObjects = new CCollisionObject*[m_nObjects];

	CSkeleton *pWin = new CSkeleton("Resource//3D//Player//Animation//Player_Win.aniinfo");
	CSkeleton *pDefeat = new CSkeleton("Resource//3D//Player//Animation//Player_Defeat.aniinfo");
	CSkeleton *pDefeat2 = new CSkeleton("Resource//3D//Player//Animation//Player_Defeat2.aniinfo");
	CSkeleton *pDie = new CSkeleton("Resource//3D//Player//Animation//Player_Die.aniinfo");

	m_ppSwordAni = new CSkeleton*[8];

	m_ppSwordAni[0] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Idle.aniinfo");
	m_ppSwordAni[1] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Start_Walk.aniinfo");
	m_ppSwordAni[2] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Walk.aniinfo");
	m_ppSwordAni[3] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Smash.aniinfo");
	m_ppSwordAni[4] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Slash.aniinfo");
	m_ppSwordAni[5] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Dash.aniinfo");
	m_ppSwordAni[6] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Dispute.aniinfo");
	m_ppSwordAni[7] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Attack.aniinfo");

	m_ppStaffAni = new CSkeleton*[8];

	m_ppStaffAni[0] = new CSkeleton("Resource//3D//Player//Animation//Staff//Player_Staff_Idle.aniinfo");
	//임시
	m_ppStaffAni[1] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Start_Walk.aniinfo");
	m_ppStaffAni[2] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Walk.aniinfo");
	///////////////////////////////////////////////////////

	m_ppStaffAni[3] = new CSkeleton("Resource//3D//Player//Animation//Staff//Player_Staff_SkillA.aniinfo");
	m_ppStaffAni[4] = new CSkeleton("Resource//3D//Player//Animation//Staff//Player_Staff_SkillB.aniinfo");
	m_ppStaffAni[5] = new CSkeleton("Resource//3D//Player//Animation//Staff//Player_Staff_SkillC.aniinfo");
	m_ppStaffAni[6] = new CSkeleton("Resource//3D//Player//Animation//Staff//Player_Staff_SkillD.aniinfo");
	m_ppStaffAni[7] = new CSkeleton("Resource//3D//Player//Animation//Staff//Player_Staff_Attack.aniinfo");

	m_ppBowAni = new CSkeleton*[8];

	m_ppBowAni[0] = new CSkeleton("Resource//3D//Player//Animation//Bow//Player_Bow_Idle.aniinfo");
	m_ppBowAni[1] = new CSkeleton("Resource//3D//Player//Animation//Bow//Player_Bow_Start_Walk.aniinfo");
	m_ppBowAni[2] = new CSkeleton("Resource//3D//Player//Animation//Bow//Player_Bow_Walk.aniinfo");
	m_ppBowAni[3] = new CSkeleton("Resource//3D//Player//Animation//Bow//Player_Bow_Attack.aniinfo");

	for (int j = 4; j < 8; ++j) {
		m_ppBowAni[j] = m_ppBowAni[3];
	}

	int i = 0;
	CPlayer *pPlayer{ NULL };

	for (int x = 0; x < m_nObjects / 2; ++x) {
		for (int z = 0; z < m_nObjects / 2; ++z) {
			pPlayer = new CPlayer();

			pPlayer->SetType(ObjectType::StickPlayer);
#if !USE_BATCH_MATERIAL
			pRotatingObject->SetMaterial(pCubeMaterial);
#endif
			pPlayer->SetCollisionSize(CONVERT_PaperUnit_to_InG(5.4f));

			pPlayer->CBaseObject::SetPosition(500.0f + (x * 9000.0f), 0.0f, 2000.0f + (z * 1000.0f));
			if (x == 1) {
				pPlayer->SetTeam(TeamType::Red);
			}
			else
				pPlayer->SetTeam(TeamType::Blue);


			pPlayer->SetSkeleton(pWin);
			pPlayer->SetSkeleton(pDefeat);
			pPlayer->SetSkeleton(pDefeat2);
			pPlayer->SetSkeleton(pDie);

			pPlayer->SetSkeleton(m_ppSwordAni[0]);
			pPlayer->SetSkeleton(m_ppSwordAni[1]);
			pPlayer->SetSkeleton(m_ppSwordAni[2]);

			pPlayer->SetSkeleton(m_ppSwordAni[3]);
			pPlayer->SetSkeleton(m_ppSwordAni[4]);
			pPlayer->SetSkeleton(m_ppSwordAni[5]);
			pPlayer->SetSkeleton(m_ppSwordAni[6]);
			pPlayer->SetSkeleton(m_ppSwordAni[7]);

			pPlayer->SetTerrain(m_pTerrain);

			pPlayer->Rotate(90, 0, 0);
			pPlayer->SetTag(10000 + i);

			pPlayer->SaveCurrentState();
			m_ppObjects[i++] = pPlayer;
		}
	}

}

void CPlayerShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			delete m_ppObjects[j];
		}
		Safe_Delete_Array(m_ppObjects);
	}
	//애니메이션 
	for (UINT j = 0; j < 8; j++)
	{
		delete m_ppSwordAni[j];
	}
	Safe_Delete_Array(m_ppSwordAni);

	for (UINT j = 0; j < 8; j++)
	{
		delete m_ppStaffAni[j];
	}
	Safe_Delete_Array(m_ppStaffAni);

	for (UINT j = 0; j < 4; j++)
	{
		delete m_ppBowAni[j];
	}
	Safe_Delete_Array(m_ppBowAni);
}
