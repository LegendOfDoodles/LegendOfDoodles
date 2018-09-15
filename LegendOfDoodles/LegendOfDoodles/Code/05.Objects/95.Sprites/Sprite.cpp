#include "stdafx.h"
#include "Sprite.h"

/// <summary>
/// ����: Sprite ó���� ������Ʈ
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-09-14
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CSprite::CSprite(shared_ptr<CCreateMgr> pCreateMgr) : CBaseObject(pCreateMgr)
{
}

CSprite::~CSprite()
{

}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
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
