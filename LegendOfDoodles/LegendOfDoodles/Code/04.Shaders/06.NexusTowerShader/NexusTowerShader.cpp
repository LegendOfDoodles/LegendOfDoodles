#include "stdafx.h"
#include "NexusTowerShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"
#include "05.Objects/07.StaticObjects/Obstacle.h"
#include "05.Objects/09.NexusTower/NexusTower.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"
#include "00.Global/02.AI/00.FSMMgr/FSMMgr.h"
#include "07.Network/Network.h"

/// <summary>
/// 목적: 넥서스 및 타워 그리기 용도의 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-09-14
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CNexusTowerShader::CNexusTowerShader(shared_ptr<CCreateMgr> pCreateMgr)
	: CShader(pCreateMgr)
{
}

CNexusTowerShader::~CNexusTowerShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CNexusTowerShader::Initialize(shared_ptr<CCreateMgr> pCreateMgr, void *pContext)
{
	pCreateMgr->ResetCommandList();
	CreateShader(pCreateMgr, RENDER_TARGET_BUFFER_CNT, true, true);
	BuildObjects(pCreateMgr, pContext);
	pCreateMgr->ExecuteCommandList();
}

void CNexusTowerShader::UpdateShaderVariables(int opt)
{
	UNREFERENCED_PARAMETER(opt);
	static UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CNexusTowerShader::UpdateBoundingBoxShaderVariables()
{
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedBoundingBoxes + (i * boundingBoxElementBytes));

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CNexusTowerShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_pFSMMgr->Update(timeElapsed, m_ppObjects[j]);
	}
}

void CNexusTowerShader::Render(CCamera *pCamera)
{
	int cnt{ 0 };
	for (int i = 0; i < m_nMaterials; ++i)
	{
		int curMaterialCnt{ cnt };
		CShader::Render(pCamera, i);
		m_ppMaterials[i]->UpdateShaderVariables();
		for (int j = 0; j < m_meshCounts[i]; ++j, ++cnt)
		{
			if (m_ppObjects[cnt] && m_ppObjects[cnt]->IsAlive())
			{
				m_ppObjects[cnt]->Render(pCamera);
			}
		}
		// 부서진 타워 랜더링 시 이미시브 적용 하지 않음
		ChangePipeline(3);
		for (int j = 0; j < m_meshCounts[i]; ++j, ++curMaterialCnt)
		{
			if (m_ppObjects[curMaterialCnt] && !m_ppObjects[curMaterialCnt]->IsAlive())
			{
				m_ppObjects[curMaterialCnt]->Render(pCamera);
			}
		}
	}
}

void CNexusTowerShader::RenderBoundingBox(CCamera * pCamera)
{
	CShader::RenderBoundingBox(pCamera);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]) m_ppObjects[j]->RenderBoundingBox(pCamera);
	}
}

void CNexusTowerShader::RenderShadow(CCamera * pCamera)
{
	int cnt{ 0 };
	for (int i = 0; i < m_nMaterials; ++i)
	{
		CShader::Render(pCamera, i, 2);
		for (int j = 0; j < m_meshCounts[i]; ++j, ++cnt)
		{
			if (m_ppObjects[cnt]) m_ppObjects[cnt]->Render(pCamera);
		}
	}
}

bool CNexusTowerShader::OnProcessKeyInput(UCHAR* pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);
	return true;
}

void CNexusTowerShader::SetColManagerToObject(shared_ptr<CCollisionManager> manager)
{
	for (int i = 0; i < m_nObjects; ++i) {

		m_ppObjects[i]->SetCollisionManager(manager);
	}
}

void CNexusTowerShader::SetThrowingManagerToObject(shared_ptr<CThrowingMgr> manager)
{
	for (int i = 0; i < m_nObjects; ++i)
	{
		m_ppObjects[i]->SetThrowingManager(manager);
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CNexusTowerShader::CreateInputLayout()
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

D3D12_SHADER_BYTECODE CNexusTowerShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/StaticShader.hlsl",
		"VSTexturedLighting",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CNexusTowerShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/StaticShader.hlsl",
		"PSTexturedLightingEmissive",
		"ps_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CNexusTowerShader::CreateNoneEmissivePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/StaticShader.hlsl",
		"PSTexturedLighting",
		"ps_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CNexusTowerShader::CreateShadowVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/ShadowShader.hlsl",
		"VSTexturedLighting",
		"vs_5_1",
		pShaderBlob));
}

void CNexusTowerShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 4;

	m_nHeaps = 5;
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

	// Non-Emissive 파이프라인 추가
	D3D12_GRAPHICS_PIPELINE_STATE_DESC NoneEmissivePipelineStateDesc{ pipelineStateDesc };
	NoneEmissivePipelineStateDesc.PS = CreateNoneEmissivePixelShader(pPixelShaderBlob);

	if (isRenderBB)
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC BBPipelineStateDesc{ pipelineStateDesc };
		BBPipelineStateDesc.VS = CreateBoundingBoxVertexShader(pVertexShaderBlob);
		BBPipelineStateDesc.PS = CreateBoundingBoxPixelShader(pPixelShaderBlob);
		BBPipelineStateDesc.RasterizerState = CreateBoundingBoxRasterizerState();
		BBPipelineStateDesc.InputLayout = CreateBoundingBoxInputLayout();
		hResult = pCreateMgr->GetDevice()->CreateGraphicsPipelineState(
			&BBPipelineStateDesc,
			IID_PPV_ARGS(m_ppPipelineStates[index++].GetAddressOf()));
		ThrowIfFailed(hResult);
	}

	if (isRenderShadow)
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC ShadowPipelineStateDesc{ pipelineStateDesc };
		ShadowPipelineStateDesc.VS = CreateShadowVertexShader(pVertexShaderBlob);
		ShadowPipelineStateDesc.PS = CreateShadowPixelShader(pPixelShaderBlob);
		pipelineStateDesc.RasterizerState = CreateShadowRasterizerState();
		pipelineStateDesc.NumRenderTargets = 0;
		pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
		hResult = pCreateMgr->GetDevice()->CreateGraphicsPipelineState(
			&ShadowPipelineStateDesc,
			IID_PPV_ARGS(m_ppPipelineStates[index++].GetAddressOf()));
		ThrowIfFailed(hResult);
	}

	// Non-Emissive 파이프라인 추가
	hResult = pCreateMgr->GetDevice()->CreateGraphicsPipelineState(
		&NoneEmissivePipelineStateDesc,
		IID_PPV_ARGS(m_ppPipelineStates[index++].GetAddressOf()));
	ThrowIfFailed(hResult);
}

void CNexusTowerShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext)
{
	UNREFERENCED_PARAMETER(pContext);

	CTransformImporter transformInporter;

	transformInporter.LoadMeshData("Resource//Data//NexusTowerSetting.txt");

	m_nObjects = transformInporter.m_iTotalCnt;
	m_ppObjects = new CCollisionObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	int accCnt{ 0 };

	CreateShaderVariables(pCreateMgr, ncbElementBytes, m_nObjects, true, ncbElementBytes, m_nObjects);
	for (int i = 0; i < m_nHeaps - 1; ++i)
	{
		CreateCbvAndSrvDescriptorHeaps(pCreateMgr, transformInporter.m_iKindMeshCnt[i], 1, i);
		CreateConstantBufferViews(pCreateMgr, transformInporter.m_iKindMeshCnt[i], m_pConstBuffer.Get(), ncbElementBytes, accCnt, i);
		accCnt += transformInporter.m_iKindMeshCnt[i];
	}
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 0, m_nHeaps - 1);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pBoundingBoxBuffer.Get(), ncbElementBytes, 0, m_nHeaps - 1);

	SaveBoundingBoxHeapNumber(m_nHeaps - 1);

#if USE_BATCH_MATERIAL
	m_nMaterials = m_nHeaps - 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = Materials::CreateTresureBoxMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[1] = Materials::CreateShellMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);
	m_ppMaterials[2] = Materials::CreateRoundSoapDispenserMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[2], &m_psrvGPUDescriptorStartHandle[2]);
	m_ppMaterials[3] = Materials::CreateSquareSoapDispenserMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[3], &m_psrvGPUDescriptorStartHandle[3]);
