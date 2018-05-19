#include "stdafx.h"
#include "StaticObjectShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"
#include "05.Objects/07.StaticObjects/Obstacle.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"

/// <summary>
/// 목적: 스테틱 오브젝트 그리기 용도의 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-05-19
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CStaticObjectShader::CStaticObjectShader(CCreateMgr *pCreateMgr)
	: CShader(pCreateMgr)
{
}

CStaticObjectShader::~CStaticObjectShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CStaticObjectShader::ReleaseUploadBuffers()
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
		for (int i = 0; i < m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
#endif
}

void CStaticObjectShader::UpdateShaderVariables()
{
#if USE_INSTANCING
	m_pCommandList->SetGraphicsRootShaderResourceView(2,
		m_pInstanceBuffer->GetGPUVirtualAddress());

	for (int i = 0; i < m_nObjects; i++)
	{
		XMStoreFloat4x4(&m_pMappedObjects[i].m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
#else
	static UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
#endif
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

void CStaticObjectShader::RenderBoundingBox(CCamera * pCamera)
{
	CShader::RenderBoundingBox(pCamera);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]) m_ppObjects[j]->RenderBoundingBox(pCamera);
	}
}

bool CStaticObjectShader::OnProcessKeyInput(UCHAR* pKeyBuffer)
{
	//if (pKeyBuffer['U'] & 0xF0)
	//{
	//	R -= 0.1f;
	//	if (R < 0.0f) R = 0.0f;
	//	m_pMaterial->SetRoughness(R);
	//	return true;
	//}

	return true;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CStaticObjectShader::CreateInputLayout()
{
	UINT nInputElementDescs = 4;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pd3dInputElementDescs[1] = {
		"NORMAL",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pd3dInputElementDescs[2] = {
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		24,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pd3dInputElementDescs[3] = {
		"TANGENT",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		32,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CStaticObjectShader::CreateVertexShader(ID3DBlob **ppShaderBlob)
{
#if USE_INSTANCING
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "VSTexturedLightingInstancing", "vs_5_1", ppShaderBlob));
#else
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "VSTexturedLighting", "vs_5_1", ppShaderBlob));
#endif
}

D3D12_SHADER_BYTECODE CStaticObjectShader::CreatePixelShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "PSTexturedLightingDetail", "ps_5_1", ppShaderBlob));
}

void CStaticObjectShader::CreateShader(CCreateMgr *pCreateMgr, UINT nRenderTargets)
{
	m_nPipelineStates = 2;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	for (int i = 0; i < m_nPipelineStates; ++i)
	{
		m_ppPipelineStates[i] = NULL;
	}

	m_nHeaps = 18;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets);
	CShader::CreateBoundingBoxShader(pCreateMgr, nRenderTargets);
}

void CStaticObjectShader::CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers)
{
	HRESULT hResult;

#if USE_INSTANCING
	m_pInstanceBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		sizeof(CB_GAMEOBJECT_INFO) * nBuffers,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL);

	hResult = m_pInstanceBuffer->Map(0, NULL, (void **)&m_pMappedObjects);
	assert(SUCCEEDED(hResult) && "m_pInstanceBuffer->Map Failed");
#else
	UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	m_pConstBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		elementBytes * nBuffers,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	hResult = m_pConstBuffer->Map(0, NULL, (void **)&m_pMappedObjects);
	assert(SUCCEEDED(hResult) && "m_pConstBuffer->Map Failed");
#endif

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

