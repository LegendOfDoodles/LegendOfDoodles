#include "stdafx.h"
#include "Sprite.h"

/// <summary>
/// 목적: Sprite 처리용 오브젝트
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-09-14
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CSprite::CSprite(shared_ptr<CCreateMgr> pCreateMgr) : CBaseObject(pCreateMgr)
{
}

CSprite::~CSprite()
{

}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CSprite::Render(CCamera * pCamera, UINT istanceCnt)
{
	OnPrepareRender();

	if (m_pMaterial)
	{
		m_pMaterial->Render(pCamera);
		m_pMaterial->UpdateShaderVariables();
	}

	if (m_cbvGPUDescriptorHandle.ptr)
		m_pCommandList->SetGraphicsRootDescriptorTable(7, m_cbvGPUDescriptorHandle);

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(istanceCnt);
		}
	}
}
