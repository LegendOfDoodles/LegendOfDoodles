#include "stdafx.h"
#include "AnimatedObject.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"

/// <summary>
/// 목적: 움직이는 오브젝트 처리용 기본 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  정휘현, 김나단
/// 최종 수정 날짜: 2018-09-10
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CAnimatedObject::CAnimatedObject() : CCollisionObject()
{
}

CAnimatedObject::~CAnimatedObject()
{
	if (m_mainPath) Safe_Delete(m_mainPath);
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CAnimatedObject::Animate(float timeElapsed)
{
	UNREFERENCED_PARAMETER(timeElapsed);

	ResetCollisionLevel();

	int Bcnt = m_pSkeleton[m_nAniIndex].GetBoneCount();

	for (int i = 0; i < Bcnt; ++i) {
		m_xmf4x4Frame[i] = m_pSkeleton[m_nAniIndex].GetBone(static_cast<float>(i)).GetFrame(static_cast<int>(m_fFrameTime));
	}
}


void CAnimatedObject::SetSkeleton(CSkeleton *skeleton)
{
	m_nAniLength[m_nAniCnt] = skeleton->GetAnimationLength();
	m_pSkeleton[m_nAniCnt++] = *skeleton;
}

void CAnimatedObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CBaseObject::SetPosition(xmf3Position);
}

void CAnimatedObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CBaseObject::SetPosition(xmf3Position);
}

void CAnimatedObject::LookAt(XMFLOAT3 objPosition)
{
	if (m_curState == States::Win) return;
	if (m_curState == States::Defeat) return;

	XMFLOAT3 upVector{ 0.f, 1.f, 0.f };
	XMFLOAT3 playerLook = GetLook();
	XMFLOAT3 playerPos = GetPosition();

	objPosition.y = playerPos.y;

	XMFLOAT3 towardVector = Vector3::Subtract(objPosition, GetPosition(), true);

	float angle{ Vector3::DotProduct(towardVector, playerLook) };
	angle = XMConvertToDegrees(acos(angle));

	if (isnan(angle)) return;

	float check{ Vector3::DotProduct(Vector3::CrossProduct(towardVector, playerLook), upVector) };

	// 캐릭터가 선택된 오브젝트 보다 오른쪽 보고 있는 경우
	if (check < 0.0f)
		Rotate(0.0f, 0.0f, -angle);
	else if (check > 0.0f)
		Rotate(0.0f, 0.0f, angle);
}

void CAnimatedObject::LookAt(XMFLOAT2 objPosition)
{
	LookAt(XMFLOAT3(objPosition.x, 0, objPosition.y));
}

XMFLOAT3 CAnimatedObject::GetLook()
{
	return(Vector3::ScalarProduct(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23), -1));
}

XMFLOAT3 CAnimatedObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

void CAnimatedObject::SetPosition(float x, float z)
{
	CBaseObject::SetPosition(x, m_pTerrain->GetHeight(x, z), z);
}

void CAnimatedObject::SetPathToGo(Path * path)
{
	if (m_mainPath)
	{
		m_mainPath->clear();
		Safe_Delete(m_mainPath);
	}
	m_mainPath = path;
	ResetDestination();
}

ProcessType CAnimatedObject::MoveToDestination(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	if (m_curState != States::Walk) return States::Processing;
	if (!m_mainPath) return States::Done;

	if (NoneDestination() || IsArrive(m_speed * timeElapsed))	//  도착 한 경우
	{
		if (m_mainPath->empty())
		{
			Safe_Delete(m_mainPath);
			ResetDestination();
			return States::Done;
		}
		else
		{
			m_destination = m_mainPath->front().To();
			m_mainPath->pop_front();
			LookAt(m_destination);
		}
	}
	else  // 아직 도착하지 않은 경우
	{
		MoveForward(m_speed * timeElapsed);
		XMFLOAT3 position = GetPosition();
		position.y = m_pTerrain->GetHeight(position.x, position.z);
		CBaseObject::SetPosition(position);
		CheckRightWay(PathType::Main, pWayFinder);
	}
	return States::Processing;
}

