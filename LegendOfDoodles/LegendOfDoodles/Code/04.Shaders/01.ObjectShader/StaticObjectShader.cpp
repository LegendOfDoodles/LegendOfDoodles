#include "stdafx.h"
#include "StaticObjectShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"
#include "05.Objects/07.StaticObjects//Obstacle.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"

/// <summary>
/// 목적: 스테틱 오브젝트 그리기 용도의 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-07-03
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CStaticObjectShader::CStaticObjectShader(shared_ptr<CCreateMgr> pCreateMgr)
	: CShader(pCreateMgr)
{
}

CStaticObjectShader::~CStaticObjectShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CStaticObjectShader::Initialize(shared_ptr<CCreateMgr> pCreateMgr, void *pContext)
{
	CreateShader(pCreateMgr, RENDER_TARGET_BUFFER_CNT, true, true);
	BuildObjects(pCreateMgr, pContext);
}

void CStaticObjectShader::UpdateShaderVariables(int opt)
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

void CStaticObjectShader::UpdateBoundingBoxShaderVariables()
{
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedBoundingBoxes + (i * boundingBoxElementBytes));

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CStaticObjectShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CStaticObjectShader::Render(CCamera *pCamera)
{
	int cnt{ 0 };
	for (int i = 0; i < m_nMaterials; ++i)
	{
		CShader::Render(pCamera, i);
		m_ppMaterials[i]->UpdateShaderVariables();
		for (int j = 0; j < m_meshCounts[i]; ++j, ++cnt)
		{
			if (m_ppObjects[cnt]) m_ppObjects[cnt]->Render(pCamera);
		}
	}
}

void CStaticObjectShader::RenderBoundingBox(CCamera * pCamera)
{
	CShader::RenderBoundingBox(pCamera);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]) m_ppObjects[j]->RenderBoundingBox(pCamera);
	}
}

void CStaticObjectShader::RenderShadow(CCamera * pCamera)
{
	int cnt{ 0 };
	for (int i = 0; i < m_nMaterials; ++i)
	{
		for (int j = 0; j < m_meshCounts[i]; ++j, ++cnt)
		{
			if (j == 0)
			{
				CShader::Render(pCamera, i, 2);
			}
			if (m_ppObjects[cnt]) m_ppObjects[cnt]->Render(pCamera);
		}
	}
}

bool CStaticObjectShader::OnProcessKeyInput(UCHAR* pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);

	return true;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CStaticObjectShader::CreateInputLayout()
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

D3D12_SHADER_BYTECODE CStaticObjectShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSTexturedLighting",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CStaticObjectShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSTexturedLightingDetail",
		"ps_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CStaticObjectShader::CreateShadowVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/ShadowShader.hlsl",
		"VSTexturedLighting",
		"vs_5_1",
		pShaderBlob));
}

void CStaticObjectShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 3;

	m_nHeaps = 18;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CStaticObjectShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext)
{
	if (pContext) m_pTerrain = (CHeightMapTerrain*)pContext;

	CTransformImporter transformInporter;

	transformInporter.LoadMeshData("Resource//Data//ObjectSetting.txt");

	m_nObjects = transformInporter.m_iTotalCnt;
	m_ppObjects = new CBaseObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	int accCnt{ 0 };

	CreateShaderVariables(pCreateMgr, ncbElementBytes, m_nObjects, true, ncbElementBytes, m_nObjects);
	for (int i = 0; i < m_nHeaps - 1; ++i)
	{
		CreateCbvAndSrvDescriptorHeaps(pCreateMgr, transformInporter.m_iKindMeshCnt[i], 4, i);
		CreateConstantBufferViews(pCreateMgr, transformInporter.m_iKindMeshCnt[i], m_pConstBuffer.Get(), ncbElementBytes, accCnt, i);
		accCnt += transformInporter.m_iKindMeshCnt[i];
	}
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 0, m_nHeaps - 1);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pBoundingBoxBuffer.Get(), ncbElementBytes, 0, m_nHeaps - 1);

	SaveBoundingBoxHeapNumber(m_nHeaps - 1);

