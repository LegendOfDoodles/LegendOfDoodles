#include "stdafx.h"
#include "Player.h"

/// <summary>
/// 목적: 플레이어 관리 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  정휘현, 김나단
/// 최종 수정 날짜: 2018-08-01
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CPlayer::CPlayer(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CAnimatedObject(pCreateMgr, nMeshes)
{
	m_detectRange = CONVERT_PaperUnit_to_InG(80.0f);
	m_sightRange = CONVERT_PaperUnit_to_InG(80.0f);
	SetSpeed(CONVERT_cm_to_InG(3.285f * 5));
}


CPlayer::~CPlayer()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CPlayer::Animate(float timeElapsed)
{
	switch (m_curState) {
	case States::Idle:
		if (m_nCurrAnimation != Animations::Idle) m_nCurrAnimation = Animations::Idle;
		if (m_mainPath) SetState(States::Walk);
		break;
	case States::Attack:
		if (m_fFrameTime >= m_nAniLength[m_nAniIndex] - 1)
		{
			SetState(States::Idle);
		}
		else if (GetType() == ObjectType::StickPlayer)
		{
			if (m_nCurrAnimation == Animations::Attack1 &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{

			}
		}
		else if (GetType() == ObjectType::SwordPlayer)
		{
			if (m_nCurrAnimation == Animations::Attack1 &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pSoundMgr->play(SOUND::Player_Sword_Attack_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillQ &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_SwordSkill_Q_Effect);
				m_pSoundMgr->play(SOUND::Player_Sword_Q_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillW &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_SwordSkill_W_Effect);
				m_pSoundMgr->play(SOUND::Player_Sword_W_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillE &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_SwordSkill_E_Effect);
				m_pSoundMgr->play(SOUND::Player_Sword_E_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillR &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.666f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.666f)
			{
				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_SwordSkill_R_Effect);
				m_pSoundMgr->play(SOUND::Player_Sword_R_Sound, GetPosition());
			}
		}
		else if (GetType() == ObjectType::StaffPlayer)
		{
			if (m_nCurrAnimation == Animations::Attack1 &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pSoundMgr->play(SOUND::Player_Staff_Attack_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillQ &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_StaffQSkill_Effect);
				m_pSoundMgr->play(SOUND::Player_Staff_Q_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillW &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_StaffAttack_Effect);
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_StaffSkill_W_Effect);
				m_pSoundMgr->play(SOUND::Player_Staff_W_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillE &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_StaffESkill_Effect);
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_StaffSkill_E_Effect);
				m_pSoundMgr->play(SOUND::Player_Staff_E_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillR &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.666f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.666f)
			{
				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_StaffAttack_Effect);
			}
		}
		else if (GetType() == ObjectType::BowPlayer)
		{
			if (m_nCurrAnimation == Animations::Attack1 &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pSoundMgr->play(SOUND::Player_Arrow_Attack_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillQ &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_ArrowAttack_Effect);
				m_pSoundMgr->play(SOUND::Player_Arrow_Q_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillW &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_ArrowAttack_Effect);
				m_pSoundMgr->play(SOUND::Player_Arrow_W_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillE &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_ArrowAttack_Effect);
				m_pSoundMgr->play(SOUND::Player_Arrow_E_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillR &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_ArrowAttack_Effect);
				m_pSoundMgr->play(SOUND::Player_Arrow_R_Sound, GetPosition());
			}
		}
		m_fPreFrameTime = m_fFrameTime;
		m_fFrameTime += ANIMATION_SPEED * timeElapsed * m_StatusInfo.AtkSpeed;
		break;

	case States::Walk:
		if (m_nCurrAnimation != Animations::StartWalk&&
			m_nCurrAnimation != Animations::Walking)
			m_nCurrAnimation = Animations::StartWalk;

		if (m_nCurrAnimation == Animations::StartWalk) {
			if (m_fFrameTime >= m_nAniLength[m_nAniIndex] - 1)
			{
				m_nCurrAnimation = Animations::Walking;
				m_fFrameTime = 0;
			}
		}
		m_fPreFrameTime = m_fFrameTime;
		m_fFrameTime += ANIMATION_SPEED * timeElapsed * m_StatusInfo.WalkSpeed;
		break;
	case States::Die:
		if (GetAnimTimeRemainRatio() < 0.05)
		{
			SetState(States::Remove);
		}
		break;
	case States::Win:
		if (m_nCurrAnimation != Animations::Win) m_nCurrAnimation = Animations::Win;
		break;
	case States::Defeat:
		if (m_nCurrAnimation != Animations::Defeat&&
			m_nCurrAnimation != Animations::Defeat2)
			m_nCurrAnimation = Animations::Defeat;

		if (m_nCurrAnimation == Animations::Defeat) {
			if (m_fFrameTime >= m_nAniLength[m_nAniIndex] - 1)
			{
				m_nCurrAnimation = Animations::Defeat2;
				m_fFrameTime = 0;
			}
		}
		break;
	case States::Remove:
		break;
	default:
		break;
	}

	if (m_curState != States::Remove &&
		m_curState != States::Walk &&
		m_curState != States::Attack)
	{
		m_fPreFrameTime = m_fFrameTime;
		m_fFrameTime += ANIMATION_SPEED * timeElapsed;
	}

	AdjustAnimationIndex();

	if (m_fFrameTime > m_nAniLength[m_nAniIndex]) {
		while (m_fFrameTime > m_nAniLength[m_nAniIndex])
			m_fFrameTime -= m_nAniLength[m_nAniIndex];
	}

	if (MoveToDestination(timeElapsed) == States::Done) SetState(States::Idle);

	CAnimatedObject::Animate(timeElapsed);
}

