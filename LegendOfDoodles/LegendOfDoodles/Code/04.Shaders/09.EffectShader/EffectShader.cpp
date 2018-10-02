#include "stdafx.h"
#include "EffectShader.h"
#include "05.Objects\96.Billboard\06.EffectObject\EffectObject.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"

/// <summary>
/// 목적: Effect 그리기 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  이용선, 김나단
/// 최종 수정 날짜: 2018-08-22
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CEffectShader::CEffectShader(shared_ptr<CCreateMgr> pCreateMgr)
	:CShader(pCreateMgr)
{
}

CEffectShader::~CEffectShader()
{
}

void CEffectShader::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			m_ppObjects[j]->ReleaseUploadBuffers();
		}
	}

	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}

	for (int j = 0; j < m_nMesh; j++)
	{
		m_ppMesh[j]->ReleaseUploadBuffers();
	}
}

void CEffectShader::UpdateShaderVariables(int opt)
{
	static UINT elementBytes = ((sizeof(CB_EFFECTOBJECT_INFO) + 255) & ~255);
	int beg{ 0 }, end{ 0 };

	switch (opt)
	{
	case 0:
		beg = 0;
		end = UseMatrialNumToObjectCnt[0];
		break;
	case 1:
		beg = UseMatrialNumToObjectCnt[0];
		end = UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1];
		break;
	case 2:
		beg = UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1];
		end = UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2];
		break;
	case 3:
		beg = UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2];
		end = UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3];
		break;
	case 4:
		beg = UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3];
		end = UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3] + UseMatrialNumToObjectCnt[4];
		break;
	case 5:
		beg = UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3] + UseMatrialNumToObjectCnt[4];
		end = UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3] + UseMatrialNumToObjectCnt[4] + UseMatrialNumToObjectCnt[5];
		break;
	case 6:
		beg = UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3] + UseMatrialNumToObjectCnt[4] + UseMatrialNumToObjectCnt[5];
		end = UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3] + UseMatrialNumToObjectCnt[4] + UseMatrialNumToObjectCnt[5] + UseMatrialNumToObjectCnt[6];
		break;
	case 7:
		beg = UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3] + UseMatrialNumToObjectCnt[4] + UseMatrialNumToObjectCnt[5] + UseMatrialNumToObjectCnt[6];
		end = UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3] + UseMatrialNumToObjectCnt[4] + UseMatrialNumToObjectCnt[5] + UseMatrialNumToObjectCnt[6] + UseMatrialNumToObjectCnt[7];
		break;
	}

	for (int i = beg; i < end; ++i)
	{
		CB_EFFECTOBJECT_INFO *pMappedObject = (CB_EFFECTOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		// Animation Time
		pMappedObject->m_fAnimationTime = ((CEffectObject*)m_ppObjects[i])->GetAnimationTime();
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CEffectShader::AnimateObjects(float timeElapsed)
{
	// 리스트에서 제거할 조건 함수
	auto removeFunc = [this](CCollisionObject* obj) {
		if (obj->GetState() == StatesType::Remove)
		{
			ResetPossibleIndex(obj->GetIndex());
			obj->Deactivate();
			return true;
		}
		return false;
	};

	if (m_Paused) return;
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}

	// 더 이상 업데이트 하면 안되는 오브젝트 리스트에서 제거
	m_PlayerSwordSkill_Q_EffectList.remove_if(removeFunc);
	m_PlayerSwordSkill_W_EffectList.remove_if(removeFunc);
	m_PlayerSwordSkill_E_EffectList.remove_if(removeFunc);
	m_PlayerSwordSkill_R_EffectList.remove_if(removeFunc);

	m_PlayerStaffSkill_W_EffectList.remove_if(removeFunc);
	m_PlayerStaffSkill_E_EffectList.remove_if(removeFunc);

	m_PlayerArrowAttack_EffectList.remove_if(removeFunc);
	m_PlayerStaffAttack_EffectList.remove_if(removeFunc);
	m_PlayerStaffQSkill_EffectList.remove_if(removeFunc);
	m_PlayerStaffESkill_EffectList.remove_if(removeFunc);

	m_MinionArrowAttack_EffectList.remove_if(removeFunc);
	m_MinionStaffAttack_EffectList.remove_if(removeFunc);

	m_MinionArrow_EffectList.remove_if(removeFunc);
	m_PlayerArrow_EffectList.remove_if(removeFunc);
	m_PlayerFireBall_EffectList.remove_if(removeFunc);
	m_TowerAttack_EffectList.remove_if(removeFunc);

	m_PlayerArrowAndFireBall_HitPosition_EffectList.remove_if(removeFunc);
	m_NormalHit_Effect.remove_if(removeFunc);

	m_GolemStandardAttack_EffectList.remove_if(removeFunc);
	m_GolemStumpAttack_EffectList.remove_if(removeFunc);
	m_GolemSpecialAttack_EffectList.remove_if(removeFunc);

	m_CircleLevelUp_EffectList.remove_if(removeFunc);
	m_ArrowLevelUp_EffectList.remove_if(removeFunc);

	m_Kill_EffectList.remove_if(removeFunc);
	m_Death_EffectList.remove_if(removeFunc);
	m_Miss_EffectList.remove_if(removeFunc);
	m_Recovery_EffectList.remove_if(removeFunc);
	m_Absorption_EffectList.remove_if(removeFunc);
}

void CEffectShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera, 0);

	if (!m_PlayerSwordSkill_Q_EffectList.empty())
	{
		m_ppMaterials[0]->UpdateShaderVariable(0);
		for (auto iter = m_PlayerSwordSkill_Q_EffectList.begin(); iter != m_PlayerSwordSkill_Q_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_PlayerSwordSkill_W_EffectList.empty())
	{
		m_ppMaterials[0]->UpdateShaderVariable(1);
		for (auto iter = m_PlayerSwordSkill_W_EffectList.begin(); iter != m_PlayerSwordSkill_W_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_PlayerSwordSkill_E_EffectList.empty())
	{
		m_ppMaterials[0]->UpdateShaderVariable(2);
		for (auto iter = m_PlayerSwordSkill_E_EffectList.begin(); iter != m_PlayerSwordSkill_E_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_PlayerSwordSkill_R_EffectList.empty())
	{
		m_ppMaterials[0]->UpdateShaderVariable(3);
		for (auto iter = m_PlayerSwordSkill_R_EffectList.begin(); iter != m_PlayerSwordSkill_R_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}

	if (!m_PlayerStaffSkill_W_EffectList.empty())
	{
		m_ppMaterials[0]->UpdateShaderVariable(4);
		for (auto iter = m_PlayerStaffSkill_W_EffectList.begin(); iter != m_PlayerStaffSkill_W_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_PlayerStaffSkill_E_EffectList.empty())
	{
		m_ppMaterials[0]->UpdateShaderVariable(5);
		for (auto iter = m_PlayerStaffSkill_E_EffectList.begin(); iter != m_PlayerStaffSkill_E_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}

	CShader::Render(pCamera, 1);

	if (!m_PlayerArrowAttack_EffectList.empty())
	{
		m_ppMaterials[1]->UpdateShaderVariable(0);
		for (auto iter = m_PlayerArrowAttack_EffectList.begin(); iter != m_PlayerArrowAttack_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_PlayerStaffAttack_EffectList.empty())
	{
		m_ppMaterials[1]->UpdateShaderVariable(1);
		for (auto iter = m_PlayerStaffAttack_EffectList.begin(); iter != m_PlayerStaffAttack_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_PlayerStaffQSkill_EffectList.empty())
	{
		m_ppMaterials[1]->UpdateShaderVariable(2);
		for (auto iter = m_PlayerStaffQSkill_EffectList.begin(); iter != m_PlayerStaffQSkill_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_PlayerStaffESkill_EffectList.empty())
	{
		m_ppMaterials[1]->UpdateShaderVariable(3);
		for (auto iter = m_PlayerStaffESkill_EffectList.begin(); iter != m_PlayerStaffESkill_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}

	CShader::Render(pCamera, 2);
	if (!m_MinionArrowAttack_EffectList.empty())
	{
		m_ppMaterials[2]->UpdateShaderVariable(0);
		for (auto iter = m_MinionArrowAttack_EffectList.begin(); iter != m_MinionArrowAttack_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_MinionStaffAttack_EffectList.empty())
	{
		m_ppMaterials[2]->UpdateShaderVariable(1);
		for (auto iter = m_MinionStaffAttack_EffectList.begin(); iter != m_MinionStaffAttack_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}

	CShader::Render(pCamera, 3);
	if (!m_MinionArrow_EffectList.empty())
	{
		m_ppMaterials[3]->UpdateShaderVariable(0);
		for (auto iter = m_MinionArrow_EffectList.begin(); iter != m_MinionArrow_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_PlayerArrow_EffectList.empty())
	{
		m_ppMaterials[3]->UpdateShaderVariable(1);
		for (auto iter = m_PlayerArrow_EffectList.begin(); iter != m_PlayerArrow_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_PlayerFireBall_EffectList.empty())
	{
		m_ppMaterials[3]->UpdateShaderVariable(2);
		for (auto iter = m_PlayerFireBall_EffectList.begin(); iter != m_PlayerFireBall_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_TowerAttack_EffectList.empty())
	{
		m_ppMaterials[3]->UpdateShaderVariable(3);
		for (auto iter = m_TowerAttack_EffectList.begin(); iter != m_TowerAttack_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}

	CShader::Render(pCamera, 4);
	if (!m_PlayerArrowAndFireBall_HitPosition_EffectList.empty())
	{
		m_ppMaterials[4]->UpdateShaderVariable(0);
		for (auto iter = m_PlayerArrowAndFireBall_HitPosition_EffectList.begin(); iter != m_PlayerArrowAndFireBall_HitPosition_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_NormalHit_Effect.empty())
	{
		m_ppMaterials[4]->UpdateShaderVariable(1);
		for (auto iter = m_NormalHit_Effect.begin(); iter != m_NormalHit_Effect.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}

	CShader::Render(pCamera, 5);
	if (!m_GolemStandardAttack_EffectList.empty())
	{
		m_ppMaterials[5]->UpdateShaderVariable(0);
		for (auto iter = m_GolemStandardAttack_EffectList.begin(); iter != m_GolemStandardAttack_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_GolemStumpAttack_EffectList.empty())
	{
		m_ppMaterials[5]->UpdateShaderVariable(1);
		for (auto iter = m_GolemStumpAttack_EffectList.begin(); iter != m_GolemStumpAttack_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_GolemSpecialAttack_EffectList.empty())
	{
		m_ppMaterials[5]->UpdateShaderVariable(2);
		for (auto iter = m_GolemSpecialAttack_EffectList.begin(); iter != m_GolemSpecialAttack_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}

	CShader::Render(pCamera, 6);
	if (!m_CircleLevelUp_EffectList.empty())
	{
		m_ppMaterials[6]->UpdateShaderVariable(0);
		for (auto iter = m_CircleLevelUp_EffectList.begin(); iter != m_CircleLevelUp_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_ArrowLevelUp_EffectList.empty())
	{
		m_ppMaterials[6]->UpdateShaderVariable(1);
		for (auto iter = m_ArrowLevelUp_EffectList.begin(); iter != m_ArrowLevelUp_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}

	CShader::Render(pCamera, 7);
	if (!m_Kill_EffectList.empty())
	{
		m_ppMaterials[7]->UpdateShaderVariable(0);
		for (auto iter = m_Kill_EffectList.begin(); iter != m_Kill_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_Death_EffectList.empty())
	{
		m_ppMaterials[7]->UpdateShaderVariable(1);
		for (auto iter = m_Death_EffectList.begin(); iter != m_Death_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_Miss_EffectList.empty())
	{
		m_ppMaterials[7]->UpdateShaderVariable(2);
		for (auto iter = m_Miss_EffectList.begin(); iter != m_Miss_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_Recovery_EffectList.empty())
	{
		m_ppMaterials[7]->UpdateShaderVariable(3);
		for (auto iter = m_Recovery_EffectList.begin(); iter != m_Recovery_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_Absorption_EffectList.empty())
	{
		m_ppMaterials[7]->UpdateShaderVariable(4);
		for (auto iter = m_Absorption_EffectList.begin(); iter != m_Absorption_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
}

void CEffectShader::SpawnEffectObject(const XMFLOAT3 & position, const XMFLOAT3 & direction, int aniLength, EffectObjectType objectType)
{
	int idx{ GetPossibleIndex(objectType) };

	if (idx != NONE)
	{
		m_ppObjects[idx]->ResetWorldMatrix();
		m_ppObjects[idx]->SaveIndex(idx);

		m_ppObjects[idx]->SetDirection(direction);
		m_ppObjects[idx]->SetEffectObjectsType(objectType);
		m_ppObjects[idx]->SetAnimationLength(aniLength);

		m_ppObjects[idx]->Activate();
		if (objectType == EffectObjectType::Player_SwordSkill_Q_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[1]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x + (CONVERT_PaperUnit_to_InG(16) * direction.x), position.y + CONVERT_PaperUnit_to_InG(3), position.z + (CONVERT_PaperUnit_to_InG(16) * direction.z)));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (m_srvIncrementSize * idx));
			m_PlayerSwordSkill_Q_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Player_SwordSkill_W_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[3]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(4), position.z));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (m_srvIncrementSize * idx));
			m_PlayerSwordSkill_W_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Player_SwordSkill_E_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[3]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x + (CONVERT_PaperUnit_to_InG(10) * direction.x), position.y + 50.f, position.z + (CONVERT_PaperUnit_to_InG(10) * direction.z)));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (m_srvIncrementSize * idx));
			m_PlayerSwordSkill_E_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Player_SwordSkill_R_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[3]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(3), position.z));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (m_srvIncrementSize * idx));
			m_PlayerSwordSkill_R_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Player_StaffSkill_W_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[3]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x + (CONVERT_PaperUnit_to_InG(8) * direction.x), position.y + 50.f, position.z + (CONVERT_PaperUnit_to_InG(8) * direction.z)));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (m_srvIncrementSize * idx));
			m_PlayerStaffSkill_W_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Player_StaffSkill_E_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[2]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x + (CONVERT_PaperUnit_to_InG(15) * direction.x), position.y + 50.f, position.z + (CONVERT_PaperUnit_to_InG(15) * direction.z)));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (m_srvIncrementSize * idx));
			m_PlayerStaffSkill_E_EffectList.emplace_back(m_ppObjects[idx]);
		}

		// Player Attack Motion Effect
		else if (objectType == EffectObjectType::Player_ArrowAttack_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[1]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(7.2), position.z));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[1].ptr + (m_srvIncrementSize * (idx - UseMatrialNumToObjectCnt[0])));
			m_PlayerArrowAttack_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Player_StaffAttack_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[3]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + 55.f, position.z));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[1].ptr + (m_srvIncrementSize * (idx - UseMatrialNumToObjectCnt[0])));
			m_PlayerStaffAttack_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Player_StaffQSkill_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[1]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(7.2), position.z));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[1].ptr + (m_srvIncrementSize *  (idx - UseMatrialNumToObjectCnt[0])));
			m_PlayerStaffQSkill_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Player_StaffESkill_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[1]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + 55.f, position.z));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[1].ptr + (m_srvIncrementSize * (idx - UseMatrialNumToObjectCnt[0])));
			m_PlayerStaffESkill_EffectList.emplace_back(m_ppObjects[idx]);
		}

		// Minion Attack Motion Effect
		else if (objectType == EffectObjectType::Minion_ArrowAttack_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[0]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(4), position.z - CONVERT_PaperUnit_to_InG(1)));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[2].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1]))));
			m_MinionArrowAttack_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Minion_StaffAttack_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[0]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x + (CONVERT_PaperUnit_to_InG(2) * direction.x), position.y + CONVERT_PaperUnit_to_InG(4), position.z + (CONVERT_PaperUnit_to_InG(4)  * direction.z)));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[2].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1]))));
			m_MinionStaffAttack_EffectList.emplace_back(m_ppObjects[idx]);
		}

		// FollowingToFlyingObejct Effect
		else if (objectType == EffectObjectType::Flying_MinionArrow_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[6]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + 4.f, position.z));
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[3].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2]))));
			m_MinionArrow_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Flying_PlayerArrow_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[7]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x - (CONVERT_PaperUnit_to_InG(3) * direction.x), position.y + 4.f, position.z - (CONVERT_PaperUnit_to_InG(3) * direction.z)));
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[3].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2]))));
			m_PlayerArrow_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Flying_PlayerFireBall_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[0]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + 4.f, position.z));
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[3].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2]))));
			m_PlayerFireBall_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Tower_Attack_Explosion_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[5]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(3), position.z));
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[3].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2]))));
			m_TowerAttack_EffectList.emplace_back(m_ppObjects[idx]);
		}

		// HIt Effect
		else if (objectType == EffectObjectType::Player_ArrowAndFireBall_HitPosition_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[2]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(4), position.z));
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[4].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3]))));
			m_PlayerArrowAndFireBall_HitPosition_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::NormallHit_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[7]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(4), position.z));
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[4].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3]))));
			m_NormalHit_Effect.emplace_back(m_ppObjects[idx]);
		}

		// Golem Attack Effect
		else if (objectType == EffectObjectType::Golem_StandardAttack_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[2]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x + (CONVERT_PaperUnit_to_InG(25) * direction.x), position.y + (CONVERT_PaperUnit_to_InG(4)), position.z + (CONVERT_PaperUnit_to_InG(25) * direction.z)));
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[5].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3] + UseMatrialNumToObjectCnt[4]))));
			m_GolemStandardAttack_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Golem_StumpAttack_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[2]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + (CONVERT_PaperUnit_to_InG(3)), position.z));
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[5].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3] + UseMatrialNumToObjectCnt[4]))));
			m_GolemStumpAttack_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Golem_SpecialAttack_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[4]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + (CONVERT_PaperUnit_to_InG(3)), position.z));
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[5].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3] + UseMatrialNumToObjectCnt[4]))));
			m_GolemSpecialAttack_EffectList.emplace_back(m_ppObjects[idx]);
		}

		// Level Up Effect
		else if (objectType == EffectObjectType::Player_LevelUp_CircleEffect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[4]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + (CONVERT_PaperUnit_to_InG(3)), position.z));
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[6].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3] + UseMatrialNumToObjectCnt[4] + UseMatrialNumToObjectCnt[5]))));
			m_CircleLevelUp_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Player_LevelUp_ArrowEffect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[4]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + (CONVERT_PaperUnit_to_InG(4)), position.z));
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[6].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3] + UseMatrialNumToObjectCnt[4] + UseMatrialNumToObjectCnt[5]))));
			m_ArrowLevelUp_EffectList.emplace_back(m_ppObjects[idx]);
		}

		// Text Effect
		else if (objectType == EffectObjectType::Kill_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[3]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + (CONVERT_PaperUnit_to_InG(3)), position.z));
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[7].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3] + UseMatrialNumToObjectCnt[4] + UseMatrialNumToObjectCnt[5] + UseMatrialNumToObjectCnt[6]))));
			m_Kill_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Death_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[3]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + (CONVERT_PaperUnit_to_InG(3)), position.z));
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[7].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3] + UseMatrialNumToObjectCnt[4] + UseMatrialNumToObjectCnt[5] + UseMatrialNumToObjectCnt[6]))));
			m_Death_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Miss_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[7]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + (CONVERT_PaperUnit_to_InG(7.2)), position.z));
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[7].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3] + UseMatrialNumToObjectCnt[4] + UseMatrialNumToObjectCnt[5] + UseMatrialNumToObjectCnt[6]))));
			m_Miss_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Recovery_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[7]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + (CONVERT_PaperUnit_to_InG(7.2)), position.z));
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[7].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3] + UseMatrialNumToObjectCnt[4] + UseMatrialNumToObjectCnt[5] + UseMatrialNumToObjectCnt[6]))));
			m_Recovery_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Absorption_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[7]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + (CONVERT_PaperUnit_to_InG(7.2)), position.z));
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[7].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3] + UseMatrialNumToObjectCnt[4] + UseMatrialNumToObjectCnt[5] + UseMatrialNumToObjectCnt[6]))));
			m_Absorption_EffectList.emplace_back(m_ppObjects[idx]);
		}
	}
}

