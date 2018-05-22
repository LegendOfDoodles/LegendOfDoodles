#include "stdafx.h"
#include "Player.h"
#include "06.Meshes/01.Mesh/Mesh.h"

/// <summary>
/// ����: �÷��̾� ���� Ŭ����
/// ���� ������:  �質��
/// ������ ���:  ������, �質��
/// ���� ���� ��¥: 2018-05-22
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CPlayer::CPlayer(CCreateMgr *pCreateMgr, int nMeshes) : CAnimatedObject(pCreateMgr, nMeshes)
{
}


CPlayer::~CPlayer()
{

}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CPlayer::Animate(float timeElapsed)
{
	m_fFrameTime += 30 * timeElapsed;

	if (m_fFrameTime > m_nAniLength[m_nAniIndex]) {
		while (m_fFrameTime > m_nAniLength[m_nAniIndex])
			m_fFrameTime -= m_nAniLength[m_nAniIndex];
	}

	CAnimatedObject::Animate(timeElapsed);
}

void CPlayer::Render(CCamera * pCamera, UINT instanceCnt)
{
	OnPrepareRender();

	if (!IsVisible(pCamera)) return;

	if (m_pMaterial)
	{
		m_pMaterial->Render(pCamera);
		m_pMaterial->UpdateShaderVariables();
	}

	if (m_cbvGPUDescriptorHandle.ptr)
		m_pCommandList->SetGraphicsRootDescriptorTable(6, m_cbvGPUDescriptorHandle);

	if (m_pShader)
	{
		UpdateShaderVariables();
		m_pShader->Render(pCamera);
	}

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(instanceCnt);
		}
	}
}

void CPlayer::SetWeapon(int weapon)
{
	if (m_WeaponState != weapon)
	{
		m_WeaponState = weapon;
		if (weapon == 0) {
			SetType(ObjectType::StickPlayer);
			SetMesh(1, m_pStickMesh);
		}
		else {
			SetType(ObjectType::SwordPlayer);
			SetMesh(1, m_ppSwordMesh[weapon-1]);

		}
	}
}
////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CPlayer::AdjustAnimationIndex()
{
	switch (m_nCurrAnimation)
	{
	case Animations::Idle:
		m_nAniIndex = 0;
		break;
	case Animations::StartWalk:
		m_nAniIndex = 1;
		break;
	case Animations::Walking:
		m_nAniIndex = 2;
		break;
	case Animations::SkillQ:
		m_nAniIndex = 3;
		break;
	case Animations::SkillE:
		m_nAniIndex = 4;
		break;
	case Animations::SkillR:
		m_nAniIndex = 5;
		break;

	case Animations::Win:
		m_nAniIndex = 6;
		break;
	case Animations::Defeat:
		m_nAniIndex = 7;
		break;
	case Animations::Defeat2:
		m_nAniIndex = 8;
		break;

	}
}
