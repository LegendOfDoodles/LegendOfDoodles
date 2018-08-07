#include "stdafx.h"
#include "UIFrameObject.h"

/// <summary>
/// 목적: UI Frame Object
/// 최종 수정자:  이용선
/// 수정자 목록:  이용선, 김나단
/// 최종 수정 날짜: 2018-05-23
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CUIFrameObject::CUIFrameObject(shared_ptr<CCreateMgr> pCreateMgr) : CBillboardObject(pCreateMgr)
{
}

CUIFrameObject::CUIFrameObject(shared_ptr<CCreateMgr> pCreateMgr, UIFrameType type) : CBillboardObject(pCreateMgr)
{
	CTexturedRectMesh *pRectMesh = NULL;

	switch (type)
	{
	case UIFrameType::MinimapFrame:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 150.58f, FRAME_BUFFER_HEIGHT / 160.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::KDAFrame:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 213.3f, FRAME_BUFFER_HEIGHT / 720.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::TimerFrame:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 320.f, FRAME_BUFFER_HEIGHT / 720.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::SkillFrameQ:
	case UIFrameType::SkillFrameW:
	case UIFrameType::SkillFrameE:
	case UIFrameType::SkillFrameR:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 640.f, FRAME_BUFFER_HEIGHT / 360.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::StatusFrame:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 232.7f, FRAME_BUFFER_HEIGHT / 288.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::CharacterFrame:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 320.f, FRAME_BUFFER_HEIGHT / 180.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::SpecialFrame:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 320.f, FRAME_BUFFER_HEIGHT / 120.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::SelectSpecial_7:
	case UIFrameType::SelectSpecial_12:
	case UIFrameType::SelectSpecial_17:
	case UIFrameType::SelectSpecial_21:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 1280.f, FRAME_BUFFER_HEIGHT / 720.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::CharacterFrameHP:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 426.6f, FRAME_BUFFER_HEIGHT / 240.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::CharacterFrameMP:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 320.f, FRAME_BUFFER_HEIGHT / 180.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::Minimap:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 160.f, FRAME_BUFFER_HEIGHT / 180.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	}

	m_type = type;
}

