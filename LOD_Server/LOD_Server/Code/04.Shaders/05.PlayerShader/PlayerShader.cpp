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
/// 최종 수정 날짜: 2018-07-03
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

void CPlayerShader::SetColManagerToObject(shared_ptr<CCollisionManager> manager)
{
	for (int i = 0; i < 4; ++i) {
		m_ppObjects[i]->SetCollisionManager(manager);
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
//CHECK!
void CPlayerShader::BuildObjects(void *pContext)
{
	if (pContext) m_pTerrain = (CHeightMapTerrain*)pContext;

	m_nObjects = 4;
	m_ppObjects = new CCollisionObject*[m_nObjects];

	/*CSkinnedMesh *pPlayerMesh = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Player.meshinfo");
	m_pStick = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Player_Stick.meshinfo");

	CCubeMesh *pBoundingBoxMesh = new CCubeMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(2.0f), CONVERT_PaperUnit_to_InG(1.0f), CONVERT_PaperUnit_to_InG(10.0f),
		0, 0, -CONVERT_PaperUnit_to_InG(6.5f));
	m_nArmor = 1;
	m_pArmor = new CSkinnedMesh*[m_nArmor];
	m_pArmor[0] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Armor//extra//Muffler.meshinfo");


	m_nSword = 3;

	m_pSword = new CSkinnedMesh*[m_nSword];
	m_pSword[0] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Sword//Player_Sword_Basic.meshinfo");
	m_pSword[1] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Sword//Player_Sword2.meshinfo");
	m_pSword[2] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Sword//Player_Sword3.meshinfo");


	m_nBow = 3;

	m_pBow = new CSkinnedMesh*[m_nBow];
	m_pBow[0] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Bow//Player_Bow_Basic.meshinfo");
	m_pBow[1] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Bow//Player_Bow_Flight.meshinfo");
	m_pBow[2] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Bow//Player_Bow_Battle.meshinfo");

	m_nStaff = 3;

	m_pStaff = new CSkinnedMesh*[m_nStaff];
	m_pStaff[0] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Staff//Player_Staff_Basic.meshinfo");
	m_pStaff[1] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Staff//Player_Staff_Lolipop.meshinfo");
	m_pStaff[2] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Staff//Player_Staff_Watch.meshinfo");*/

	CSkeleton *pWin = new CSkeleton("Resource//3D//Player//Animation//Player_Win.aniinfo");
	CSkeleton *pDefeat = new CSkeleton("Resource//3D//Player//Animation//Player_Defeat.aniinfo");
	CSkeleton *pDefeat2 = new CSkeleton("Resource//3D//Player//Animation//Player_Defeat2.aniinfo");

	m_ppSwordAni = new CSkeleton*[7];

	m_ppSwordAni[0] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Idle.aniinfo");
	m_ppSwordAni[1] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Start_Walk.aniinfo");
	m_ppSwordAni[2] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Walk.aniinfo");
	m_ppSwordAni[3] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Smash.aniinfo");
	m_ppSwordAni[4] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Slash.aniinfo");
	m_ppSwordAni[5] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Dash.aniinfo");
	m_ppSwordAni[6] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Dispute.aniinfo");

	m_ppStaffAni = new CSkeleton*[7];

	m_ppStaffAni[0] = new CSkeleton("Resource//3D//Player//Animation//Staff//Player_Staff_Idle.aniinfo");
	//임시
	m_ppStaffAni[1] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Start_Walk.aniinfo");
	m_ppStaffAni[2] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Walk.aniinfo");
	///////////////////////////////////////////////////////

	m_ppStaffAni[3] = new CSkeleton("Resource//3D//Player//Animation//Staff//Player_Staff_SkillA.aniinfo");
	m_ppStaffAni[4] = new CSkeleton("Resource//3D//Player//Animation//Staff//Player_Staff_SkillB.aniinfo");
	m_ppStaffAni[5] = new CSkeleton("Resource//3D//Player//Animation//Staff//Player_Staff_SkillC.aniinfo");
	m_ppStaffAni[6] = new CSkeleton("Resource//3D//Player//Animation//Staff//Player_Staff_SkillD.aniinfo");

	m_ppBowAni = new CSkeleton*[7];

	m_ppBowAni[0] = new CSkeleton("Resource//3D//Player//Animation//Bow//Player_Bow_Idle.aniinfo");
	m_ppBowAni[1] = new CSkeleton("Resource//3D//Player//Animation//Bow//Player_Bow_Start_Walk.aniinfo");
	m_ppBowAni[2] = new CSkeleton("Resource//3D//Player//Animation//Bow//Player_Bow_Walk.aniinfo");
	m_ppBowAni[3] = new CSkeleton("Resource//3D//Player//Animation//Bow//Player_Bow_Attack.aniinfo");

	for (int j = 4; j < 7; ++j) {
		m_ppBowAni[j] = m_ppBowAni[3];
	}

	pPlayerMesh->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(6.5f)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(1.0f), CONVERT_PaperUnit_to_InG(1.0f), CONVERT_PaperUnit_to_InG(5.0f)));

	int i = 0;
	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CPlayer *pPlayer = NULL;


	m_pStick->AddRef();
	for (UINT j = 0; j < m_nSword; ++j) {
		m_pSword[j]->AddRef();
	}
	for (UINT j = 0; j < m_nStaff; ++j) {
		m_pStaff[j]->AddRef();
	}
	for (UINT j = 0; j < m_nBow; ++j) {
		m_pBow[j]->AddRef();
	}
	m_pArmor[0]->AddRef();

	for (int x = 0; x < m_nObjects / 2; ++x) {
		for (int z = 0; z < m_nObjects / 2; ++z) {

			pPlayer = new CPlayer(pCreateMgr, 3);

			pPlayer->SetMesh(0, pPlayerMesh);

			pPlayer->SetMesh(1, m_pStick);
			pPlayer->SetMesh(2, m_pArmor[0]);
			pPlayer->SetType(ObjectType::StickPlayer);
#if !USE_BATCH_MATERIAL
			pRotatingObject->SetMaterial(pCubeMaterial);
#endif
			pPlayer->SetBoundingMesh(pBoundingBoxMesh);
			pPlayer->SetCollisionSize(CONVERT_PaperUnit_to_InG(3));
			if (x == 0 && z == 0) {
				pPlayer->tag = 1;
			}
			pPlayer->CBaseObject::SetPosition(500.0f + (z * 9000.0f), 0.0f, 2000.0f + (x * 1000.0f));
			if (z == 1) {
				pPlayer->SetTeam(TeamType::Red);
			}
			else
				pPlayer->SetTeam(TeamType::Blue);


			pPlayer->SetSkeleton(pWin);
			pPlayer->SetSkeleton(pDefeat);
			pPlayer->SetSkeleton(pDefeat2);

			pPlayer->SetSkeleton(m_ppSwordAni[0]);
			pPlayer->SetSkeleton(m_ppSwordAni[1]);
			pPlayer->SetSkeleton(m_ppSwordAni[2]);

			pPlayer->SetSkeleton(m_ppSwordAni[3]);
			pPlayer->SetSkeleton(m_ppSwordAni[4]);
			pPlayer->SetSkeleton(m_ppSwordAni[5]);
			pPlayer->SetSkeleton(m_ppSwordAni[6]);


			pPlayer->SetTerrain(m_pTerrain);

			pPlayer->Rotate(90, 0, 0);

			pPlayer->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * i));
			pPlayer->SetCbvGPUDescriptorHandlePtrForBB(m_pcbvGPUDescriptorStartHandle[1].ptr + (incrementSize * i));
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
	for (UINT j = 0; j < 7; j++)
	{
		delete m_ppSwordAni[j];
	}
	Safe_Delete_Array(m_ppSwordAni);

	for (UINT j = 0; j < 7; j++)
	{
		delete m_ppStaffAni[j];
	}
	Safe_Delete_Array(m_ppStaffAni);

	for (UINT j = 0; j < 4; j++)
	{
		delete m_ppBowAni[j];
	}
	Safe_Delete_Array(m_ppBowAni);
	//////////////////////////////////////
	//메쉬

	for (UINT j = 0; j < m_nSword; j++)
	{
		delete m_pSword[j];
	}
	Safe_Delete_Array(m_pSword);


	for (UINT j = 0; j < m_nStaff; j++)
	{
		delete m_pStaff[j];
	}
	Safe_Delete_Array(m_pStaff);


	for (UINT j = 0; j < m_nBow; j++)
	{
		delete m_pBow[j];
	}
	Safe_Delete_Array(m_pBow);


	for (UINT j = 0; j < m_nArmor; j++)
	{
		delete m_pArmor[j];
	}
	Safe_Delete_Array(m_pArmor);

}