void CAnimatedObject::MoveToSubDestination(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	if (pWayFinder != NULL)
	{
		m_availableTime -= timeElapsed;
		if (m_availableTime <= 0.0f && m_pEnemy)
		{
			XMFLOAT3 myPos{ GetPosition() };
			XMFLOAT3 enemyPos{ m_pEnemy->GetPosition() };
			m_availableTime = TIME_REFRESH_SUB_PATH;
			ResetSubPath();
			m_subPath = pWayFinder->GetPathToPosition(
				myPos,
				enemyPos);
			if (m_subPath)
			{
				m_subPath->push_back(CPathEdge(XMFLOAT2(enemyPos.x, enemyPos.z), Vector3::ToVector2(Vector3::Add(enemyPos, Vector3::Subtract(enemyPos, myPos)))));
			}
		}
	}

	if (NoneDestination(PathType::Sub) || IsArrive(m_speed * timeElapsed, PathType::Sub))	//  도착 한 경우
	{
		if (m_subPath == NULL || m_subPath->empty())
		{
			Safe_Delete(m_subPath);
			ResetDestination(PathType::Sub);
			LookAt(m_destination);
		}
		else
		{
			m_subDestination = m_subPath->front().To();
			m_subPath->pop_front();
			LookAt(m_subDestination);
		}
	}
	MoveForward(m_speed * timeElapsed);
	XMFLOAT3 position = GetPosition();
	position.y = m_pTerrain->GetHeight(position.x, position.z);
	CBaseObject::SetPosition(position);
	CheckRightWay(PathType::Sub, pWayFinder);
}

void CAnimatedObject::GenerateSubPathToMainPath(shared_ptr<CWayFinder> pWayFinder)
{
	ResetSubPath();
	if (NoneDestination(PathType::Main))
	{
		if (m_mainPath && !m_mainPath->empty())
		{
			m_destination = m_mainPath->front().To();
			m_mainPath->pop_front();
		}
	}
	m_subPath = pWayFinder->GetPathToPosition(
		GetPosition(),
		m_destination);
	if (m_subPath)
	{
		m_subDestination = m_subPath->front().To();
		m_subPath->pop_front();
		LookAt(m_subDestination);
	}
}

void CAnimatedObject::GenerateSubPathToPosition(shared_ptr<CWayFinder> pWayFinder, XMFLOAT3 position)
{
	ResetSubPath();
	m_subPath = pWayFinder->GetPathToPosition(
		GetPosition(),
		position);
	if (m_subPath)
	{
		m_subDestination = m_subPath->front().To();
		m_subPath->pop_front();
		LookAt(m_subDestination);
	}
}

void CAnimatedObject::RegenerateLookAt()
{
	if (NoneDestination(PathType::Sub))
	{
		if (NoneDestination()) return;

		LookAt(m_destination);
	}
}

bool CAnimatedObject::Attackable(CCollisionObject * other)
{
	if (!CheckEnemyState(other)) return false;
	float dstSqr = Vector3::DistanceSquare(GetPosition(), other->GetPosition());
	// 공격 범위의 70% 안에 적이 있는지 확인 -> 여러 적을 공격하기 위함
	float adjRange = m_attackRange * 0.7f;
	return (dstSqr < adjRange * adjRange);
}

// 원거리 확인 함수
bool CAnimatedObject::AttackableFarRange(CCollisionObject * other)
{
	if (!CheckEnemyState(other)) return false;
	float dstSqr = Vector3::DistanceSquare(GetPosition(), other->GetPosition());
	// 공격 범위의 70% 안에 적이 있는지 확인 -> 여러 적을 공격하기 위함
	float adjRange = m_farAttackRange * 0.7f;
	return (dstSqr < adjRange * adjRange);
}