CUIFrameObject::~CUIFrameObject()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CUIFrameObject::Animate(float fTimeElapsed)
{
	CBillboardObject::Animate(fTimeElapsed);

	XMFLOAT3 newPos{ 0, 0, 0 };

	float UpVectorMoveWeight = 1.f;
	float RightVectorMoveWeight = 1.f;

	switch (m_type)
	{
	case UIFrameType::MinimapFrame:
	case UIFrameType::Minimap:
		UpVectorMoveWeight		= -(FRAME_BUFFER_HEIGHT / 102.85f);
		RightVectorMoveWeight	= (FRAME_BUFFER_WIDTH / 94.8f);
		break;
	case UIFrameType::KDAFrame:
		UpVectorMoveWeight		= (FRAME_BUFFER_HEIGHT / 75.7f);
		RightVectorMoveWeight	= (FRAME_BUFFER_WIDTH / 85.3f);
		break;
	case UIFrameType::TimerFrame:
		UpVectorMoveWeight = (FRAME_BUFFER_HEIGHT / 75.7f);
		RightVectorMoveWeight = -(FRAME_BUFFER_WIDTH / 81.f);
		break;
	case UIFrameType::SkillFrameQ:
		UpVectorMoveWeight		= -(FRAME_BUFFER_HEIGHT / 96.f);
		RightVectorMoveWeight	= -(FRAME_BUFFER_WIDTH / 296.f);
		break;
	case UIFrameType::SkillFrameW:
		UpVectorMoveWeight		= -(FRAME_BUFFER_HEIGHT / 96.f);
		RightVectorMoveWeight	= -(FRAME_BUFFER_WIDTH / 914.2f);
		break;
	case UIFrameType::SkillFrameE:
		UpVectorMoveWeight		= -(FRAME_BUFFER_HEIGHT / 96.f);
		RightVectorMoveWeight	= (FRAME_BUFFER_WIDTH / 914.2f);
		break;
	case UIFrameType::SkillFrameR:
		UpVectorMoveWeight		= -(FRAME_BUFFER_HEIGHT / 96.f);
		RightVectorMoveWeight	= (FRAME_BUFFER_WIDTH / 296.f);
		break;
	case UIFrameType::StatusFrame:
		UpVectorMoveWeight		= (FRAME_BUFFER_HEIGHT / 84.f);
		RightVectorMoveWeight	= (FRAME_BUFFER_WIDTH / 1280.f);
		break;
	case UIFrameType::CharacterFrame:
		UpVectorMoveWeight		= -(FRAME_BUFFER_HEIGHT / 96.f);
		RightVectorMoveWeight	= -(FRAME_BUFFER_WIDTH / 85.3f); 
		break;
	case UIFrameType::SpecialFrame:
		UpVectorMoveWeight		= -(FRAME_BUFFER_HEIGHT / 288.f);
		RightVectorMoveWeight	= -(FRAME_BUFFER_WIDTH / 87.67f);
		break;
	case UIFrameType::SelectSpecial_7:
		UpVectorMoveWeight		= -(FRAME_BUFFER_HEIGHT / 130.9f);
		RightVectorMoveWeight	= -(FRAME_BUFFER_WIDTH / 75.29f);
		break;
	case UIFrameType::SelectSpecial_12:
		UpVectorMoveWeight = -(FRAME_BUFFER_HEIGHT / 163.6f);
		RightVectorMoveWeight = -(FRAME_BUFFER_WIDTH / 75.29f);
		break;
	case UIFrameType::SelectSpecial_17:
		UpVectorMoveWeight = -(FRAME_BUFFER_HEIGHT / 218.18f);
		RightVectorMoveWeight = -(FRAME_BUFFER_WIDTH / 75.29f);
		break;
	case UIFrameType::SelectSpecial_21:
		UpVectorMoveWeight = -(FRAME_BUFFER_HEIGHT / 327.27f);
		RightVectorMoveWeight = -(FRAME_BUFFER_WIDTH / 75.29f);
		break;
	case UIFrameType::CharacterFrameHP:
		UpVectorMoveWeight		= -(FRAME_BUFFER_HEIGHT / 96.f);
		RightVectorMoveWeight	= -(FRAME_BUFFER_WIDTH / 85.3f);
		break;
	case UIFrameType::CharacterFrameMP:
		UpVectorMoveWeight		= -(FRAME_BUFFER_HEIGHT / 96.f);
		RightVectorMoveWeight	= -(FRAME_BUFFER_WIDTH / 85.3f);
		break;
	}

	newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
	newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), UpVectorMoveWeight)), Vector3::ScalarProduct(m_pCamera->GetRightVector(), RightVectorMoveWeight));

	m_xmf4x4World._41 = newPos.x;
	m_xmf4x4World._42 = newPos.y;
	m_xmf4x4World._43 = newPos.z;
}

void CUIFrameObject::Render(CCamera * pCamera, UINT istanceCnt)
{

	switch (m_type)
	{
	case UIFrameType::MinimapFrame:
	case UIFrameType::KDAFrame:
	case UIFrameType::TimerFrame:
	case UIFrameType::SkillFrameQ:
	case UIFrameType::SkillFrameW:
	case UIFrameType::SkillFrameE:
	case UIFrameType::SkillFrameR:
	case UIFrameType::StatusFrame:
	case UIFrameType::SpecialFrame:
	case UIFrameType::CharacterFrame:
	case UIFrameType::Minimap:
	case UIFrameType::SelectSpecial_7:
	case UIFrameType::SelectSpecial_12:
	case UIFrameType::SelectSpecial_17:
	case UIFrameType::SelectSpecial_21:
		CBillboardObject::Render(pCamera, istanceCnt);
		break;
	case UIFrameType::CharacterFrameHP:
	case UIFrameType::CharacterFrameMP:
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
		break;
	}
}

float CUIFrameObject::GetCurrentHP()
{
	return (m_pMasterObject->GetPlayerStatus()->HP / m_pMasterObject->GetPlayerStatus()->maxHP);
}

float CUIFrameObject::GetCurrentMP()
{
	return (m_pMasterObject->GetPlayerStatus()->MP / m_pMasterObject->GetPlayerStatus()->maxMP);
}
