#include "stdafx.h"
#include "PlayerShader.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "05.Objects/06.Minion/Minion.h"
#include "05.Objects/08.Player/Player.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
CPlayerShader::CPlayerShader() : CShader()
{
}

CPlayerShader::~CPlayerShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CPlayerShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

bool CPlayerShader::OnProcessKeyInput(UCHAR* pKeyBuffer)
{
	static float R = 0.0f;
	static float M = 0.0f;

	// Warning 스킬 처리
	if (GetAsyncKeyState('Q') & 0x0001)
	{
		//dynamic_cast<CPlayer*>(m_ppObjects[m_pNetwork->m_myid])->ActiveSkill(Animations::SkillQ);
	}
	if (GetAsyncKeyState('E') & 0x0001)
	{
		//dynamic_cast<CPlayer*>(m_ppObjects[m_pNetwork->m_myid])->ActiveSkill(Animations::SkillE);
	}
	if (GetAsyncKeyState('R') & 0x0001)
	{
		//dynamic_cast<CPlayer*>(m_ppObjects[m_pNetwork->m_myid])->ActiveSkill(Animations::SkillR);
	}

	return true;
}

void CPlayerShader::SetColManagerToObject(CCollisionManager * manager)
{
	for (int i = 0; i < 4; ++i) 
	{
		dynamic_cast<CCollisionObject*>(m_ppObjects[i])->SetCollisionManager(manager);
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CPlayerShader::BuildObjects(void *pContext)
{
	if (pContext) m_pTerrain = (CHeightMapTerrain*)pContext;

	m_nObjects = 4;
	m_ppObjects = new CBaseObject*[m_nObjects];
	
	CSkeleton *pWin = new CSkeleton("Resource//3D//Player//Animation//Player_Win.aniinfo");
	CSkeleton *pDefeat = new CSkeleton("Resource//3D//Player//Animation//Player_Defeat.aniinfo");
	CSkeleton *pDefeat2 = new CSkeleton("Resource//3D//Player//Animation//Player_Defeat2.aniinfo");

	CSkeleton *pSIdle = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Idle.aniinfo");
	CSkeleton *pSStartWalk = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Start_Walk.aniinfo");
	CSkeleton *pSWalk = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Walk.aniinfo");
	CSkeleton *pSSlash = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Slash.aniinfo");
	CSkeleton *pSSmash = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Smash.aniinfo");
	CSkeleton *pSDispute = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Dispute.aniinfo");


	int i = 0;
	CPlayer *pPlayer{ NULL };

	for (int x = 0; x < m_nObjects / 2; ++x) {
		for (int z = 0; z < m_nObjects / 2; ++z) {
			pPlayer = new CPlayer();

			pPlayer->SetType(ObjectType::StickPlayer);

			pPlayer->SetCollisionSize(CONVERT_PaperUnit_to_InG(3));

			pPlayer->CBaseObject::SetPosition(500+(z*9000), 0, 2000+(x*1000));

			if (z == 1)
				pPlayer->SetTeam(TeamType::Red);
			else
				pPlayer->SetTeam(TeamType::Blue);

			pPlayer->SetSkeleton(pSIdle);
			pPlayer->SetSkeleton(pSStartWalk);
			pPlayer->SetSkeleton(pSWalk);

			pPlayer->SetSkeleton(pSSmash);
			pPlayer->SetSkeleton(pSSlash);
			pPlayer->SetSkeleton(pSDispute);
			
			pPlayer->SetSkeleton(pWin);
			pPlayer->SetSkeleton(pDefeat);
			pPlayer->SetSkeleton(pDefeat2);

			pPlayer->SetTerrain(m_pTerrain);

			m_ppObjects[i++] = pPlayer;
		}
	}

	Safe_Delete(pSIdle);
	Safe_Delete(pSSlash);
	Safe_Delete(pSSmash);
	Safe_Delete(pSDispute);
	Safe_Delete(pSStartWalk);
	Safe_Delete(pSWalk);
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
}