void CStaticObjectShader::BuildObjects(CCreateMgr *pCreateMgr, void *pContext)
{
	if (pContext) m_pTerrain = (CHeightMapTerrain*)pContext;

	CTransformImporter transformInporter;

	transformInporter.LoadMeshData("Resource//Data//ObjectSetting.txt");

	m_nObjects = transformInporter.m_iTotalCnt;
	m_ppObjects = new CBaseObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateShaderVariables(pCreateMgr, m_nObjects);
	for (int i = 0; i < m_nHeaps - 1; ++i)
	{
		CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 4, i);
		CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pConstBuffer, ncbElementBytes, i);
	}
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 0, m_nHeaps - 1);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pBoundingBoxBuffer, ncbElementBytes, m_nHeaps - 1);

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
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(13)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(26.5), CONVERT_PaperUnit_to_InG(7), CONVERT_PaperUnit_to_InG(13)));

	pMeshes[1] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//Duck.meshinfo");
	pMeshes[1]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(14)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(14), CONVERT_PaperUnit_to_InG(14), CONVERT_PaperUnit_to_InG(14)));

	pMeshes[2] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//KeumWonBo.meshinfo");
	pMeshes[2]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(18), CONVERT_PaperUnit_to_InG(18), CONVERT_PaperUnit_to_InG(12.5)));

	pMeshes[3] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//PencilCase.meshinfo");//***
	pMeshes[3]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(12)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(13.5), CONVERT_PaperUnit_to_InG(47), CONVERT_PaperUnit_to_InG(12)));

	pMeshes[4] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//Nail.meshinfo");//***
	pMeshes[4]->SetBoundingBox(
		XMFLOAT3(CONVERT_PaperUnit_to_InG(12), 0.0f, -CONVERT_PaperUnit_to_InG(8)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(21), CONVERT_PaperUnit_to_InG(3), CONVERT_PaperUnit_to_InG(8)));

	pMeshes[5] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//BlueHead.meshinfo");//***
	pMeshes[5]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(65), CONVERT_PaperUnit_to_InG(17), CONVERT_PaperUnit_to_InG(65)));

	pMeshes[6] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//ShortPencil.meshinfo");
	pMeshes[6]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(25)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(7.5), CONVERT_PaperUnit_to_InG(7.5), CONVERT_PaperUnit_to_InG(25)));

	pMeshes[7] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//LongPencil.meshinfo");
	pMeshes[7]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(45)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(9), CONVERT_PaperUnit_to_InG(9), CONVERT_PaperUnit_to_InG(45)));

	pMeshes[8] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//Cup.meshinfo");
	pMeshes[8]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(18)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(13), CONVERT_PaperUnit_to_InG(13), CONVERT_PaperUnit_to_InG(18)));

	pMeshes[9] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//RedHead.meshinfo");
	pMeshes[9]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(10)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(65), CONVERT_PaperUnit_to_InG(17), CONVERT_PaperUnit_to_InG(50)));

	pMeshes[10] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//PenCover.meshinfo");
	pMeshes[10]->SetBoundingBox(
		XMFLOAT3(0.0f, CONVERT_PaperUnit_to_InG(30), 0.0f),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(8), CONVERT_PaperUnit_to_InG(30), CONVERT_PaperUnit_to_InG(8)));

	pMeshes[11] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//Pen.meshinfo");
	pMeshes[11]->SetBoundingBox(
		XMFLOAT3(CONVERT_PaperUnit_to_InG(45), 0.0f, -CONVERT_PaperUnit_to_InG(9)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(45), CONVERT_PaperUnit_to_InG(9), CONVERT_PaperUnit_to_InG(9)));

	pMeshes[12] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//Dice.meshinfo");
	pMeshes[12]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(12.5), CONVERT_PaperUnit_to_InG(12.5), CONVERT_PaperUnit_to_InG(12.5)));

	pMeshes[13] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//Book.meshinfo", transformInporter.BookScale[0]);
	pMeshes[13]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(48)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(37), CONVERT_PaperUnit_to_InG(3.75), CONVERT_PaperUnit_to_InG(96)));

	pMeshes[14] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//Book.meshinfo", transformInporter.BookScale[1]);
	pMeshes[14]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(32.5)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(27), CONVERT_PaperUnit_to_InG(3), CONVERT_PaperUnit_to_InG(32.5)));

	pMeshes[15] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//Book.meshinfo", transformInporter.BookScale[2]);
	pMeshes[15]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(32.5)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(27), CONVERT_PaperUnit_to_InG(9), CONVERT_PaperUnit_to_InG(32.5)));

	pMeshes[16] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//Book.meshinfo", transformInporter.BookScale[3]);
	pMeshes[16]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(32.5)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(27), CONVERT_PaperUnit_to_InG(6), CONVERT_PaperUnit_to_InG(32.5)));

	int cnt = 0;
	for (int i = 0; i < m_nMaterials; ++i) {
		m_meshCounts[i] = transformInporter.m_iKindMeshCnt[i];
		for (int j = 0; j < transformInporter.m_iKindMeshCnt[i]; ++j) {
			XMFLOAT3 pos = transformInporter.m_Transform[cnt].pos;
			XMFLOAT3 rot = transformInporter.m_Transform[cnt].rotation;
			m_ppObjects[cnt] = new CObstacle(pCreateMgr);
			m_ppObjects[cnt]->SetPosition(CONVERT_Unit_to_InG(pos.x), CONVERT_Unit_to_InG(pos.y), CONVERT_Unit_to_InG(pos.z));

			m_ppObjects[cnt]->Rotate(0, 180, 0);
			m_ppObjects[cnt]->Rotate(-rot.x, rot.y, -rot.z);

			m_ppObjects[cnt]->SetMesh(0, pMeshes[i]);
			SetBoundingBoxMeshByIndex(pCreateMgr, m_ppObjects[cnt], i);

			m_ppObjects[cnt]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * cnt));
			m_ppObjects[cnt]->SetCbvGPUDescriptorHandlePtrForBB(m_pcbvGPUDescriptorStartHandle[m_nHeaps - 1].ptr + (incrementSize * cnt));
			++cnt;
		}
	}
}

void CStaticObjectShader::ReleaseObjects()
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

