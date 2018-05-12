#include "stdafx.h"
#include "AnimatedObject.h"
#include "06.Meshes/00.Vertex/Vertex.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"

/// <summary>
/// 목적: 움직이는 오브젝트 처리용 기본 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  정휘현, 김나단
/// 최종 수정 날짜: 2018-05-12
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CAnimatedObject::CAnimatedObject(CCreateMgr * pCreateMgr, int nMeshes) : CCollisionObject(pCreateMgr, nMeshes)
{
}

CAnimatedObject::~CAnimatedObject()
{
	if (m_pathToGo) Safe_Delete(m_pathToGo);
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CAnimatedObject::Animate(float timeElapsed)
{
	ResetCollisionLevel();

	int Bcnt = m_pSkeleton[m_nAniIndex].GetBoneCount();

	for (int i = 0; i < Bcnt; ++i) {
		m_xmf4x4Frame[i] = m_pSkeleton[m_nAniIndex].GetBone(i).GetFrame((int)m_fFrameTime);
	}
}

void CAnimatedObject::Render(CCamera * pCamera, UINT instanceCnt)
{
	OnPrepareRender();

	if (m_pMaterial)
	{
		m_pMaterial->Render(pCamera);
		m_pMaterial->UpdateShaderVariables();
	}

	if (m_cbvGPUDescriptorHandle.ptr)
		m_pCommandList->SetGraphicsRootDescriptorTable(7, m_cbvGPUDescriptorHandle);

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
	if (m_curState == States::Attack) return;

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
	if (m_pathToGo)
	{
		m_pathToGo->clear();
		Safe_Delete(m_pathToGo);
	}
	m_pathToGo = path;
	ResetDestination();
	if(Walkable()) SetState(States::Walk);
}

ProcessType CAnimatedObject::MoveToDestination(float timeElapsed)
{
	if (m_curState != States::Walk) return States::Processing;
	if (!m_pathToGo) return States::Done;

	if (NoneDestination() || IsArrive(m_speed * timeElapsed))	//  도착 한 경우
	{
		if (m_pathToGo->empty())
		{
			Safe_Delete(m_pathToGo);
			ResetDestination();
			return States::Done;
		}
		else
		{
			m_destination = m_pathToGo->front().To();
			m_pathToGo->pop_front();
			LookAt(m_destination);
		}
	}
	else  // 아직 도착하지 않은 경우
	{
		MoveForward(m_speed * timeElapsed);
		XMFLOAT3 position = GetPosition();
		position.y = m_pTerrain->GetHeight(position.x, position.z);
		CBaseObject::SetPosition(position);
	}
	return States::Processing;
}

void CAnimatedObject::MoveToEnemy(float timeElapsed, CWayFinder* pWayFinder)
{
	if (!m_pEnemy) return;

	XMFLOAT3 pos = m_pEnemy->GetPosition();
	LookAt(XMFLOAT2(pos.x, pos.z));
	MoveForward(m_speed * timeElapsed);

	pWayFinder->AdjustValueByWallCollision(this, GetLook(), m_speed * timeElapsed);

	XMFLOAT3 position = GetPosition();
	position.y = m_pTerrain->GetHeight(position.x, position.z);
	CBaseObject::SetPosition(position);
}

void CAnimatedObject::RegenerateLookAt()
{
	if (NoneDestination()) return;

	LookAt(m_destination);
}

bool CAnimatedObject::Attackable(CCollisionObject * other)
{
	if (!CheckEnemyState(other)) return false;
	float dstSqr = Vector3::DistanceSquare(GetPosition(), other->GetPosition());
	// 공격 범위의 절반 안에 적이 있는지 확인 -> 여러 적을 공격하기 위함
	return (dstSqr < m_attackRange * m_attackRange * 0.5f);
}

bool CAnimatedObject::Chaseable(CCollisionObject * other)
{
	if (!CheckEnemyState(other)) return false;
	float dstSqr = Vector3::DistanceSquare(GetPosition(), other->GetPosition());
	return (dstSqr < m_detectRange * m_detectRange);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
bool CAnimatedObject::IsArrive(float dst)
{
	XMFLOAT2 curPos{ GetPosition().x, GetPosition().z };
	int distanceSqr = Vector2::DistanceSquare(curPos, m_destination);
	// 정확히 도착 하는 경우
	if (distanceSqr < dst * dst) return true;
	if (m_pathToGo->empty()) return false;

	XMFLOAT2 next = m_pathToGo->front().To();
	XMFLOAT2 dstToNext = Vector2::Subtract(next, m_destination, true);
	float dstToNextLengthSqr = Vector2::DistanceSquare(next, m_destination);
	float curPosToNextLength = Vector2::DotProduct(Vector2::Subtract(next, curPos), dstToNext);

	return dstToNextLengthSqr > curPosToNextLength * curPosToNextLength;
}

bool CAnimatedObject::Walkable()
{
	if (m_curState == States::Attack) return false;
	if (m_curState == States::Die) return false;
	if (m_curState == States::Remove) return false;
	if (m_curState == States::Win) return false;
	if (m_curState == States::Defeat) return false;
	return true;
}