void CPlayer::Render(CCamera * pCamera, UINT instanceCnt)
{
	OnPrepareRender();

	if (!IsVisible(pCamera) || !m_Detected) return;

	if (m_pMaterial)
	{
		m_pMaterial->Render(pCamera);
		m_pMaterial->UpdateShaderVariables();
	}

	if (m_cbvGPUDescriptorHandle.ptr)
		m_pCommandList->SetGraphicsRootDescriptorTable(6, m_cbvGPUDescriptorHandle);

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(instanceCnt);
		}
	}
}

void CPlayer::LookAt(XMFLOAT3 objPosition)
{
	if (m_curState == States::Attack) return;
	CAnimatedObject::LookAt(objPosition);
}

void CPlayer::LookAt(XMFLOAT2 objPosition)
{
	if (m_curState == States::Attack) return;
	CAnimatedObject::LookAt(objPosition);
}

void CPlayer::ActiveSkill(AnimationsType act)
{
	if (m_curState != States::Attack) {
		m_curState = States::Attack;
		m_nCurrAnimation = act;
		m_fFrameTime = 0;
		m_skilling = false;
	}
}

void CPlayer::SetState(StatesType newState, shared_ptr<CWayFinder> pWayFinder)
{
	UNREFERENCED_PARAMETER(pWayFinder);
	m_curState = newState;
	
	switch (newState)
	{
	case States::Idle:
		m_nCurrAnimation = Animations::Idle;
		break;
	case States::Walk:
		RegenerateLookAt();
		if (m_nCurrAnimation != Animations::StartWalk &&
			m_nCurrAnimation != Animations::Walking)
			m_nCurrAnimation = Animations::StartWalk;
		break;
	case States::Chase:
		m_nCurrAnimation = Animations::StartWalk;
		break;
	case States::Attack:
		m_fFrameTime = 0;
		break;
	case States::Die:
		m_nCurrAnimation = Animations::Die;
		m_fFrameTime = 0;
		m_StatusInfo.HP = 0.f;
		SetPathToGo(NULL);
		break;
	case States::Remove:
		break;
	case States::Win:
		m_nCurrAnimation = Animations::Win;
		m_fFrameTime = 0;
		SetPathToGo(NULL);
		break;
	case States::Defeat:
		m_nCurrAnimation = Animations::Defeat;
		m_fFrameTime = 0;
		SetPathToGo(NULL);
		break;
	default:
		assert(!"Error:: There is No State");
	}
}

void CPlayer::SetHP(float maxHP, float curHP)
{
	m_StatusInfo.maxHP = maxHP; 
	m_StatusInfo.HP = curHP;

	if (m_StatusInfo.HP <= 0)
	{
		SetState(States::Die);
	}
}

void CPlayer::ChangeSkillSet(CSkeleton ** ppskill)
{
	for (int j = 0; j < 8; j++)
	{
		m_nAniLength[j + 4] = ppskill[j]->GetAnimationLength();
		m_pSkeleton[j + 4] = *ppskill[j];
	}
}

void CPlayer::SaveCurrentState()
{
	m_xmf4x4SpawnWorld = m_xmf4x4World;
	m_spawnLocation = GetPosition();
}

void CPlayer::Respawn()
{
	SetState(StatesType::Idle);

	m_StatusInfo.HP = m_StatusInfo.maxHP;

	m_pColManager->AddCollider(this);

	m_xmf4x4World = m_xmf4x4SpawnWorld;
}

void CPlayer::SetCollisionManager(shared_ptr<CCollisionManager> manager)
{
	m_pColManager = manager;
	m_pColManager->AddPlayerCollider(this);
}

void CPlayer::RequestSpawnMissile(FlyingObjectType type)
{
	m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, type, m_StatusInfo.Atk);
}

void CPlayer::ReceiveSpecial(BYTE idx,  SpecialType type, bool myid)
{
	m_StatusInfo.Special[idx] = type;

	if (myid) m_StatusInfo.SpecialPoint--;

	if (type == SpecialType::AttackSpecial) {
		SetWeaponData(m_StatusInfo.Weapon, idx + 1);
	}
	else if (type == SpecialType::DefenceSpecial) {
		AddEquipCnt(0, idx);
	}
	else if (type == SpecialType::TechnicSpecial) {
		AddEquipCnt(1, idx);
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CPlayer::AdjustAnimationIndex()
{
	switch (m_nCurrAnimation)
	{
	case Animations::Win:
		m_nAniIndex = 0;
		break;
	case Animations::Defeat:
		m_nAniIndex = 1;
		break;
	case Animations::Defeat2:
		m_nAniIndex = 2;
		break;
	case Animations::Die:
		m_nAniIndex = 3;
		break;
	case Animations::Idle:
		m_nAniIndex = 4;
		break;
	case Animations::StartWalk:
		m_nAniIndex = 5;
		break;
	case Animations::Walking:
		m_nAniIndex = 6;
		break;
	case Animations::SkillQ:
		m_nAniIndex = 7;
		break;
	case Animations::SkillW:
		m_nAniIndex = 8;
		break;
	case Animations::SkillE:
		m_nAniIndex = 9;
		break;
	case Animations::SkillR:
		m_nAniIndex = 10;
		break;
	case Animations::Attack1:
		m_nAniIndex = 11;
		break;
	}
}
