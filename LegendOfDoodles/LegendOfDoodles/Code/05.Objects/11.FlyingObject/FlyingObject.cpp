#include "stdafx.h"
#include "FlyingObject.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"

/// <summary>
/// 목적: 날아가는(스킬, 화살 등) 오브젝트 처리 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-08-12
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CFlyingObject::CFlyingObject(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CCollisionObject(pCreateMgr, nMeshes)
{
	m_ObjectType = ObjectType::FlyingObject;
}

CFlyingObject::~CFlyingObject()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CFlyingObject::Animate(float timeElapsed)
{
	if (!m_Activated) return;

	if (m_flyingObjectType == FlyingObjectType::Roider_Dumbel ||
		m_flyingObjectType == FlyingObjectType::Minion_Arrow ||
		m_flyingObjectType == FlyingObjectType::Player_Arrow ||
		m_flyingObjectType == FlyingObjectType::Player_ArrowSkill_W)
	{
		m_distance += timeElapsed * m_speed;
		MoveToDirection(timeElapsed * m_speed);
		m_pColManager->RequestCollide(CollisionType::SPHERE, this, 0, m_attackRange, m_damage * timeElapsed);

		if (m_SoundTrigier &&
			(m_flyingObjectType == FlyingObjectType::Player_Arrow ||
				m_flyingObjectType == FlyingObjectType::Player_ArrowSkill_W))
		{
			m_pSoundMgr->play(SOUND::Flying_PlayerArrow_Sound, GetPosition());
			m_SoundTrigier = false;
		}


		if (m_flyingObjectType == FlyingObjectType::Minion_Arrow  && m_EffectTriger)
		{
			m_pEffectMgr->RequestSpawn(GetPosition(), m_direction, static_cast<int>(m_maxDistance), EffectObjectType::Flying_MinionArrow_Effect);
			m_EffectTriger = false;
		}

		if (m_distance > m_maxDistance)
		{
			m_curState = StatesType::Remove;
		}
	}
	else if (m_flyingObjectType == FlyingObjectType::Minion_Magic)
	{
		m_distance += timeElapsed * m_speed;
		MoveToDirection(timeElapsed * m_speed);
		m_pColManager->RequestCollide(CollisionType::SPHERE, this, 0, m_attackRange, m_damage * timeElapsed);
		Rotate(0, 0, 3.0f);
		if (m_distance > m_maxDistance)
		{
			m_curState = StatesType::Remove;
		}
	}
	else if (m_flyingObjectType == FlyingObjectType::BlueTower_Attack ||
		m_flyingObjectType == FlyingObjectType::RedTower_Attack)
	{
		MoveToDirection(timeElapsed * m_speed);
		Rotate(0, 0, 10.0f);
		XMFLOAT3 curPos{ GetPosition() };
		if (curPos.y < m_pTerrain->GetHeight(curPos.x, curPos.z))
		{
			if (m_SoundTrigier)
			{
				m_pSoundMgr->play(SOUND::TowerAttack_Explosion_Sound, GetPosition());
				m_SoundTrigier = false;
			}

			m_pEffectMgr->RequestSpawn(GetPosition(), m_direction, 30, EffectObjectType::Tower_Attack_Explosion_Effect);
			m_pColManager->RequestCollide(CollisionType::SPHERE, this, 0, m_attackRange, m_damage);
			m_curState = StatesType::Remove;
		}
	}
	else if (m_flyingObjectType == FlyingObjectType::Player_ArrowSkill_Q ||
		m_flyingObjectType == FlyingObjectType::Player_MagicSkill_Q)
	{
		m_distance += timeElapsed * m_speed;
		MoveToDirection(timeElapsed * m_speed);

		if (m_SoundTrigier && m_flyingObjectType == FlyingObjectType::Player_ArrowSkill_Q)
		{
			m_pSoundMgr->play(SOUND::Flying_PlayerArrow_Sound, GetPosition());
			m_SoundTrigier = false;
		}

		if (m_pColManager->RequestNearObject(this, m_attackRange * 0.5f, m_TeamType) != NULL)
		{
			m_pEffectMgr->RequestSpawn(GetPosition(), m_direction, 10, EffectObjectType::Player_ArrowAndFireBall_HitPosition_Effect);

			m_pColManager->RequestCollide(CollisionType::SPHERE, this, 0, m_attackRange, m_damage);
			m_curState = StatesType::Remove;
		}

		if (m_distance > m_maxDistance)
		{
			m_curState = StatesType::Remove;
		}
	}
	else if (m_flyingObjectType == FlyingObjectType::Player_ArrowSkill_E)
	{
		m_distance += timeElapsed * m_speed;
		MoveToDirection(timeElapsed * m_speed);
		m_pColManager->RequestCollide(CollisionType::SPHERE, this, 0, m_attackRange, m_damage * timeElapsed);

		if (m_SoundTrigier)
		{
			m_pSoundMgr->play(SOUND::Flying_PlayerArrow_Sound, GetPosition());
			m_SoundTrigier = false;
		}

		if (m_EffectTriger && m_distance > m_maxDistance * 0.5f)
		{
			m_direction = Vector3::ScalarProduct(m_direction, -1);
			Rotate(180.0f, 0, 0.0f);
			m_pEffectMgr->RequestSpawn(GetPosition(), m_direction, static_cast<int>(m_maxDistance * 0.5f), EffectObjectType::Flying_PlayerArrow_Effect);
			m_damage *= 2.0f;
			m_EffectTriger = false;
		}

		if (m_distance > m_maxDistance)
		{
			m_curState = StatesType::Remove;
		}
	}
	else if (m_flyingObjectType == FlyingObjectType::Player_ArrowSkill_R)
	{
		m_distance += timeElapsed * m_speed;
		MoveToDirection(timeElapsed * m_speed);
		m_pColManager->RequestCollide(CollisionType::SPHERE, this, 0, m_attackRange, m_damage * timeElapsed);

		if (m_SoundTrigier)
		{
			m_pSoundMgr->play(SOUND::Flying_PlayerRArrow_Sound, GetPosition());
			m_SoundTrigier = false;
		}

		Rotate(0, 55.0f, 0.0f);

		if (m_distance > m_maxDistance)
		{
			m_curState = StatesType::Remove;
		}
	}
	else if (m_flyingObjectType == FlyingObjectType::Player_MagicSkill_R)
	{
		MoveToDirection(timeElapsed * m_speed);
		Rotate(-360.f * timeElapsed, 0.f, 0.0f);
		XMFLOAT3 curPos{ GetPosition() };
		if (curPos.y < m_pTerrain->GetHeight(curPos.x, curPos.z))
		{
			m_pColManager->RequestCollide(CollisionType::SPHERE, this, 0, m_attackRange, m_damage);
			m_pSoundMgr->play(SOUND::Player_Staff_R_Sound, GetPosition());
			m_curState = StatesType::Remove;
		}
	}
	else if (m_flyingObjectType == FlyingObjectType::Player_Magic)
	{
		m_distance += timeElapsed * m_speed;
		MoveToDirection(timeElapsed * m_speed);
		m_pColManager->RequestCollide(CollisionType::SPHERE, this, 0, m_attackRange, m_damage * timeElapsed);
		Rotate(0, 0, 10.0f);
		if (m_distance > m_maxDistance)
		{
			m_curState = StatesType::Remove;
		}
	}
}

void CFlyingObject::SetFlyingObjectsType(FlyingObjectType type)
{
	m_flyingObjectType = type;
	m_curState = StatesType::Idle;

	if (type == FlyingObjectType::Roider_Dumbel)
	{
		m_attackRange = CONVERT_PaperUnit_to_InG(5);
		m_distance = 0.0f;
		m_maxDistance = CONVERT_PaperUnit_to_InG(30);
		m_speed = CONVERT_cm_to_InG(2.805f);
	}
	else if (type == FlyingObjectType::Minion_Arrow)
	{
		m_attackRange = CONVERT_PaperUnit_to_InG(2.5f);
		m_distance = 0.0f;
		m_EffectTriger = true;
		m_maxDistance = CONVERT_PaperUnit_to_InG(30);
		m_speed = MINION_ARROW_SPEED;
	}
	else if (type == FlyingObjectType::Minion_Magic)
	{
		m_attackRange = CONVERT_PaperUnit_to_InG(2.5f);
		m_distance = 0.0f;
		m_maxDistance = CONVERT_PaperUnit_to_InG(24);
		m_speed = CONVERT_cm_to_InG(2.148f);
	}
	else if (type == FlyingObjectType::BlueTower_Attack)
	{
		m_SoundTrigier = true;
		m_attackRange = CONVERT_PaperUnit_to_InG(20);
		m_speed = CONVERT_cm_to_InG(4.733f);
	}
	else if (type == FlyingObjectType::RedTower_Attack)
	{
		m_SoundTrigier = true;
		m_attackRange = CONVERT_PaperUnit_to_InG(20);
		m_speed = CONVERT_cm_to_InG(4.733f);
	}
	else if (type == FlyingObjectType::Player_Arrow)
	{
		m_attackRange = CONVERT_PaperUnit_to_InG(5);
		m_distance = 0.0f;
		m_EffectTriger = true;
		m_SoundTrigier = true;
		m_maxDistance = CONVERT_PaperUnit_to_InG(40);
		m_speed = CONVERT_cm_to_InG(7.22f);
	}
	else if (type == FlyingObjectType::Player_MagicSkill_Q)
	{
		m_attackRange = CONVERT_PaperUnit_to_InG(15);
		m_distance = 0.0f;
		m_maxDistance = CONVERT_PaperUnit_to_InG(36);
		m_speed = CONVERT_cm_to_InG(6.13f);
	}
	else if (type == FlyingObjectType::Player_ArrowSkill_Q)
	{
		m_attackRange = CONVERT_PaperUnit_to_InG(15);
		m_distance = 0.0f;
		m_SoundTrigier = true;
		m_maxDistance = CONVERT_PaperUnit_to_InG(32);
		m_speed = CONVERT_cm_to_InG(5.79f);
	}
	else if (type == FlyingObjectType::Player_ArrowSkill_W)
	{
		m_attackRange = CONVERT_PaperUnit_to_InG(6);
		m_distance = 0.0f;
		m_SoundTrigier = true;
		m_maxDistance = CONVERT_PaperUnit_to_InG(50);
		m_speed = CONVERT_cm_to_InG(3.305f);
	}
	else if (type == FlyingObjectType::Player_ArrowSkill_E)
	{
		m_attackRange = CONVERT_PaperUnit_to_InG(5);
		m_distance = 0.0f;
		m_EffectTriger = true;
		m_maxDistance = CONVERT_PaperUnit_to_InG(80);
		m_speed = PLAYER_ESKILL_ARROW_SPEED
	}
	else if (type == FlyingObjectType::Player_ArrowSkill_R)
	{
		m_attackRange = CONVERT_PaperUnit_to_InG(17);
		m_distance = 0.0f;
		m_SoundTrigier = true;
		m_maxDistance = CONVERT_PaperUnit_to_InG(44);
		m_speed = CONVERT_cm_to_InG(9.05f);
	}
	else if (type == FlyingObjectType::Player_MagicSkill_R)
	{
		m_attackRange = CONVERT_PaperUnit_to_InG(30);
		m_speed = CONVERT_cm_to_InG(15.3f);
		m_direction = XMFLOAT3(0, -1.f, 0);
	}
	else if (type == FlyingObjectType::Player_Magic)
	{
		m_attackRange = CONVERT_PaperUnit_to_InG(5);
		m_distance = 0.0f;
		m_EffectTriger = true;
		m_maxDistance = CONVERT_PaperUnit_to_InG(36);
		m_speed = CONVERT_cm_to_InG(3.52f);
	}
}

void CFlyingObject::SetDirection(const XMFLOAT3 & direction)
{
	m_direction = direction;
	LookAt(Vector3::Add(GetPosition(), Vector3::ScalarProduct(direction, 10000, false)));
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CFlyingObject::LookAt(XMFLOAT3 target)
{
	if (m_curState == States::Attack) return;
	if (m_curState == States::Win) return;
	if (m_curState == States::Defeat) return;

	XMFLOAT3 upVector{ 0.f, 1.f, 0.f };
	XMFLOAT3 objLookVector = GetLook();
	XMFLOAT3 objPos = GetPosition();

	target.y = objPos.y;

	XMFLOAT3 towardVector = Vector3::Subtract(GetPosition(), target, true);

	float angle{ Vector3::DotProduct(towardVector, objLookVector) };
	angle = XMConvertToDegrees(acos(angle));

	if (isnan(angle)) return;

	float check{ Vector3::DotProduct(Vector3::CrossProduct(towardVector, objLookVector), upVector) };

	// 회전 방향 결정
	if (check < 0.0f)
		Rotate(0.0f, angle, 0.0f);
	else if (check > 0.0f)
		Rotate(0.0f, -angle, 0.0f);
}

void CFlyingObject::LookAt(XMFLOAT2 target)
{
	LookAt(XMFLOAT3(target.x, 0, target.y));
}

void CFlyingObject::MoveToDirection(float dist)
{
	CBaseObject::SetPosition(Vector3::Add(GetPosition(), m_direction, dist));
}