bool CAnimatedObject::Chaseable(CCollisionObject * other)
{
	if (!CheckEnemyState(other)) return false;
	float dstSqr = Vector3::DistanceSquare(GetPosition(), other->GetPosition());
	return (dstSqr < m_detectRange * m_detectRange);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
bool CAnimatedObject::IsArrive(float dst, PathType type)
{
	XMFLOAT2 curPos{ GetPosition().x, GetPosition().z };
	if (type == PathType::Main)
	{
		int distanceSqr = static_cast<int>(Vector2::DistanceSquare(curPos, m_destination));
		// 정확히 도착 하는 경우
		if (distanceSqr < dst * dst) return true;
		if (m_mainPath && !m_mainPath->empty())
		{
			XMFLOAT2 next = m_mainPath->front().To();
			XMFLOAT2 dstToNext = Vector2::Subtract(next, m_destination, true);
			float dstToNextLengthSqr = Vector2::DistanceSquare(next, m_destination);
			float curPosToNextLength = Vector2::DotProduct(Vector2::Subtract(next, curPos), dstToNext);

			return dstToNextLengthSqr > curPosToNextLength * curPosToNextLength;
		}
	}
	else if (type == PathType::Sub)
	{
		int distanceSqr = static_cast<int>(Vector2::DistanceSquare(curPos, m_subDestination));
		// 정확히 도착 하는 경우
		if (distanceSqr < dst * dst) return true;

		XMFLOAT2 next;
		if (m_subPath && !m_subPath->empty())
		{
			next = m_subPath->front().To();
		}
		else if (m_mainPath && !m_mainPath->empty())
		{
			next = m_mainPath->front().To();
		}
		else
		{
			return false;
		}
		XMFLOAT2 dstToNext = Vector2::Subtract(next, m_subDestination, true);
		float dstToNextLengthSqr = Vector2::DistanceSquare(next, m_subDestination);
		float curPosToNextLength = Vector2::DotProduct(Vector2::Subtract(next, curPos), dstToNext);

		return dstToNextLengthSqr > curPosToNextLength * curPosToNextLength;
	}

	return false;
}

bool CAnimatedObject::Walkable()
{
	if (m_curState != States::Idle) return false;
	return true;
}

bool CAnimatedObject::NoneDestination(PathType type)
{
	if (type == PathType::Main)
	{
		return m_destination.x == NONE;
	}
	else if (type == PathType::Sub)
	{
		return m_subDestination.x == NONE;
	}
	return false;
}

void CAnimatedObject::ResetDestination(PathType type)
{
	if (type == PathType::Main)
	{
		m_destination.x = NONE;
	}
	else if (type == PathType::Sub)
	{
		m_subDestination.x = NONE;
	}
}

void CAnimatedObject::ResetSubPath()
{
	if (m_subPath)
	{
		m_subPath->clear();
		Safe_Delete(m_subPath);
		ResetDestination(PathType::Sub);
	}
}

void CAnimatedObject::CheckRightWay(PathType type, shared_ptr<CWayFinder> pWayFinder)
{
	XMFLOAT2 destination;
	if (type == PathType::Main)
	{
		destination = m_destination;
	}
	else
	{
		destination = m_subDestination;
	}

	float dstSqr = Vector3::DistanceSquare(GetPosition(), XMFLOAT3(destination.x, 0, destination.y));
	if (dstSqr > (NODE_SIZE_SQR * 1.5f))
	{
		LookAt(destination);
	}
}

void CAnimatedObject::Recovery(float timeElapsed)
{
	m_recoveryTime += timeElapsed;

	// 10초동안 공격 받거나 공격 하지 않은 경우 회복 시작
	if (m_recoveryTime < TIME_ACTIVATE_RECOVERY) return;
	// 최대 체력까지 체력이 차있는 경우 제외하고 체력 회복
	// 회복이 된 경우 최근 회복 시간 변경
	if (!Heal(timeElapsed)) return;
}

void CAnimatedObject::ResetRecovery()
{
	m_recoveryTime = m_lastRecoveryTime = 0.f;
}