D3D12_INPUT_LAYOUT_DESC CEffectShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pInputElementDescs[0] = {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[1] = {
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_BLEND_DESC CEffectShader::CreateBlendState()
{
	D3D12_BLEND_DESC blendDesc;
	::ZeroMemory(&blendDesc, sizeof(D3D12_BLEND_DESC));

	blendDesc.AlphaToCoverageEnable = TRUE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	blendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(blendDesc);
}

D3D12_SHADER_BYTECODE CEffectShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	//./Code/04.Shaders/99.GraphicsShader/
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/EffectShader.hlsl",
		"VSTexturedEffect",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CEffectShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/EffectShader.hlsl",
		"PSTexturedEffect",
		"ps_5_1",
		pShaderBlob));
}

void CEffectShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;

	m_nHeaps = m_nMesh;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CEffectShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void * pContext)
{
	m_pCamera = (CCamera*)pContext;

	// 오브젝트 순서 설정
	EffectObjectType objectOrder[]{
		EffectObjectType::Player_SwordSkill_Q_Effect,
		EffectObjectType::Player_SwordSkill_W_Effect,
		EffectObjectType::Player_SwordSkill_E_Effect,
		EffectObjectType::Player_SwordSkill_R_Effect,

		EffectObjectType::Player_StaffSkill_W_Effect,
		EffectObjectType::Player_StaffSkill_E_Effect,

		EffectObjectType::Player_ArrowAttack_Effect,
		EffectObjectType::Player_StaffAttack_Effect,
		EffectObjectType::Player_StaffQSkill_Effect,
		EffectObjectType::Player_StaffESkill_Effect,

		EffectObjectType::Minion_ArrowAttack_Effect,
		EffectObjectType::Minion_StaffAttack_Effect,

		EffectObjectType::Flying_MinionArrow_Effect,
		EffectObjectType::Flying_PlayerArrow_Effect,
		EffectObjectType::Flying_PlayerFireBall_Effect,
		EffectObjectType::Tower_Attack_Explosion_Effect,

		EffectObjectType::Player_ArrowAndFireBall_HitPosition_Effect,
		EffectObjectType::NormallHit_Effect,

		EffectObjectType::Golem_StandardAttack_Effect,
		EffectObjectType::Golem_StumpAttack_Effect,
		EffectObjectType::Golem_SpecialAttack_Effect,

		EffectObjectType::Player_LevelUp_CircleEffect,
		EffectObjectType::Player_LevelUp_ArrowEffect,

		EffectObjectType::Kill_Effect,
		EffectObjectType::Death_Effect,
		EffectObjectType::Miss_Effect,
		EffectObjectType::Recovery_Effect,
		EffectObjectType::Absorption_Effect
	};

	// 각 오브젝트의 최대 개수 설정
	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_SwordSkill_Q_Effect] = MAX_SKILL;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_SwordSkill_W_Effect] = MAX_SKILL;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_SwordSkill_E_Effect] = MAX_SKILL;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_SwordSkill_R_Effect] = MAX_SKILL;

	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_StaffSkill_W_Effect] = MAX_SKILL;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_StaffSkill_E_Effect] = MAX_SKILL;
	// 0번 Matrial을 사용하는 Obejct 갯수
	UseMatrialNumToObjectCnt[0] = m_nObjects;

	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_ArrowAttack_Effect] = MAX_SKILL;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_StaffAttack_Effect] = MAX_SKILL;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_StaffQSkill_Effect] = MAX_SKILL;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_StaffESkill_Effect] = MAX_SKILL;
	// 1번 Matrial을 사용하는 Obejct 갯수
	UseMatrialNumToObjectCnt[1] = (m_nObjects - UseMatrialNumToObjectCnt[0]);

	m_nObjects += m_objectsMaxCount[EffectObjectType::Minion_ArrowAttack_Effect] = MAX_ARROW;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Minion_StaffAttack_Effect] = MAX_MAGIC;
	// 2번 Matrial을 사용하는 Obejct 갯수
	UseMatrialNumToObjectCnt[2] = (m_nObjects - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1]));

	m_nObjects += m_objectsMaxCount[EffectObjectType::Flying_MinionArrow_Effect] = MAX_ARROW;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Flying_PlayerArrow_Effect] = MAX_ARROW;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Flying_PlayerFireBall_Effect] = MAX_MAGIC;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Tower_Attack_Explosion_Effect] = MAX_EACH_TOWER_ATK;
	// 3번 Matrial을 사용하는 Obejct 갯수
	UseMatrialNumToObjectCnt[3] = (m_nObjects - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2]));

	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_ArrowAndFireBall_HitPosition_Effect] = MAX_HIT;
	m_nObjects += m_objectsMaxCount[EffectObjectType::NormallHit_Effect] = MAX_HIT;
	// 4번 Matrial을 사용하는 Obejct 갯수
	UseMatrialNumToObjectCnt[4] = (m_nObjects - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3]));

	m_nObjects += m_objectsMaxCount[EffectObjectType::Golem_StandardAttack_Effect] = 1;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Golem_StumpAttack_Effect] = 1;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Golem_SpecialAttack_Effect] = 1;
	// 5번 Matrial을 사용하는 Obejct 갯수
	UseMatrialNumToObjectCnt[5] = (m_nObjects - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3] + UseMatrialNumToObjectCnt[4]));

	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_LevelUp_CircleEffect] = 4;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_LevelUp_ArrowEffect] = 4;
	// 6번 Matrial을 사용하는 Obejct 갯수
	UseMatrialNumToObjectCnt[6] = (m_nObjects - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3] + UseMatrialNumToObjectCnt[4] + UseMatrialNumToObjectCnt[5]));

	m_nObjects += m_objectsMaxCount[EffectObjectType::Kill_Effect] = 4;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Death_Effect] = 4;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Miss_Effect] = 20;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Recovery_Effect] = 20;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Absorption_Effect] = 20;
	// 6번 Matrial을 사용하는 Obejct 갯수
	UseMatrialNumToObjectCnt[7] = (m_nObjects - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3] + UseMatrialNumToObjectCnt[4] + UseMatrialNumToObjectCnt[5] + UseMatrialNumToObjectCnt[6]));

	// 각 오브젝트 개수 만큼 Possible Index 생성
	m_objectsPossibleIndices = std::unique_ptr<bool[]>(new bool[m_nObjects]);

	// 설정된 Possible Indices를 0(false)로 초기화
	memset(m_objectsPossibleIndices.get(), false, m_nObjects * sizeof(bool));

	m_ppObjects = new CCollisionObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_EFFECTOBJECT_INFO) + 255) & ~255);
	int accCnt{ 0 };

	CreateShaderVariables(pCreateMgr, ncbElementBytes, m_nObjects);

	// Matrial 갯수 별 설정
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, UseMatrialNumToObjectCnt[0], 6, 0);
	CreateConstantBufferViews(pCreateMgr, UseMatrialNumToObjectCnt[0], m_pConstBuffer.Get(), ncbElementBytes, 0, 0);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, UseMatrialNumToObjectCnt[1], 4, 1);
	CreateConstantBufferViews(pCreateMgr, UseMatrialNumToObjectCnt[1], m_pConstBuffer.Get(), ncbElementBytes, UseMatrialNumToObjectCnt[0], 1);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, UseMatrialNumToObjectCnt[2], 2, 2);
	CreateConstantBufferViews(pCreateMgr, UseMatrialNumToObjectCnt[2], m_pConstBuffer.Get(), ncbElementBytes, UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1], 2);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, UseMatrialNumToObjectCnt[3], 4, 3);
	CreateConstantBufferViews(pCreateMgr, UseMatrialNumToObjectCnt[3], m_pConstBuffer.Get(), ncbElementBytes, UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2], 3);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, UseMatrialNumToObjectCnt[4], 2, 4);
	CreateConstantBufferViews(pCreateMgr, UseMatrialNumToObjectCnt[4], m_pConstBuffer.Get(), ncbElementBytes, UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3], 4);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, UseMatrialNumToObjectCnt[5], 3, 5);
	CreateConstantBufferViews(pCreateMgr, UseMatrialNumToObjectCnt[5], m_pConstBuffer.Get(), ncbElementBytes, UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3] + UseMatrialNumToObjectCnt[4], 5);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, UseMatrialNumToObjectCnt[6], 2, 6);
	CreateConstantBufferViews(pCreateMgr, UseMatrialNumToObjectCnt[6], m_pConstBuffer.Get(), ncbElementBytes, UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3] + UseMatrialNumToObjectCnt[4] + UseMatrialNumToObjectCnt[5], 6);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, UseMatrialNumToObjectCnt[7], 5, 7);
	CreateConstantBufferViews(pCreateMgr, UseMatrialNumToObjectCnt[7], m_pConstBuffer.Get(), ncbElementBytes, UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3] + UseMatrialNumToObjectCnt[4] + UseMatrialNumToObjectCnt[5] + UseMatrialNumToObjectCnt[6], 7);

	// 오브젝트 Index
	for (int i = 0; i < EffectObjectTime_Max_COUNT; ++i) {
		m_objectsIndices[objectOrder[i]] = EffectObjectIndices();
		m_objectsIndices[objectOrder[i]].m_begIndex = accCnt;
		accCnt += m_objectsMaxCount[objectOrder[i]];
		m_objectsIndices[objectOrder[i]].m_endIndex = accCnt;
	}

	m_nMaterials = m_nHeaps;
	m_ppMaterials = new CMaterial*[m_nMaterials];

	m_ppMaterials[0] = Materials::CreatePlayerSkillEffectMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[1] = Materials::CreatePlayerAttackEffectMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);
	m_ppMaterials[2] = Materials::CreateMinionAttackEffectMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[2], &m_psrvGPUDescriptorStartHandle[2]);
	m_ppMaterials[3] = Materials::CreateFlyingObjectEffectMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[3], &m_psrvGPUDescriptorStartHandle[3]);
	m_ppMaterials[4] = Materials::CreateHitEffectMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[4], &m_psrvGPUDescriptorStartHandle[4]);
	m_ppMaterials[5] = Materials::CreateGolemAttackEffectMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[5], &m_psrvGPUDescriptorStartHandle[5]);
	m_ppMaterials[6] = Materials::CreateLevelUpEffectMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[6], &m_psrvGPUDescriptorStartHandle[6]);
	m_ppMaterials[7] = Materials::CreateTextEffectMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[7], &m_psrvGPUDescriptorStartHandle[7]);

	m_ppMesh[0] = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 12.8f, FRAME_BUFFER_HEIGHT / 7.2f, 0.f);		// 100 x 100
	m_ppMesh[1] = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 4.26f, FRAME_BUFFER_HEIGHT / 2.4f, 0.f);		// 300 x 300
	m_ppMesh[2] = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 3.65f, FRAME_BUFFER_HEIGHT / 2.05f, 0.f);		// 350 x 350
	m_ppMesh[3] = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 3.2f, FRAME_BUFFER_HEIGHT / 1.8f, 0.f);		// 400 x 400
	m_ppMesh[4] = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 2.84f, FRAME_BUFFER_HEIGHT / 1.6f, 0.f);		// 450 x 450
	m_ppMesh[5] = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 2.56f, FRAME_BUFFER_HEIGHT / 1.44f, 0.f);		// 500 x 500
	m_ppMesh[6] = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 25.6f, FRAME_BUFFER_HEIGHT / 14.4f, 0.f);		// 50 x 50
	m_ppMesh[7] = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 6.4f, FRAME_BUFFER_HEIGHT / 3.6f, 0.f);		// 200 x 200

	for (int i = 0; i < m_nMesh; ++i)
	{
		m_ppMesh[i]->AddRef();
	}

	m_srvIncrementSize = pCreateMgr->GetCbvSrvDescriptorIncrementSize();

	// 오브젝트 생성
	CEffectObject *pObject{ NULL };
	for (int j = 0; j < m_nObjects; ++j) {
		pObject = new CEffectObject(pCreateMgr);

		pObject->SetStatic(StaticType::Static);
		pObject->SetCamera(m_pCamera);

		m_ppObjects[j] = pObject;
	}
}

