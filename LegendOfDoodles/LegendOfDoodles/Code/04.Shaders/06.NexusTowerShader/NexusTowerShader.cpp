#include "stdafx.h"
#include "NexusTowerShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"
#include "05.Objects/07.StaticObjects/Obstacle.h"
#include "05.Objects/09.NexusTower/NexusTower.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"

/// <summary>
/// 목적: 스테틱 오브젝트 그리기 용도의 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-06-27
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CNexusTowerShader::CNexusTowerShader(CCreateMgr *pCreateMgr, Network* network)
	: CShader(pCreateMgr)
{
	m_pNetwork = network;
}

CNexusTowerShader::~CNexusTowerShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CNexusTowerShader::Initialize(CCreateMgr *pCreateMgr, void *pContext)
{
	CreateShader(pCreateMgr, RENDER_TARGET_BUFFER_CNT, true);
	BuildObjects(pCreateMgr, pContext);
}

void CNexusTowerShader::ReleaseUploadBuffers()
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
}

void CNexusTowerShader::UpdateShaderVariables()
{
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
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CNexusTowerShader::Render(CCamera *pCamera)
{
	int cnt{ 0 };
	for (int i = 0; i < m_nMaterials; ++i)
	{
		for (int j = 0; j < m_meshCounts[i]; ++j, ++cnt)
		{
			if (j == 0)
			{
				CShader::Render(pCamera, i);
				m_ppMaterials[i]->UpdateShaderVariables();
			}
			if (m_ppObjects[cnt]) m_ppObjects[cnt]->Render(pCamera);
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

CBaseObject *CNexusTowerShader::PickObjectByRayIntersection(
	XMFLOAT3& pickPosition, XMFLOAT4X4& xmf4x4View, float &nearHitDistance)
{
	bool intersected = 0;

	nearHitDistance = FLT_MAX;
	float hitDistance = FLT_MAX;
	CBaseObject *pSelectedObject{ NULL };

	for (int j = 0; j < m_nObjects; j++)
	{
		intersected = m_ppObjects[j]->PickObjectByRayIntersection(pickPosition, xmf4x4View, hitDistance);
		if (intersected && (hitDistance < nearHitDistance))
		{
			nearHitDistance = hitDistance;
			pSelectedObject = m_ppObjects[j];
		}
	}

	return(pSelectedObject);
}

bool CNexusTowerShader::OnProcessKeyInput(UCHAR* pKeyBuffer)
{
	return true;
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

D3D12_SHADER_BYTECODE CNexusTowerShader::CreateVertexShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "VSTexturedLighting", "vs_5_1", ppShaderBlob));
}

D3D12_SHADER_BYTECODE CNexusTowerShader::CreatePixelShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "PSTexturedLightingEmissive", "ps_5_1", ppShaderBlob));
}

void CNexusTowerShader::CreateShader(CCreateMgr *pCreateMgr, UINT nRenderTargets, bool isRenderBB)
{
	m_nPipelineStates = 2;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	for (int i = 0; i < m_nPipelineStates; ++i)
	{
		m_ppPipelineStates[i] = NULL;
	}

	m_nHeaps = 5;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB);
}

void CNexusTowerShader::CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers)
{
	HRESULT hResult;

	UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	m_pConstBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		elementBytes * nBuffers,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	hResult = m_pConstBuffer->Map(0, NULL, (void **)&m_pMappedObjects);
	assert(SUCCEEDED(hResult) && "m_pConstBuffer->Map Failed");

	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	m_pBoundingBoxBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		boundingBoxElementBytes * nBuffers,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	hResult = m_pBoundingBoxBuffer->Map(0, NULL, (void **)&m_pMappedBoundingBoxes);
	assert(SUCCEEDED(hResult) && "m_pBoundingBoxBuffer->Map Failed");
}

void CNexusTowerShader::BuildObjects(CCreateMgr *pCreateMgr, void *pContext)
{

	CTransformImporter transformInporter;

	transformInporter.LoadMeshData("Resource//Data//NexusTowerSetting.txt");

	m_nObjects = transformInporter.m_iTotalCnt;
	m_ppObjects = new CBaseObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateShaderVariables(pCreateMgr, m_nObjects);
	for (int i = 0; i < m_nHeaps - 1; ++i)
	{
		CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 1, i);
		CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pConstBuffer, ncbElementBytes, i);
	}
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 0, m_nHeaps - 1);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pBoundingBoxBuffer, ncbElementBytes, m_nHeaps - 1);

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
			if (i < 2) pBuild->SetType(ObjectType::Nexus);
			else pBuild->SetType(ObjectType::FirstTower);

			pBuild->Rotate(0, 180, 0);
			pBuild->Rotate(-rot.x, rot.y, -rot.z);
			pBuild->SetMesh(0, pMeshes[i]);
			SetBoundingBoxMeshByIndex(pCreateMgr, pBuild, i);

			pBuild->SetStatic(StaticType::Static);

			pBuild->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * cnt));
			pBuild->SetCbvGPUDescriptorHandlePtrForBB(m_pcbvGPUDescriptorStartHandle[m_nHeaps - 1].ptr + (incrementSize * cnt));
			m_ppObjects[cnt++] = pBuild;
		}
	}
}

void CNexusTowerShader::ReleaseObjects()
{
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
			if (m_ppMaterials[i]) delete m_ppMaterials[i];
		}
		Safe_Delete(m_ppMaterials);
	}
#endif
}

void CNexusTowerShader::SetBoundingBoxMeshByIndex(CCreateMgr * pCreateMgr, CBaseObject * target, int index)
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
