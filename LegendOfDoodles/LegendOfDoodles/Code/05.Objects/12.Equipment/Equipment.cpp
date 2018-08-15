#include "stdafx.h"
#include "Equipment.h"
#include "05.Objects/08.Player/Player.h"


CEquipment::CEquipment(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CAnimatedObject(pCreateMgr, nMeshes)
{
}


CEquipment::~CEquipment()
{
}

void CEquipment::Render(CCamera * pCamera, UINT instanceCnt)
{
	OnPrepareRender();

	if (!IsVisible(pCamera) || !m_pPlayer->GetDetected()) return;

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

void CEquipment::AdjustAnimationIndex()
{
}