#if USE_BATCH_MATERIAL
	m_nMaterials = m_nHeaps - 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = Materials::CreateEraserMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[1] = Materials::CreateDuckMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);
	m_ppMaterials[2] = Materials::CreateKeumOneBoMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[2], &m_psrvGPUDescriptorStartHandle[2]);
	m_ppMaterials[3] = Materials::CreatePencilCaseMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[3], &m_psrvGPUDescriptorStartHandle[3]);
	m_ppMaterials[4] = Materials::CreateNailClipperMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[4], &m_psrvGPUDescriptorStartHandle[4]);
	m_ppMaterials[5] = Materials::CreateSquareHeadPhoneMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[5], &m_psrvGPUDescriptorStartHandle[5]);
	m_ppMaterials[6] = Materials::CreateShortPencilMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[6], &m_psrvGPUDescriptorStartHandle[6]);
	m_ppMaterials[7] = Materials::CreateLongPencilMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[7], &m_psrvGPUDescriptorStartHandle[7]);
	m_ppMaterials[8] = Materials::CreatePaperCupMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[8], &m_psrvGPUDescriptorStartHandle[8]);
	m_ppMaterials[9] = Materials::CreateRoundHeadPhoneMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[9], &m_psrvGPUDescriptorStartHandle[9]);
	m_ppMaterials[10] = Materials::CreatePenCoverMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[10], &m_psrvGPUDescriptorStartHandle[10]);
	m_ppMaterials[11] = Materials::CreatePenMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[11], &m_psrvGPUDescriptorStartHandle[11]);
	m_ppMaterials[12] = Materials::CreateDiceMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[12], &m_psrvGPUDescriptorStartHandle[12]);
	m_ppMaterials[13] = Materials::CreateBook1Material(pCreateMgr, &m_psrvCPUDescriptorStartHandle[13], &m_psrvGPUDescriptorStartHandle[13]);
	m_ppMaterials[14] = Materials::CreateBook2Material(pCreateMgr, &m_psrvCPUDescriptorStartHandle[14], &m_psrvGPUDescriptorStartHandle[14]);
	m_ppMaterials[15] = Materials::CreateBook3Material(pCreateMgr, &m_psrvCPUDescriptorStartHandle[15], &m_psrvGPUDescriptorStartHandle[15]);
	m_ppMaterials[16] = Materials::CreateBook4Material(pCreateMgr, &m_psrvCPUDescriptorStartHandle[16], &m_psrvGPUDescriptorStartHandle[16]);