#else
	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#endif

	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CStaticMesh *pMeshes[4];
	pMeshes[0] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//NexusTower//Treasure Box Nexus(UV).meshinfo");
	pMeshes[0]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(16)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(25), CONVERT_PaperUnit_to_InG(18), CONVERT_PaperUnit_to_InG(16)));

	pMeshes[1] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//NexusTower//Shell Nexus (UV).meshinfo");
	pMeshes[1]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(10)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(18), CONVERT_PaperUnit_to_InG(18), CONVERT_PaperUnit_to_InG(10)));

	pMeshes[2] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//NexusTower//Circle Soap Dispenser (UV).meshinfo");
	pMeshes[2]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(10)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(5), CONVERT_PaperUnit_to_InG(7), CONVERT_PaperUnit_to_InG(10)));

	pMeshes[3] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//NexusTower//Square Soap Dispenser ver.2 (UV).meshinfo");
	pMeshes[3]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(10)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(5), CONVERT_PaperUnit_to_InG(7), CONVERT_PaperUnit_to_InG(10)));
	CNexusTower *pBuild = NULL;

	m_nNexus = transformInporter.m_iKindMeshCnt[0] + transformInporter.m_iKindMeshCnt[1];
	m_nTower = transformInporter.m_iKindMeshCnt[2] + transformInporter.m_iKindMeshCnt[3];

	int cnt = 0;
	for (int i = 0; i < m_nMaterials; ++i) {
		m_meshCounts[i] = transformInporter.m_iKindMeshCnt[i];
		for (int j = 0; j < transformInporter.m_iKindMeshCnt[i]; ++j) {
			XMFLOAT3 pos = transformInporter.m_Transform[cnt].pos;
			XMFLOAT3 rot = transformInporter.m_Transform[cnt].rotation;
			pBuild = new CNexusTower(pCreateMgr);
			pBuild->SetPosition(CONVERT_Unit_to_InG(pos.x), CONVERT_Unit_to_InG(pos.y), CONVERT_Unit_to_InG(pos.z));
			if (i == 0 || i == 2)
			{
				pBuild->SetTeam(TeamType::Blue);
			}
			else {
				pBuild->SetTeam(TeamType::Red);
			}
			if (i < 2) {
				pBuild->SetCollisionSize(CONVERT_PaperUnit_to_InG(40));
				pBuild->SetType(ObjectType::Nexus);
			}
			else {
				pBuild->SetCollisionSize(CONVERT_PaperUnit_to_InG(8));
				pBuild->SetType(ObjectType::FirstTower);

			}
			pBuild->Rotate(0, 180, 0);
			pBuild->Rotate(-rot.x, rot.y, -rot.z);
			pBuild->SetMesh(0, pMeshes[i]);
			SetBoundingBoxMeshByIndex(pCreateMgr, pBuild, i);

			pBuild->ResetCollisionLevel();
			pBuild->SetStatic(StaticType::Static);
			pBuild->SetTag(static_cast<short>(30000 + cnt));

			pBuild->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[i].ptr + (incrementSize * j));
			pBuild->SetCbvGPUDescriptorHandlePtrForBB(m_pcbvGPUDescriptorStartHandle[m_nHeaps - 1].ptr + (incrementSize * cnt));
			m_ppObjects[cnt++] = pBuild;
		}
	}
}

void CNexusTowerShader::SetBoundingBoxMeshByIndex(shared_ptr<CCreateMgr> pCreateMgr, CBaseObject * target, int index)
{
	static CCubeMesh towerBBMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(10), CONVERT_PaperUnit_to_InG(7), CONVERT_PaperUnit_to_InG(20),
		0, 0, -CONVERT_PaperUnit_to_InG(10));
	towerBBMesh.AddRef();

	switch (index)
	{
	case 0: // 보물 상자
		target->SetBoundingMesh(new CCubeMesh(pCreateMgr,
			CONVERT_PaperUnit_to_InG(50), CONVERT_PaperUnit_to_InG(18), CONVERT_PaperUnit_to_InG(32),
			0, 0, -CONVERT_PaperUnit_to_InG(16)));
		break;
	case 1: // 진주 조개
		target->SetBoundingMesh(new CCubeMesh(pCreateMgr,
			CONVERT_PaperUnit_to_InG(36), CONVERT_PaperUnit_to_InG(18), CONVERT_PaperUnit_to_InG(20),
			0, 0, -CONVERT_PaperUnit_to_InG(10)));
		break;
	case 2: // 동글 비누 타워
	case 3: // 네모 비누 타워
		target->SetBoundingMesh(&towerBBMesh);
		break;
	}
}
