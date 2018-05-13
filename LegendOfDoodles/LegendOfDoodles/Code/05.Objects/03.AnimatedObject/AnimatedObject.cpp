#include "stdafx.h"
#include "AnimatedObject.h"
#include "06.Meshes/00.Vertex/Vertex.h"

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
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CAnimatedObject::Animate(float timeElapsed)
{
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

void CAnimatedObject::SetPosition(float x, float z)
{
	CBaseObject::SetPosition(x, m_pTerrain->GetHeight(x, z), z);
}

void CAnimatedObject::SetAnimation(AnimationsType newAnimation)
{
	m_nCurrAnimation = newAnimation;
	AdjustAnimationIndex();
	m_fFrameTime = 0.0f;
}

void CAnimatedObject::RegenerateWorldMatrixWithLook(XMFLOAT3 look)
{
	XMFLOAT3 up{ 0.0f, 1.0f, 0.0f };
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