#else
	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#endif

	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CStaticMesh *pMeshes[17];
	pMeshes[0] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//Eraser.meshinfo");
	pMeshes[0]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(13.0f)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(26.5f), CONVERT_PaperUnit_to_InG(7.0f), CONVERT_PaperUnit_to_InG(13.0f)));

	pMeshes[1] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//Duck.meshinfo");
	pMeshes[1]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(14.0f)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(14.0f), CONVERT_PaperUnit_to_InG(14.0f), CONVERT_PaperUnit_to_InG(14.0f)));

	pMeshes[2] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//KeumWonBo.meshinfo");
	pMeshes[2]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(18.0f), CONVERT_PaperUnit_to_InG(18.0f), CONVERT_PaperUnit_to_InG(12.5f)));

	pMeshes[3] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//PencilCase.meshinfo");//***
	pMeshes[3]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(12.0f)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(13.5f), CONVERT_PaperUnit_to_InG(47.0f), CONVERT_PaperUnit_to_InG(12.0f)));

	pMeshes[4] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//Nail.meshinfo");//***
	pMeshes[4]->SetBoundingBox(
		XMFLOAT3(CONVERT_PaperUnit_to_InG(12.0f), 0.0f, -CONVERT_PaperUnit_to_InG(8.0f)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(21.0f), CONVERT_PaperUnit_to_InG(3.0f), CONVERT_PaperUnit_to_InG(8.0f)));

	pMeshes[5] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//BlueHead.meshinfo");//***
	pMeshes[5]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(65.0f), CONVERT_PaperUnit_to_InG(17.0f), CONVERT_PaperUnit_to_InG(65.0f)));

	pMeshes[6] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//ShortPencil.meshinfo");
	pMeshes[6]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(25.0f)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(7.5f), CONVERT_PaperUnit_to_InG(7.5f), CONVERT_PaperUnit_to_InG(25.0f)));

	pMeshes[7] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//LongPencil.meshinfo");
	pMeshes[7]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(45.0f)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(9.0f), CONVERT_PaperUnit_to_InG(9.0f), CONVERT_PaperUnit_to_InG(45.0f)));

	pMeshes[8] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//Cup.meshinfo");
	pMeshes[8]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(18.0f)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(13.0f), CONVERT_PaperUnit_to_InG(13.0f), CONVERT_PaperUnit_to_InG(18.0f)));

	pMeshes[9] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//RedHead.meshinfo");
	pMeshes[9]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(10.0f)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(65.0f), CONVERT_PaperUnit_to_InG(17.0f), CONVERT_PaperUnit_to_InG(50.0f)));

	pMeshes[10] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//PenCover.meshinfo");
	pMeshes[10]->SetBoundingBox(
		XMFLOAT3(0.0f, CONVERT_PaperUnit_to_InG(30.0f), 0.0f),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(8.0f), CONVERT_PaperUnit_to_InG(30.0f), CONVERT_PaperUnit_to_InG(8.0f)));

	pMeshes[11] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//Pen.meshinfo");
	pMeshes[11]->SetBoundingBox(
		XMFLOAT3(CONVERT_PaperUnit_to_InG(45.0f), 0.0f, -CONVERT_PaperUnit_to_InG(9.0f)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(45.0f), CONVERT_PaperUnit_to_InG(9.0f), CONVERT_PaperUnit_to_InG(9.0f)));

	pMeshes[12] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//Dice.meshinfo");
	pMeshes[12]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(12.5f), CONVERT_PaperUnit_to_InG(12.5f), CONVERT_PaperUnit_to_InG(12.5f)));

	pMeshes[13] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//Book.meshinfo", transformInporter.BookScale[0]);
	pMeshes[13]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(48.0f)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(37.0f), CONVERT_PaperUnit_to_InG(3.75f), CONVERT_PaperUnit_to_InG(96.0f)));

	pMeshes[14] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//Book.meshinfo", transformInporter.BookScale[1]);
	pMeshes[14]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(32.5f)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(27.0f), CONVERT_PaperUnit_to_InG(3.0f), CONVERT_PaperUnit_to_InG(32.5f)));

	pMeshes[15] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//Book.meshinfo", transformInporter.BookScale[2]);
	pMeshes[15]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(32.5f)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(27.0f), CONVERT_PaperUnit_to_InG(9.0f), CONVERT_PaperUnit_to_InG(32.5f)));

	pMeshes[16] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//Book.meshinfo", transformInporter.BookScale[3]);
	pMeshes[16]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(32.5f)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(27.0f), CONVERT_PaperUnit_to_InG(6.0f), CONVERT_PaperUnit_to_InG(32.5f)));

	int cnt = 0;
	for (int i = 0; i < m_nMaterials; ++i) {
		m_meshCounts[i] = transformInporter.m_iKindMeshCnt[i];
		for (int j = 0; j < transformInporter.m_iKindMeshCnt[i]; ++j) {
			XMFLOAT3 pos = transformInporter.m_Transform[cnt].pos;
			XMFLOAT3 rot = transformInporter.m_Transform[cnt].rotation;
			m_ppObjects[cnt] = new CObstacle(pCreateMgr);
			m_ppObjects[cnt]->SetPosition(CONVERT_Unit_to_InG(pos.x), CONVERT_Unit_to_InG(pos.y), CONVERT_Unit_to_InG(pos.z));

			m_ppObjects[cnt]->Rotate(0.0f, 180.0f, 0.0f);
			m_ppObjects[cnt]->Rotate(-rot.x, rot.y, -rot.z);

			m_ppObjects[cnt]->SetMesh(0, pMeshes[i]);
			SetBoundingBoxMeshByIndex(pCreateMgr, m_ppObjects[cnt], i);

			m_ppObjects[cnt]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[i].ptr + (incrementSize * j));
			m_ppObjects[cnt]->SetCbvGPUDescriptorHandlePtrForBB(m_pcbvGPUDescriptorStartHandle[m_nHeaps - 1].ptr + (incrementSize * cnt));
			++cnt;
		}
	}
}

