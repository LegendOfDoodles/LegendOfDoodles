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
/// 최종 수정 날짜: 2018-08-07
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
}

void CFlyingShader::SpawnFlyingObject(const XMFLOAT3& position, const XMFLOAT3& direction, TeamType teamType, FlyingObjectType objectType)
{
	int idx{ GetPossibleIndex(objectType) };

	if (idx != NONE)
	{
		m_ppObjects[idx]->ResetWorldMatrix();
		m_ppObjects[idx]->SaveIndex(idx);
		m_ppObjects[idx]->SetTeam(teamType);
		m_ppObjects[idx]->SetFlyingObjectsType(objectType);
		m_ppObjects[idx]->SetDirection(direction);
		m_ppObjects[idx]->ResetCollisionLevel();
		m_ppObjects[idx]->Activate();
		int adjIdx{ idx - m_objectsIndices[objectType].m_begIndex };
		if (objectType == FlyingObjectType::Roider_Dumbel)
		{
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(8), position.z));
			m_ppObjects[idx]->SetMesh(0, m_pMeshes[0]);
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (m_srvIncrementSize * adjIdx));
			m_ppObjects[idx]->Rotate(0, RandInRange(0.0f, 360.0f), 0);
			m_dumbelList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::Minion_Arrow)
		{
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(4), position.z));
			m_ppObjects[idx]->SetMesh(0, m_pMeshes[1]);
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[1].ptr + (m_srvIncrementSize * adjIdx));
			m_arrowList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == FlyingObjectType::Minion_Magic)
		{
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(4), position.z));
			m_ppObjects[idx]->SetMesh(0, m_pMeshes[2]);
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[2].ptr + (m_srvIncrementSize * adjIdx));
			m_magicList.emplace_back(m_ppObjects[idx]);
		}
	}
}

void CFlyingShader::SetColManagerToObject(shared_ptr<CCollisionManager> manager)
{
	for (int i = 0; i < m_nObjects; ++i) {
		m_ppObjects[i]->SetCollisionManager(manager);
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
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSTexturedLighting",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CFlyingShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSThrowingObjEmissive",
		"ps_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CFlyingShader::CreateNonEmissivePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSThrowingObj",
		"ps_5_1",
		pShaderBlob));
}

void CFlyingShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	UNREFERENCED_PARAMETER(isRenderShadow);

	m_nPipelineStates = 2;

	m_nHeaps = m_nMesh;
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
	UNREFERENCED_PARAMETER(pContext);

	CTransformImporter monsterTransformImporter;
	monsterTransformImporter.LoadMeshData("Resource//Data//MonsterSetting.txt");

	// 오브젝트 순서 설정
	FlyingObjectType objectOrder[]{
		FlyingObjectType::Roider_Dumbel,
		FlyingObjectType::Minion_Arrow,
		FlyingObjectType::Minion_Magic
	};

	// 각 오브젝트의 최대 개수 설정
	m_nObjects += m_objectsMaxCount[FlyingObjectType::Roider_Dumbel] = monsterTransformImporter.m_iKindMeshCnt[0];
	m_nObjects += m_objectsMaxCount[FlyingObjectType::Minion_Arrow] = MAX_ARROW;
	m_nObjects += m_objectsMaxCount[FlyingObjectType::Minion_Magic] = MAX_MAGIC;

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
	m_nMaterials = m_nMesh;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = Materials::CreateDumbbellMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[1] = Materials::CreateArrowMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);
	m_ppMaterials[2] = Materials::CreateMagicMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[2], &m_psrvGPUDescriptorStartHandle[2]);
#else
	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#endif

	m_srvIncrementSize = pCreateMgr->GetCbvSrvDescriptorIncrementSize();

	// 필요한 메쉬 저장
	m_pMeshes[0] = new CStaticMesh(pCreateMgr, "Resource//3D//Monster//Mesh//Dumbbell//Dumbbell.meshinfo");
	m_pMeshes[1] = new CStaticMesh(pCreateMgr, "Resource//3D//Common//Arrow.meshinfo");
	m_pMeshes[2] = new CStaticMesh(pCreateMgr, "Resource//3D//Common//MagicBall.meshinfo");

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

		m_ppObjects[j] = pObject;
	}
}

void CFlyingShader::ReleaseObjects()
{
	if (!m_dumbelList.empty()) m_dumbelList.clear();
	if (!m_arrowList.empty()) m_arrowList.clear();
	if (!m_magicList.empty()) m_magicList.clear();

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