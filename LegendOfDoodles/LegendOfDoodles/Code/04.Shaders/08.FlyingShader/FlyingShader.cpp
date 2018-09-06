#include "stdafx.h"
#include "FlyingShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"
#include "05.Objects/11.FlyingObject/FlyingObject.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"

/// <summary>
/// 목적: 날아다니는(화살 등) 오브젝트 그리기 용도의 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-09-06
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CFlyingShader::CFlyingShader(shared_ptr<CCreateMgr> pCreateMgr)
	: CShader(pCreateMgr)
{
}

CFlyingShader::~CFlyingShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CFlyingShader::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			m_ppObjects[j]->ReleaseUploadBuffers();
		}
	}

#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
#endif

	for (int j = 0; j < m_nMesh; j++)
	{
		m_pMeshes[j]->ReleaseUploadBuffers();
	}
}

void CFlyingShader::UpdateShaderVariables(int opt)
{
	static UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	int beg{ 0 }, end{ 0 };

	switch (opt)
	{
	case 0:	// Dumbel Update
		beg = m_objectsIndices[FlyingObjectType::Roider_Dumbel].m_begIndex;
		end = m_objectsIndices[FlyingObjectType::Roider_Dumbel].m_endIndex;
		break;
	case 1:	// Minion Arrow Update
		beg = m_objectsIndices[FlyingObjectType::Minion_Arrow].m_begIndex;
		end = m_objectsIndices[FlyingObjectType::Minion_Arrow].m_endIndex;
		break;
	case 2:	// Minion Magic Update
		beg = m_objectsIndices[FlyingObjectType::Minion_Magic].m_begIndex;
		end = m_objectsIndices[FlyingObjectType::Minion_Magic].m_endIndex;
		break;
	case 3:	// Blue Tower Atk Update
		beg = m_objectsIndices[FlyingObjectType::BlueTower_Attack].m_begIndex;
		end = m_objectsIndices[FlyingObjectType::BlueTower_Attack].m_endIndex;
		break;
	case 4:	// Red Tower Atk Update
		beg = m_objectsIndices[FlyingObjectType::RedTower_Attack].m_begIndex;
		end = m_objectsIndices[FlyingObjectType::RedTower_Attack].m_endIndex;
		break;
	case 5:	// Player Arrow Update
		beg = m_objectsIndices[FlyingObjectType::Player_Arrow].m_begIndex;
		end = m_objectsIndices[FlyingObjectType::Player_Arrow].m_endIndex;
		break;
	case 6:	// Player Magic Update
		beg = m_objectsIndices[FlyingObjectType::Player_MagicSkill_Q].m_begIndex;
		end = m_objectsIndices[FlyingObjectType::Player_MagicSkill_Q].m_endIndex;
		break;
	case 7:	// Player Arrow Skill R Update
		beg = m_objectsIndices[FlyingObjectType::Player_ArrowSkill_R].m_begIndex;
		end = m_objectsIndices[FlyingObjectType::Player_ArrowSkill_R].m_endIndex;
		break;
	case 8:	// Player Magic Skill R Update
		beg = m_objectsIndices[FlyingObjectType::Player_MagicSkill_R].m_begIndex;
		end = m_objectsIndices[FlyingObjectType::Player_MagicSkill_R].m_endIndex;
		break;
	case 9:	// Player Magic Update
		beg = m_objectsIndices[FlyingObjectType::Player_Magic].m_begIndex;
		end = m_objectsIndices[FlyingObjectType::Player_Magic].m_endIndex;
		break;
	}

	for (int i = beg; i < end; ++i)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CFlyingShader::AnimateObjects(float timeElapsed)
{
	// 리스트에서 제거할 조건 함수
	static auto removeFunc = [this](CCollisionObject* obj) {
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
	m_dumbelList.remove_if(removeFunc);
	m_arrowList.remove_if(removeFunc);
	m_magicList.remove_if(removeFunc);
	m_blueTowerAtkList.remove_if(removeFunc);
	m_redTowerAtkList.remove_if(removeFunc);
	m_playerArrowList.remove_if(removeFunc);
	m_playerMagicSkillQList.remove_if(removeFunc);
	m_playerArrowSkillWList.remove_if(removeFunc);
	m_playerMagicSkillRList.remove_if(removeFunc);
	m_playerMagicList.remove_if(removeFunc);
}

void CFlyingShader::Render(CCamera *pCamera)
{
	if (!m_dumbelList.empty())
	{
		CShader::Render(pCamera, 0);
		m_ppMaterials[0]->UpdateShaderVariables();
		for (auto iter = m_dumbelList.begin(); iter != m_dumbelList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_arrowList.empty())
	{
		CShader::Render(pCamera, 1, 1);
		m_ppMaterials[1]->UpdateShaderVariables();
		for (auto iter = m_arrowList.begin(); iter != m_arrowList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_magicList.empty())
	{
		CShader::Render(pCamera, 2);
		m_ppMaterials[2]->UpdateShaderVariables();
		for (auto iter = m_magicList.begin(); iter != m_magicList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_blueTowerAtkList.empty())
	{
		CShader::Render(pCamera, 3);
		m_ppMaterials[3]->UpdateShaderVariables();
		for (auto iter = m_blueTowerAtkList.begin(); iter != m_blueTowerAtkList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_redTowerAtkList.empty())
	{
		CShader::Render(pCamera, 4);
		m_ppMaterials[4]->UpdateShaderVariables();
		for (auto iter = m_redTowerAtkList.begin(); iter != m_redTowerAtkList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_playerArrowList.empty())
	{
		CShader::Render(pCamera, 5);
		m_ppMaterials[5]->UpdateShaderVariables();
		for (auto iter = m_playerArrowList.begin(); iter != m_playerArrowList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_playerMagicSkillQList.empty())
	{
		CShader::Render(pCamera, 6);
		m_ppMaterials[6]->UpdateShaderVariables();
		for (auto iter = m_playerMagicSkillQList.begin(); iter != m_playerMagicSkillQList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_playerArrowSkillWList.empty())
	{
		CShader::Render(pCamera, 7);
		m_ppMaterials[7]->UpdateShaderVariables();
		for (auto iter = m_playerArrowSkillWList.begin(); iter != m_playerArrowSkillWList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_playerMagicSkillRList.empty())
	{
		CShader::Render(pCamera, 8, 1);
		m_ppMaterials[8]->UpdateShaderVariables();
		for (auto iter = m_playerMagicSkillRList.begin(); iter != m_playerMagicSkillRList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_playerMagicList.empty())
	{
		CShader::Render(pCamera, 9);
		m_ppMaterials[9]->UpdateShaderVariables();
		for (auto iter = m_playerMagicList.begin(); iter != m_playerMagicList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
}

void CFlyingShader::SpawnFlyingObject(const XMFLOAT3& position, const XMFLOAT3& direction, TeamType teamType, FlyingObjectType objectType, float damage, float rangeAdj)
{
	FlyingObjectType adjObjectType{ objectType };

	if (objectType == FlyingObjectType::Player_ArrowSkill_Q ||
		objectType == FlyingObjectType::Player_ArrowSkill_W ||
		objectType == FlyingObjectType::Player_ArrowSkill_E)
	{
		adjObjectType = FlyingObjectType::Player_Arrow;
	}

	int idx{ GetPossibleIndex(adjObjectType) };

	if (idx != NONE)
	{
		m_ppObjects[idx]->ResetWorldMatrix();
		m_ppObjects[idx]->SaveIndex(idx);
		m_ppObjects[idx]->SetTeam(teamType);
		m_ppObjects[idx]->SetDirection(direction);
		m_ppObjects[idx]->SetFlyingObjectsType(objectType);
		m_ppObjects[idx]->SetDamage(damage);
		m_ppObjects[idx]->ApplyAtkRange(rangeAdj);
		m_ppObjects[idx]->ResetCollisionLevel();
		m_ppObjects[idx]->Activate();
		int adjIdx{ idx - m_objectsIndices[adjObjectType].m_begIndex };
		if (objectType == FlyingObjectType::Roider_Dumbel)
		{
			m_ppObjects[idx]->SetPosition(Vector3::Add(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(8), position.z), Vector3::ScalarProduct(direction, CONVERT_PaperUnit_to_InG(4), false)));
			m_ppObjects[idx]->SetMesh(0, m_pMeshes[0]);
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (m_srvIncrementSize * adjIdx));
			m_ppObjects[idx]->Rotate(0, RandInRange(0.0f, 360.0f), 0);
			m_dumbelList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::Minion_Arrow)
		{
			m_ppObjects[idx]->SetPosition(Vector3::Add(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(5), position.z), Vector3::ScalarProduct(direction, CONVERT_PaperUnit_to_InG(2), false)));
			m_ppObjects[idx]->SetMesh(0, m_pMeshes[1]);
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[1].ptr + (m_srvIncrementSize * adjIdx));
			m_arrowList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::Minion_Magic)
		{
			m_ppObjects[idx]->SetPosition(Vector3::Add(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(5), position.z), Vector3::ScalarProduct(direction, CONVERT_PaperUnit_to_InG(2), false)));
			m_ppObjects[idx]->SetMesh(0, m_pMeshes[2]);
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[2].ptr + (m_srvIncrementSize * adjIdx));
			m_magicList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::BlueTower_Attack)
		{
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y, position.z));
			m_ppObjects[idx]->SetMesh(0, m_pMeshes[3]);
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[3].ptr + (m_srvIncrementSize * adjIdx));
			m_blueTowerAtkList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::RedTower_Attack)
		{
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y, position.z));
			m_ppObjects[idx]->SetMesh(0, m_pMeshes[3]);
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[4].ptr + (m_srvIncrementSize * adjIdx));
			m_redTowerAtkList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::Player_Arrow ||
			objectType == FlyingObjectType::Player_ArrowSkill_Q ||
			objectType == FlyingObjectType::Player_ArrowSkill_E)
		{
			m_ppObjects[idx]->SetPosition(Vector3::Add(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(7), position.z), Vector3::ScalarProduct(direction, CONVERT_PaperUnit_to_InG(3), false)));
			m_ppObjects[idx]->SetMesh(0, m_pMeshes[4]);
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[5].ptr + (m_srvIncrementSize * adjIdx));
			m_playerArrowList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::Player_MagicSkill_Q)
		{
			m_ppObjects[idx]->SetPosition(Vector3::Add(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(7), position.z), Vector3::ScalarProduct(direction, CONVERT_PaperUnit_to_InG(3), false)));
			m_ppObjects[idx]->SetMesh(0, m_pMeshes[5]);
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[6].ptr + (m_srvIncrementSize * adjIdx));
			m_playerMagicSkillQList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::Player_ArrowSkill_W)
		{
			m_ppObjects[idx]->SetPosition(Vector3::Add(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(7), position.z), Vector3::ScalarProduct(direction, CONVERT_PaperUnit_to_InG(4), false)));
			m_ppObjects[idx]->SetMesh(0, m_pMeshes[6]);
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[5].ptr + (m_srvIncrementSize * adjIdx));
			m_playerArrowList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::Player_ArrowSkill_R)
		{
			m_ppObjects[idx]->SetPosition(Vector3::Add(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(7), position.z), Vector3::ScalarProduct(direction, CONVERT_PaperUnit_to_InG(3), false)));
			m_ppObjects[idx]->SetMesh(0, m_pMeshes[4]);
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[7].ptr + (m_srvIncrementSize * adjIdx));
			m_playerArrowSkillWList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::Player_MagicSkill_R)
		{
			m_ppObjects[idx]->SetPosition(Vector3::Add(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(100), position.z), Vector3::ScalarProduct(direction, CONVERT_PaperUnit_to_InG(13), false)));
			m_ppObjects[idx]->SetMesh(0, m_pMeshes[7]);
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[8].ptr + (m_srvIncrementSize * adjIdx));
			m_playerMagicSkillRList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::Player_Magic)
		{
			m_ppObjects[idx]->SetPosition(Vector3::Add(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(7), position.z), Vector3::ScalarProduct(direction, CONVERT_PaperUnit_to_InG(3), false)));
			m_ppObjects[idx]->SetMesh(0, m_pMeshes[2]);
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[9].ptr + (m_srvIncrementSize * adjIdx));
			m_playerMagicList.emplace_back(m_ppObjects[idx]);
		}
	}
}