void CStaticObjectShader::SetBoundingBoxMeshByIndex(shared_ptr<CCreateMgr> pCreateMgr, CBaseObject * target, int index)
{
	static CCubeMesh eraserBBMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(53.0f), CONVERT_PaperUnit_to_InG(7.0f), CONVERT_PaperUnit_to_InG(26.0f),
		0, 0, -CONVERT_PaperUnit_to_InG(13.0f));
	eraserBBMesh.AddRef();
	static CCubeMesh duckBBMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(28.0f), CONVERT_PaperUnit_to_InG(14.0f), CONVERT_PaperUnit_to_InG(28.0f),
		0, 0, -CONVERT_PaperUnit_to_InG(14.0f));
	duckBBMesh.AddRef();
	static CCubeMesh keumWonBoBBMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(36.0f), CONVERT_PaperUnit_to_InG(18.0f), CONVERT_PaperUnit_to_InG(25.0f),
		0, 0, 0);
	keumWonBoBBMesh.AddRef();
	static CCubeMesh pencilCaseBBMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(27.0f), CONVERT_PaperUnit_to_InG(47.0f), CONVERT_PaperUnit_to_InG(24.0f),
		0, 0, -CONVERT_PaperUnit_to_InG(12.0f));
	pencilCaseBBMesh.AddRef();
	static CCubeMesh nailBBMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(42.0f), CONVERT_PaperUnit_to_InG(4.0f), CONVERT_PaperUnit_to_InG(16.0f),
		CONVERT_PaperUnit_to_InG(12.0f), 0, -CONVERT_PaperUnit_to_InG(8.0f));
	nailBBMesh.AddRef();
	static CCubeMesh blueHeadBBMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(130.0f), CONVERT_PaperUnit_to_InG(17.0f), CONVERT_PaperUnit_to_InG(130.0f),
		0, 0, 0);
	blueHeadBBMesh.AddRef();
	static CCubeMesh shortPencilBBMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(15.0f), CONVERT_PaperUnit_to_InG(7.5f), CONVERT_PaperUnit_to_InG(50.0f),
		0, 0, -CONVERT_PaperUnit_to_InG(25.0f));
	shortPencilBBMesh.AddRef();
	static CCubeMesh longPencilBBMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(18.0f), CONVERT_PaperUnit_to_InG(9.0f), CONVERT_PaperUnit_to_InG(90.0f),
		0, 0, -CONVERT_PaperUnit_to_InG(45.0f));
	longPencilBBMesh.AddRef();
	static CCubeMesh cupBBMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(26.0f), CONVERT_PaperUnit_to_InG(13.0f), CONVERT_PaperUnit_to_InG(36.0f),
		0, 0, -CONVERT_PaperUnit_to_InG(18.0f));
	cupBBMesh.AddRef();
	static CCubeMesh redHeadBBMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(130.0f), CONVERT_PaperUnit_to_InG(17.0f), CONVERT_PaperUnit_to_InG(100.0f),
		0, 0, -CONVERT_PaperUnit_to_InG(10.0f));
	redHeadBBMesh.AddRef();
	static CCubeMesh penCapBBMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(16.0f), CONVERT_PaperUnit_to_InG(30.0f), CONVERT_PaperUnit_to_InG(16.0f),
		0, CONVERT_PaperUnit_to_InG(30.0f), 0);
	penCapBBMesh.AddRef();
	static CCubeMesh penBBMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(90.0f), CONVERT_PaperUnit_to_InG(9.0f), CONVERT_PaperUnit_to_InG(18.0f),
		CONVERT_PaperUnit_to_InG(45.0f), 0, -CONVERT_PaperUnit_to_InG(9.0f));
	penBBMesh.AddRef();
	static CCubeMesh diceBBMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(25.0f), CONVERT_PaperUnit_to_InG(12.5f), CONVERT_PaperUnit_to_InG(25.0f),
		0, 0, 0);
	diceBBMesh.AddRef();
	static CCubeMesh book1BBMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(74.0f), CONVERT_PaperUnit_to_InG(3.75f), CONVERT_PaperUnit_to_InG(96.0f),
		0, 0, -CONVERT_PaperUnit_to_InG(48.0f));
	book1BBMesh.AddRef();
	static CCubeMesh book2BBMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(54.0f), CONVERT_PaperUnit_to_InG(3.0f), CONVERT_PaperUnit_to_InG(65.0f),
		0, 0, -CONVERT_PaperUnit_to_InG(32.5f));
	book2BBMesh.AddRef();
	static CCubeMesh book3BBMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(54.0f), CONVERT_PaperUnit_to_InG(9.0f), CONVERT_PaperUnit_to_InG(65.0f),
		0, 0, -CONVERT_PaperUnit_to_InG(32.5f));
	book3BBMesh.AddRef();
	static CCubeMesh book4BBMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(54.0f), CONVERT_PaperUnit_to_InG(6.0f), CONVERT_PaperUnit_to_InG(65.0f),
		0, 0, -CONVERT_PaperUnit_to_InG(32.5f));
	book4BBMesh.AddRef();

	switch (index)
	{
	case 0: // 지우개
		target->SetBoundingMesh(&eraserBBMesh);
		break;
	case 1: // 오리
		target->SetBoundingMesh(&duckBBMesh);
		break;
	case 2: // 금원보
		target->SetBoundingMesh(&keumWonBoBBMesh);
		break;
	case 3: // 필통
		target->SetBoundingMesh(&pencilCaseBBMesh);
		break;
	case 4: // 손톱 깎이
		target->SetBoundingMesh(&nailBBMesh);
		break;
	case 5: // 블루 헤드셋
		target->SetBoundingMesh(&blueHeadBBMesh);
		break;
	case 6: // 짧은 연필
		target->SetBoundingMesh(&shortPencilBBMesh);
		break;
	case 7: // 긴 연필
		target->SetBoundingMesh(&longPencilBBMesh);
		break;
	case 8: // 종이 컵
		target->SetBoundingMesh(&cupBBMesh);
		break;
	case 9: // 레드 헤드셋
		target->SetBoundingMesh(&redHeadBBMesh);
		break;
	case 10: // 펜 뚜껑
		target->SetBoundingMesh(&penCapBBMesh);
		break;
	case 11: // 펜 본체
		target->SetBoundingMesh(&penBBMesh);
		break;
	case 12: // 주사위
		target->SetBoundingMesh(&diceBBMesh);
		break;
	case 13: // 책 1
		target->SetBoundingMesh(&book1BBMesh);
		break;
	case 14: // 책 2
		target->SetBoundingMesh(&book2BBMesh);
		break;
	case 15: // 책 3
		target->SetBoundingMesh(&book3BBMesh);
		break;
	case 16: // 책 4
		target->SetBoundingMesh(&book4BBMesh);
		break;
	}
}
