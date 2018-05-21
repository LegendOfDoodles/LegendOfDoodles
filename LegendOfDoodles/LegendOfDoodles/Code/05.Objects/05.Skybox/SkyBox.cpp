#include "stdafx.h"
#include "SkyBox.h"

/// <summary>
/// 목적: 스카이 박스 출력용 오브젝트
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-05-22
/// </summary>

////////////////////////////////////////////////////////////////////////
// CSkyBox
// 생성자, 소멸자
CSkyBox::CSkyBox(CCreateMgr *pCreateMgr) : CBaseObject(pCreateMgr, 6)
{
	CTexturedRectMesh *pSkyBoxMesh = new CTexturedRectMesh(pCreateMgr, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, +10.0f);
	SetMesh(0, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pCreateMgr, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, -10.0f);
	SetMesh(1, pSkyBoxMesh);	
	pSkyBoxMesh = new CTexturedRectMesh(pCreateMgr, 0.0f, 20.0f, 20.0f, -10.0f, 0.0f, 0.0f);
	SetMesh(2, pSkyBoxMesh);	
	pSkyBoxMesh = new CTexturedRectMesh(pCreateMgr, 0.0f, 20.0f, 20.0f, +10.0f, 0.0f, 0.0f);
	SetMesh(3, pSkyBoxMesh);	
	pSkyBoxMesh = new CTexturedRectMesh(pCreateMgr, 20.0f, 0.0f, 20.0f, 0.0f, +10.0f, 0.0f);
	SetMesh(4, pSkyBoxMesh);	
	pSkyBoxMesh = new CTexturedRectMesh(pCreateMgr, 20.0f, 0.0f, 20.0f, 0.0f, -10.0f, 0.0f);
	SetMesh(5, pSkyBoxMesh);
}

CSkyBox::~CSkyBox()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CSkyBox::Render(CCamera *pCamera, UINT istanceCnt)
{
	if (m_cbvGPUDescriptorHandle.ptr)
		m_pCommandList->SetGraphicsRootDescriptorTable(1, m_cbvGPUDescriptorHandle);

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_pMaterial) m_pMaterial->UpdateShaderVariable(i);
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(istanceCnt);
		}
	}
}

////////////////////////////////////////////////////////////////////////
// CFloor
// 생성자, 소멸자
CFloor::CFloor(CCreateMgr * pCreateMgr) : CBaseObject(pCreateMgr)
{
	CTexturedRectMesh *pFloorMesh = new CTexturedRectMesh(pCreateMgr, 1.0f, 0.0f, 1.0f, 8000.0f, -100.0f, 4000.0f);
	SetMesh(0, pFloorMesh);

	SetPosition(5000.0f, 3500.0f);
}

CFloor::~CFloor()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수