void CFlyingShader::SetColManagerToObject(shared_ptr<CCollisionManager> manager)
{
	for (int i = 0; i < m_nObjects; ++i) {
		m_ppObjects[i]->SetCollisionManager(manager);
	}
}

void CFlyingShader::SetEffectManagerToObject(shared_ptr<CEffectMgr> manager)
{
	for (int i = 0; i < m_nObjects; ++i) {
		m_ppObjects[i]->SetEffectManager(manager);
	}
}
void CFlyingShader::SetSoundManagerToObject(shared_ptr<CSoundManager> manager)
{
	for (int i = 0; i < m_nObjects; ++i) {
		m_ppObjects[i]->SetSoundManager(manager);
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CFlyingShader::CreateInputLayout()
{
	UINT nInputElementDescs = 4;
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
		"NORMAL",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[2] = {
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		24,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[3] = {
		"TANGENT",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		32,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CFlyingShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/ThrowingShader.hlsl",
		"VSThrowingObj",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CFlyingShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/ThrowingShader.hlsl",
		"PSThrowingObjEmissive",
		"ps_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CFlyingShader::CreateNonEmissivePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/ThrowingShader.hlsl",
		"PSThrowingObj",
		"ps_5_1",
		pShaderBlob));
}

void CFlyingShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	UNREFERENCED_PARAMETER(isRenderShadow);

	m_nPipelineStates = 2;

	m_nHeaps = m_nMesh + 2;	// 메쉬 개수 + 타워 공격 2종류 - 동일한 매터리얼 쓰는 다른 메쉬(ArrowSkillW) 1종 + 동일한 메쉬에 다른 매터리얼 사용하는 스킬(ArrowSkillR) 1종 + 플레이어 마법 기본 공격 1종
	CreateDescriptorHeaps();

	m_ppPipelineStates.resize(m_nPipelineStates);

	m_isRenderBB = isRenderBB;

	int index{ 0 };
	HRESULT hResult;
	ComPtr<ID3DBlob> pVertexShaderBlob, pPixelShaderBlob;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc;
	::ZeroMemory(&pipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	pipelineStateDesc.pRootSignature = pCreateMgr->GetGraphicsRootSignature().Get();
	pipelineStateDesc.VS = CreateVertexShader(pVertexShaderBlob);
	pipelineStateDesc.PS = CreatePixelShader(pPixelShaderBlob);
	pipelineStateDesc.RasterizerState = CreateRasterizerState();
	pipelineStateDesc.BlendState = CreateBlendState();
	pipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	pipelineStateDesc.InputLayout = CreateInputLayout();
	pipelineStateDesc.SampleMask = UINT_MAX;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.NumRenderTargets = nRenderTargets;
	for (UINT i = 0; i < nRenderTargets; i++)
	{
		pipelineStateDesc.RTVFormats[i] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	}
	pipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	pipelineStateDesc.SampleDesc.Count = 1;
	pipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	hResult = pCreateMgr->GetDevice()->CreateGraphicsPipelineState(
		&pipelineStateDesc,
		IID_PPV_ARGS(m_ppPipelineStates[index++].GetAddressOf()));
	ThrowIfFailed(hResult);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC NonEmissivePipelineStateDesc{ pipelineStateDesc };
	NonEmissivePipelineStateDesc.PS = CreateNonEmissivePixelShader(pPixelShaderBlob);
	hResult = pCreateMgr->GetDevice()->CreateGraphicsPipelineState(
		&NonEmissivePipelineStateDesc,
		IID_PPV_ARGS(m_ppPipelineStates[index++].GetAddressOf()));
	ThrowIfFailed(hResult);
}

void CFlyingShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext)
{
	if (pContext) m_pTerrain = (CHeightMapTerrain*)pContext;

	CTransformImporter monsterTransformImporter;
	monsterTransformImporter.LoadMeshData("Resource//Data//MonsterSetting.txt");

	// 오브젝트 순서 설정
	FlyingObjectType objectOrder[]{
		FlyingObjectType::Roider_Dumbel,
		FlyingObjectType::Minion_Arrow,
		FlyingObjectType::Minion_Magic,
		FlyingObjectType::BlueTower_Attack,
		FlyingObjectType::RedTower_Attack,
		FlyingObjectType::Player_Arrow,
		FlyingObjectType::Player_MagicSkill_Q,
		FlyingObjectType::Player_ArrowSkill_R,
		FlyingObjectType::Player_MagicSkill_R,
		FlyingObjectType::Player_Magic
	};

	// 각 오브젝트의 최대 개수 설정
	m_nObjects += m_objectsMaxCount[FlyingObjectType::Roider_Dumbel] = monsterTransformImporter.m_iKindMeshCnt[0];
	m_nObjects += m_objectsMaxCount[FlyingObjectType::Minion_Arrow] = MAX_ARROW;
	m_nObjects += m_objectsMaxCount[FlyingObjectType::Minion_Magic] = MAX_MAGIC;
	m_nObjects += m_objectsMaxCount[FlyingObjectType::BlueTower_Attack] = MAX_EACH_TOWER_ATK;
	m_nObjects += m_objectsMaxCount[FlyingObjectType::RedTower_Attack] = MAX_EACH_TOWER_ATK;
	m_nObjects += m_objectsMaxCount[FlyingObjectType::Player_Arrow] = MAX_PLAYER_ARROW_ATK + MAX_PLAYER_SKILL + MAX_PLAYER_SKILL + MAX_PLAYER_SKILL;	// 화살 기본 공격 + Q스킬 + W스킬 + E스킬
	m_nObjects += m_objectsMaxCount[FlyingObjectType::Player_MagicSkill_Q] = MAX_PLAYER_MAGIC_ATK;
	m_nObjects += m_objectsMaxCount[FlyingObjectType::Player_ArrowSkill_R] = MAX_PLAYER_SKILL;
	m_nObjects += m_objectsMaxCount[FlyingObjectType::Player_MagicSkill_R] = MAX_PLAYER_SKILL;
	m_nObjects += m_objectsMaxCount[FlyingObjectType::Player_Magic] = MAX_PLAYER_MAGIC_ATK;

	// 각 오브젝트 개수 만큼 Possible Index 생성
	m_objectsPossibleIndices = std::unique_ptr<bool[]>(new bool[m_nObjects]);

	// 설정된 Possible Indices를 0(false)로 초기화
	memset(m_objectsPossibleIndices.get(), false, m_nObjects * sizeof(bool));

	m_ppObjects = new CCollisionObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	int accCnt{ 0 };

	CreateShaderVariables(pCreateMgr, ncbElementBytes, m_nObjects);
	for (int i = 0; i < m_nHeaps; ++i)
	{
		m_objectsIndices[objectOrder[i]] = FlyingObjectIndices();
		m_objectsIndices[objectOrder[i]].m_begIndex = accCnt;
		CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_objectsMaxCount[objectOrder[i]], 1, i);
		CreateConstantBufferViews(pCreateMgr, m_objectsMaxCount[objectOrder[i]], m_pConstBuffer.Get(), ncbElementBytes, accCnt, i);
		accCnt += m_objectsMaxCount[objectOrder[i]];
		m_objectsIndices[objectOrder[i]].m_endIndex = accCnt;
	}

#if USE_BATCH_MATERIAL
	m_nMaterials = m_nMesh + 2;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = Materials::CreateDumbbellMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[1] = Materials::CreateArrowMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);
	m_ppMaterials[2] = Materials::CreateMagicMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[2], &m_psrvGPUDescriptorStartHandle[2]);
	m_ppMaterials[3] = Materials::CreateBlueTowerAtkMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[3], &m_psrvGPUDescriptorStartHandle[3]);
	m_ppMaterials[4] = Materials::CreateRedTowerAtkMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[4], &m_psrvGPUDescriptorStartHandle[4]);
	m_ppMaterials[5] = Materials::CreatePlayerArrowMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[5], &m_psrvGPUDescriptorStartHandle[5]);
	m_ppMaterials[6] = Materials::CreatePlayerMagicSkillQMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[6], &m_psrvGPUDescriptorStartHandle[6]);
	m_ppMaterials[7] = Materials::CreatePlayerArrowSkillWMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[7], &m_psrvGPUDescriptorStartHandle[7]);
	m_ppMaterials[8] = Materials::CreatePlayerMagicSkillRMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[8], &m_psrvGPUDescriptorStartHandle[8]);
	m_ppMaterials[9] = Materials::CreatePlayerMagicMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[9], &m_psrvGPUDescriptorStartHandle[9]);
