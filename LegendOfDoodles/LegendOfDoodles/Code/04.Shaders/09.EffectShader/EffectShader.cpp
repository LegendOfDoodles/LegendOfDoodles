#include "stdafx.h"
#include "EffectShader.h"
#include "05.Objects\96.Billboard\06.EffectObject\EffectObject.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"


/// <summary>
/// 목적: Effect 그리기 쉐이더
/// 최종 수정자:  이용선
/// 수정자 목록:  이용선
/// 최종 수정 날짜: 2018-08-06
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
}

void CEffectShader::UpdateShaderVariables(int opt)
{
	static UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	int beg{ 0 }, end{ 0 };

	switch (opt)
	{
	case 0:	// Dumbel Update
		beg = m_objectsIndices[EffectObjectType::Player_SwordSkill_Q].m_begIndex;
		end = m_objectsIndices[EffectObjectType::Player_SwordSkill_Q].m_endIndex;
		break;
	}

	for (int i = beg; i < end; ++i)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CEffectShader::AnimateObjects(float timeElapsed)
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
	m_SwordQSkillList.remove_if(removeFunc);
}

void CEffectShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera, 0);
	
	if (!m_SwordQSkillList.empty())
	{
		m_ppMaterials[0]->UpdateShaderVariables();
		for (auto iter = m_SwordQSkillList.begin(); iter != m_SwordQSkillList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	
}

void CEffectShader::SpawnEffectObject(const XMFLOAT3 & position, const float positionOffset, const XMFLOAT3 & direction, TeamType teamType, EffectObjectType objectType)
{
	int idx{ GetPossibleIndex(objectType) };

	if (idx != NONE)
	{
		m_ppObjects[idx]->ResetWorldMatrix();
		m_ppObjects[idx]->SaveIndex(idx);
		m_ppObjects[idx]->SetDirection(direction);
		m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + positionOffset, position.z));
		m_ppObjects[idx]->Activate();
		int adjIdx{ idx - m_objectsIndices[objectType].m_begIndex };
		if (objectType == EffectObjectType::Player_SwordSkill_Q)
		{
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (m_srvIncrementSize * adjIdx));
			m_SwordQSkillList.emplace_back(m_ppObjects[idx]);
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
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSTextured",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CEffectShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSTextured",
		"ps_5_1",
		pShaderBlob));
}

void CEffectShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CEffectShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void * pContext)
{
	m_pCamera = (CCamera*)pContext;

	// 오브젝트 순서 설정
	EffectObjectType objectOrder[]{
		EffectObjectType::Player_SwordSkill_Q,
	};

	// 각 오브젝트의 최대 개수 설정
	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_SwordSkill_Q] = MAX_QSKILL;

	// 각 오브젝트 개수 만큼 Possible Index 생성
	m_objectsPossibleIndices = std::unique_ptr<bool[]>(new bool[m_nObjects]);

	// 설정된 Possible Indices를 0(false)로 초기화
	memset(m_objectsPossibleIndices.get(), false, m_nObjects * sizeof(bool));

	m_ppObjects = new CCollisionObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	int accCnt{ 0 };

	CreateShaderVariables(pCreateMgr, ncbElementBytes, m_nObjects);
	
	m_objectsIndices[objectOrder[0]] = EffectObjectIndices();
	m_objectsIndices[objectOrder[0]].m_begIndex = accCnt;
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 1, 0);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pConstBuffer.Get(), ncbElementBytes, 0);
	accCnt += m_objectsMaxCount[objectOrder[0]];
	m_objectsIndices[objectOrder[0]].m_endIndex = accCnt;

	m_nMaterials = m_nHeaps;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = Materials::CreateEffectMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);

	m_srvIncrementSize = pCreateMgr->GetCbvSrvDescriptorIncrementSize();

	// 오브젝트 생성
	CEffectObject *pObject{ NULL };
	for (int j = 0; j < m_nObjects; ++j) {
		pObject = new CEffectObject(pCreateMgr);

		pObject->SetTeam(TeamType::None);
		pObject->SetCamera(m_pCamera);
		pObject->SetStatic(StaticType::Static);

		m_ppObjects[j] = pObject;
	}
}

void CEffectShader::ReleaseObjects()
{
	if (!m_SwordQSkillList.empty()) m_SwordQSkillList.clear();

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