void CStaticObjectShader::SetBoundingBoxMeshByIndex(CCreateMgr *pCreateMgr, CBaseObject * target, int index)
{
	switch (index)
	{
	case 0: // 지우개
		target->SetBoundingMesh(pCreateMgr,
			CONVERT_PaperUnit_to_InG(53), CONVERT_PaperUnit_to_InG(14), CONVERT_PaperUnit_to_InG(26),
			0, 0, -CONVERT_PaperUnit_to_InG(13));
		break;
	case 1: // 오리
		target->SetBoundingMesh(pCreateMgr,
			CONVERT_PaperUnit_to_InG(28), CONVERT_PaperUnit_to_InG(28), CONVERT_PaperUnit_to_InG(28),
			0, 0, -CONVERT_PaperUnit_to_InG(14));
		break;
	case 2: // 금원보
		target->SetBoundingMesh(pCreateMgr,
			CONVERT_PaperUnit_to_InG(36), CONVERT_PaperUnit_to_InG(36), CONVERT_PaperUnit_to_InG(25),
			0, 0, 0);
		break;
	case 3: // 필통
		target->SetBoundingMesh(pCreateMgr,
			CONVERT_PaperUnit_to_InG(27), CONVERT_PaperUnit_to_InG(94), CONVERT_PaperUnit_to_InG(24),
			0, 0, -CONVERT_PaperUnit_to_InG(12));
		break;
	case 4: // 손톱 깎이
		target->SetBoundingMesh(pCreateMgr,
			CONVERT_PaperUnit_to_InG(42), CONVERT_PaperUnit_to_InG(8), CONVERT_PaperUnit_to_InG(16),
			CONVERT_PaperUnit_to_InG(12), 0, -CONVERT_PaperUnit_to_InG(8));
		break;
	case 5: // 블루 헤드셋
		target->SetBoundingMesh(pCreateMgr,
			CONVERT_PaperUnit_to_InG(130), CONVERT_PaperUnit_to_InG(34), CONVERT_PaperUnit_to_InG(130),
			0, 0, 0);
		break;
	case 6: // 짧은 연필
		target->SetBoundingMesh(pCreateMgr,
			CONVERT_PaperUnit_to_InG(15), CONVERT_PaperUnit_to_InG(15), CONVERT_PaperUnit_to_InG(50),
			0, 0, -CONVERT_PaperUnit_to_InG(25));
		break;
	case 7: // 긴 연필
		target->SetBoundingMesh(pCreateMgr,
			CONVERT_PaperUnit_to_InG(18), CONVERT_PaperUnit_to_InG(18), CONVERT_PaperUnit_to_InG(90),
			0, 0, -CONVERT_PaperUnit_to_InG(45));
		break;
	case 8: // 종이 컵
		target->SetBoundingMesh(pCreateMgr,
			CONVERT_PaperUnit_to_InG(26), CONVERT_PaperUnit_to_InG(26), CONVERT_PaperUnit_to_InG(36),
			0, 0, -CONVERT_PaperUnit_to_InG(18));
		break;
	case 9: // 레드 헤드셋
		target->SetBoundingMesh(pCreateMgr,
			CONVERT_PaperUnit_to_InG(130), CONVERT_PaperUnit_to_InG(34), CONVERT_PaperUnit_to_InG(100),
			0, 0, -CONVERT_PaperUnit_to_InG(10));
		break;
	case 10: // 펜 뚜껑
		target->SetBoundingMesh(pCreateMgr,
			CONVERT_PaperUnit_to_InG(16), CONVERT_PaperUnit_to_InG(60), CONVERT_PaperUnit_to_InG(16),
			0, CONVERT_PaperUnit_to_InG(30), 0);
		break;
	case 11: // 펜 본체
		target->SetBoundingMesh(pCreateMgr,
			CONVERT_PaperUnit_to_InG(90), CONVERT_PaperUnit_to_InG(18), CONVERT_PaperUnit_to_InG(18),
			CONVERT_PaperUnit_to_InG(45), 0, -CONVERT_PaperUnit_to_InG(9));
		break;
	case 12: // 주사위
		target->SetBoundingMesh(pCreateMgr,
			CONVERT_PaperUnit_to_InG(25), CONVERT_PaperUnit_to_InG(25), CONVERT_PaperUnit_to_InG(25),
			0, 0, 0);
		break;
	case 13: // 책 1
		target->SetBoundingMesh(pCreateMgr,
			CONVERT_PaperUnit_to_InG(74), CONVERT_PaperUnit_to_InG(7.5), CONVERT_PaperUnit_to_InG(96),
			0, 0, -CONVERT_PaperUnit_to_InG(48));
		break;
	case 14: // 책 2
		target->SetBoundingMesh(pCreateMgr,
			CONVERT_PaperUnit_to_InG(54), CONVERT_PaperUnit_to_InG(6), CONVERT_PaperUnit_to_InG(65),
			0, 0, -CONVERT_PaperUnit_to_InG(32.5));
		break;
	case 15: // 책 3
		target->SetBoundingMesh(pCreateMgr,
			CONVERT_PaperUnit_to_InG(54), CONVERT_PaperUnit_to_InG(18), CONVERT_PaperUnit_to_InG(65),
			0, 0, -CONVERT_PaperUnit_to_InG(32.5));
		break;
	case 16: // 책 4
		target->SetBoundingMesh(pCreateMgr,
			CONVERT_PaperUnit_to_InG(54), CONVERT_PaperUnit_to_InG(12), CONVERT_PaperUnit_to_InG(65),
			0, 0, -CONVERT_PaperUnit_to_InG(32.5));
		break;
	}
}