#else
	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#endif

	m_srvIncrementSize = pCreateMgr->GetCbvSrvDescriptorIncrementSize();

	// 필요한 메쉬 저장
	m_pMeshes[0] = new CStaticMesh(pCreateMgr, "Resource//3D//Monster//Mesh//Dumbbell//Dumbbell.meshinfo");
	m_pMeshes[1] = new CStaticMesh(pCreateMgr, "Resource//3D//Common//Arrow.meshinfo");
	m_pMeshes[2] = new CStaticMesh(pCreateMgr, "Resource//3D//Common//MagicBall.meshinfo");
	m_pMeshes[3] = new CStaticMesh(pCreateMgr, "Resource//3D//Common//Crayon.meshinfo");
	m_pMeshes[4] = new CStaticMesh(pCreateMgr, "Resource//3D//Player//Mesh//Throwing//PlayerArrow.meshinfo");
	m_pMeshes[5] = new CStaticMesh(pCreateMgr, "Resource//3D//Player//Mesh//Throwing//PlayerMagicSkillQ.meshinfo");
	m_pMeshes[6] = new CStaticMesh(pCreateMgr, "Resource//3D//Player//Mesh//Throwing//PlayerArrowSkillW.meshinfo");
	m_pMeshes[7] = new CStaticMesh(pCreateMgr, "Resource//3D//Player//Mesh//Throwing//PlayerMagicSkillR.meshinfo");

	for (int j = 0; j < m_nMesh; j++)
	{
		m_pMeshes[j]->AddRef();
	}

	// 오브젝트 생성
	CFlyingObject *pObject{ NULL };
	for (int j = 0; j < m_nObjects; ++j) {
		pObject = new CFlyingObject(pCreateMgr);

		pObject->SetTeam(TeamType::None);

		pObject->SetStatic(StaticType::Move);

		pObject->SetTerrainImage(m_pTerrain);

		m_ppObjects[j] = pObject;
	}
}