void CEffectShader::ReleaseObjects()
{
	if (!m_PlayerSwordSkill_Q_EffectList.empty()) m_PlayerSwordSkill_Q_EffectList.clear();
	if (!m_PlayerSwordSkill_W_EffectList.empty()) m_PlayerSwordSkill_W_EffectList.clear();
	if (!m_PlayerSwordSkill_E_EffectList.empty()) m_PlayerSwordSkill_E_EffectList.clear();
	if (!m_PlayerSwordSkill_R_EffectList.empty()) m_PlayerSwordSkill_R_EffectList.clear();

	if (!m_PlayerStaffSkill_W_EffectList.empty()) m_PlayerStaffSkill_W_EffectList.clear();
	if (!m_PlayerStaffSkill_E_EffectList.empty()) m_PlayerStaffSkill_E_EffectList.clear();

	if (!m_PlayerArrowAttack_EffectList.empty()) m_PlayerArrowAttack_EffectList.clear();
	if (!m_PlayerStaffAttack_EffectList.empty()) m_PlayerStaffAttack_EffectList.clear();
	if (!m_PlayerStaffESkill_EffectList.empty()) m_PlayerStaffESkill_EffectList.clear();
	if (!m_PlayerStaffQSkill_EffectList.empty()) m_PlayerStaffQSkill_EffectList.clear();

	if (!m_MinionArrowAttack_EffectList.empty()) m_MinionArrowAttack_EffectList.clear();
	if (!m_MinionStaffAttack_EffectList.empty()) m_MinionStaffAttack_EffectList.clear();

	if (!m_MinionArrow_EffectList.empty()) m_MinionArrow_EffectList.clear();
	if (!m_PlayerArrow_EffectList.empty()) m_PlayerArrow_EffectList.clear();
	if (!m_PlayerFireBall_EffectList.empty()) m_PlayerFireBall_EffectList.clear();
	if (!m_TowerAttack_EffectList.empty()) m_TowerAttack_EffectList.clear();

	if (!m_PlayerArrowAndFireBall_HitPosition_EffectList.empty()) m_PlayerArrowAndFireBall_HitPosition_EffectList.clear();
	if (!m_NormalHit_Effect.empty()) m_NormalHit_Effect.clear();

	if (!m_GolemStandardAttack_EffectList.empty()) m_GolemStandardAttack_EffectList.clear();
	if (!m_GolemStumpAttack_EffectList.empty()) m_GolemStumpAttack_EffectList.clear();
	if (!m_GolemSpecialAttack_EffectList.empty()) m_GolemSpecialAttack_EffectList.clear();

	if (!m_CircleLevelUp_EffectList.empty()) m_CircleLevelUp_EffectList.clear();
	if (!m_ArrowLevelUp_EffectList.empty()) m_ArrowLevelUp_EffectList.clear();

	if (!m_Kill_EffectList.empty()) m_Kill_EffectList.clear();
	if (!m_Death_EffectList.empty()) m_Death_EffectList.clear();
	if (!m_Miss_EffectList.empty()) m_Miss_EffectList.clear();
	if (!m_Recovery_EffectList.empty()) m_Recovery_EffectList.clear();
	if (!m_Absorption_EffectList.empty()) m_Absorption_EffectList.clear();

	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			delete m_ppObjects[j];
		}
		Safe_Delete_Array(m_ppObjects);
	}

	if (m_ppMaterials)
	{
		for (int i = 0; i < m_nMaterials; ++i)
		{
			Safe_Delete(m_ppMaterials[i]);
		}
		Safe_Delete_Array(m_ppMaterials);
	}

	for (int j = 0; j < m_nMesh; j++)
	{
		m_ppMesh[j]->ReleaseUploadBuffers();
	}
}

int CEffectShader::GetPossibleIndex(EffectObjectType type)
{
	for (int idx = m_objectsIndices[type].m_begIndex; idx < m_objectsIndices[type].m_endIndex; ++idx)
	{
		if (!m_objectsPossibleIndices[idx])
		{
			m_objectsPossibleIndices[idx] = true;
			return idx;
		}
	}
	return NONE;
}