void CFlyingShader::ReleaseObjects()
{
	if (!m_dumbelList.empty()) m_dumbelList.clear();
	if (!m_arrowList.empty()) m_arrowList.clear();
	if (!m_magicList.empty()) m_magicList.clear();
	if (!m_blueTowerAtkList.empty()) m_blueTowerAtkList.clear();
	if (!m_redTowerAtkList.empty()) m_redTowerAtkList.clear();
	if (!m_playerArrowList.empty()) m_playerArrowList.clear();
	if (!m_playerMagicSkillQList.empty()) m_playerMagicSkillQList.clear();
	if (!m_playerArrowSkillWList.empty()) m_playerArrowSkillWList.clear();
	if (!m_playerMagicSkillRList.empty()) m_playerMagicSkillRList.clear();
	if (!m_playerMagicList.empty()) m_playerMagicList.clear();

	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			delete m_ppObjects[j];
		}
		Safe_Delete_Array(m_ppObjects);
	}

#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i < m_nMaterials; ++i)
		{
			Safe_Delete(m_ppMaterials[i]);
		}
		Safe_Delete_Array(m_ppMaterials);
	}
#endif

	for (int j = 0; j < m_nMesh; j++)
	{
		m_pMeshes[j]->Release();
	}
}

int CFlyingShader::GetPossibleIndex(FlyingObjectType type)
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