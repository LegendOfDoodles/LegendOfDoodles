#include "stdafx.h"
#include "AniShader.h"
#include "05.Objects/06.Minion/Minion.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"
#include "05.Objects/99.Material/Material.h"
#include "00.Global/01.Utility/06.HPGaugeManager/HPGaugeManager.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"

/// <summary>
/// 목적: 움직이는 오브젝트 관리 및 그리기 용도
/// 최종 수정자:  김나단
/// 수정자 목록:  정휘현, 김나단
/// 최종 수정 날짜: 2018-06-27
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CAniShader::CAniShader(CCreateMgr *pCreateMgr, Network* pNetwork) : CShader(pCreateMgr)
{
	m_pCreateMgr = pCreateMgr;
	m_pNetwork = pNetwork;
}

CAniShader::~CAniShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CAniShader::Initialize(CCreateMgr *pCreateMgr, void *pContext)
{
	CreateShader(pCreateMgr, RENDER_TARGET_BUFFER_CNT, true);
	BuildObjects(pCreateMgr, pContext);
}

void CAniShader::ReleaseUploadBuffers()
{
	if (m_ppBlues)
	{
		for (int j = 0; j < MAX_MINION; j++)
		{
			m_ppBlues[j]->ReleaseUploadBuffers();
		}
	}

	if (m_ppReds)
	{
		for (int j = 0; j < MAX_MINION; j++)
		{
			m_ppReds[j]->ReleaseUploadBuffers();
		}
	}

	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
	m_buildFinished = true;
}

void CAniShader::UpdateShaderVariables()
{
	static UINT elementBytes = ((sizeof(CB_ANIOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nBlues; i++)
	{
		CB_ANIOBJECT_INFO *pMappedObject = (CB_ANIOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		XMFLOAT4X4 tmp[128];
		memcpy(tmp, m_ppBlues[i]->GetFrameMatrix(), sizeof(XMFLOAT4X4) * 128);
		memcpy(pMappedObject->m_xmf4x4Frame, tmp, sizeof(XMFLOAT4X4) * 128);

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World0,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppBlues[i]->GetWorldMatrix())));
	}

	for (int i = 0; i < m_nReds; i++)
	{
		CB_ANIOBJECT_INFO *pMappedObject = (CB_ANIOBJECT_INFO *)(m_pMappedObjects + ((MAX_MINION + i) * elementBytes));
		XMFLOAT4X4 tmp[128];
		memcpy(tmp, m_ppReds[i]->GetFrameMatrix(), sizeof(XMFLOAT4X4) * 128);
		memcpy(pMappedObject->m_xmf4x4Frame, tmp, sizeof(XMFLOAT4X4) * 128);

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World0,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppReds[i]->GetWorldMatrix())));
	}
}

void CAniShader::UpdateBoundingBoxShaderVariables()
{
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nBlues; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedBoundingBoxes + (i * boundingBoxElementBytes));

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppBlues[i]->GetWorldMatrix())));
	}

	for (int i = 0; i < m_nReds; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedBoundingBoxes + ((MAX_MINION + i) * boundingBoxElementBytes));

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppReds[i]->GetWorldMatrix())));
	}
}

void CAniShader::AnimateObjects(float timeElapsed)
{
	if (m_buildFinished)
	{
		m_pNetwork->ReadPacket(m_pNetwork->m_mysocket);
		if (m_nBlues > 0) m_pNetwork->ReadPacket(m_pNetwork->m_mysocket);
		m_pNetwork->ReadPacket(m_pNetwork->m_mysocket);
		if (m_nReds > 0) m_pNetwork->ReadPacket(m_pNetwork->m_mysocket);
	}

	for (int i = 0; i < m_nBlues; i++)
	{
		m_ppBlues[i]->Animate(timeElapsed);
	}

	for (int i = 0; i < m_nReds; i++)
	{
		m_ppReds[i]->Animate(timeElapsed);
	}
}

void CAniShader::Render(CCamera *pCamera)
{
	CShader::Render(pCamera);

	if (m_ppMaterials) m_ppMaterials[0]->UpdateShaderVariables();
	for (int i = 0; i < m_nBlues; i++)
	{
		m_ppBlues[i]->Render(pCamera);
	}

	if (m_ppMaterials) m_ppMaterials[1]->UpdateShaderVariables();
	for (int i = 0; i < m_nReds; i++)
	{
		m_ppReds[i]->Render(pCamera);
	}
}

void CAniShader::RenderBoundingBox(CCamera * pCamera)
{
	CShader::RenderBoundingBox(pCamera);

	for (int i = 0; i < m_nBlues; i++)
	{
		m_ppBlues[i]->RenderBoundingBox(pCamera);
	}

	for (int i = 0; i < m_nReds; i++)
	{
		m_ppReds[i]->RenderBoundingBox(pCamera);
	}
}

CBaseObject *CAniShader::PickObjectByRayIntersection(
	XMFLOAT3& pickPosition, XMFLOAT4X4& xmf4x4View, float &nearHitDistance)
{
	bool intersected = 0;

	nearHitDistance = FLT_MAX;
	float hitDistance = FLT_MAX;
	CBaseObject *pSelectedObject{ NULL };
	
	for (int i = 0; i < m_nBlues; i++)
	{
		intersected = m_ppBlues[i]->PickObjectByRayIntersection(pickPosition, xmf4x4View, hitDistance);
		if (intersected && (hitDistance < nearHitDistance))
		{
			nearHitDistance = hitDistance;
			pSelectedObject = m_ppBlues[i];
		}
	}

	for (int i = 0; i < m_nReds; i++)
	{
		intersected = m_ppReds[i]->PickObjectByRayIntersection(pickPosition, xmf4x4View, hitDistance);
		if (intersected && (hitDistance < nearHitDistance))
		{
			nearHitDistance = hitDistance;
			pSelectedObject = m_ppReds[i];
		}
	}

	return(pSelectedObject);
}

bool CAniShader::OnProcessKeyInput(UCHAR* pKeyBuffer)
{
	static float R = 0.0f;
	static float M = 0.0f;

	if (GetAsyncKeyState('M') & 0x0001)
	{
		m_nWeaponState++;
		if (m_nWeaponState >= 3)m_nWeaponState = 0;

	}

	return true;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CAniShader::CreateInputLayout()
{
	UINT nInputElementDescs = 6;
	D3D12_INPUT_ELEMENT_DESC *pInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	UINT cnt = 0;
	pInputElementDescs[0] = {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[1] = {
		"NORMAL",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[2] = {
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	pInputElementDescs[3] = {
		"WEIGHTS",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[4] = {
		"BONEINDICES",
		0,
		DXGI_FORMAT_R8G8B8A8_UINT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	pInputElementDescs[5] = {
		"TANGENT",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CAniShader::CreateVertexShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "VSBone", "vs_5_1", ppShaderBlob));
}

D3D12_SHADER_BYTECODE CAniShader::CreatePixelShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "PSBone", "ps_5_1", ppShaderBlob));
}

void CAniShader::CreateShader(CCreateMgr *pCreateMgr, UINT nRenderTargets, bool isRenderBB)
{
	m_nPipelineStates = 2;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	for (int i = 0; i < m_nPipelineStates; ++i)
	{
		m_ppPipelineStates[i] = NULL;
	}

	m_nHeaps = 3;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB);
}

void CAniShader::CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers)
{
	HRESULT hResult;

	UINT elementBytes = ((sizeof(CB_ANIOBJECT_INFO) + 255) & ~255);

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

void CAniShader::BuildObjects(CCreateMgr *pCreateMgr, void *pContext)
{
	if (pContext) m_pTerrain = (CHeightMapTerrain*)pContext;

	UINT ncbElementBytes = ((sizeof(CB_ANIOBJECT_INFO) + 255) & ~255);
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateShaderVariables(pCreateMgr, MAX_MINION * 2);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, MAX_MINION * 2, 1);
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, MAX_MINION * 2, 1, 1);
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, MAX_MINION * 2, 0, 2);

	CreateConstantBufferViews(pCreateMgr, MAX_MINION * 2, m_pConstBuffer, ncbElementBytes, 0);
	CreateConstantBufferViews(pCreateMgr, MAX_MINION * 2, m_pConstBuffer, ncbElementBytes, 1);
	CreateConstantBufferViews(pCreateMgr, MAX_MINION * 2, m_pBoundingBoxBuffer, boundingBoxElementBytes, 2);

	SaveBoundingBoxHeapNumber(2);

#if USE_BATCH_MATERIAL
	m_nMaterials = 2;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	// Blue
	m_ppMaterials[0] = Materials::CreateMinionMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[0]->SetAlbedo(XMFLOAT4(0.6, 0.6, 1.0, 1.0));
	// Red
	m_ppMaterials[1] = Materials::CreateMinionMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);
	m_ppMaterials[1]->SetAlbedo(XMFLOAT4(1.0, 0.6, 0.6, 1.0));
#else
	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#endif

	m_pWeapons[0] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Minion//Weapon//Minion_Sword.meshinfo");
	m_pWeapons[1] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Minion//Weapon//Minion_Staff.meshinfo");
	m_pWeapons[2] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Minion//Weapon//Minion_Bow2.meshinfo");

	for (int j = 0; j < 3; ++j) { m_pWeapons[j]->AddRef(); }

	UINT incrementSize{ m_pCreateMgr->GetCbvSrvDescriptorIncrementSize() };

	CSkinnedMesh *pMinionMesh = new CSkinnedMesh(m_pCreateMgr, "Resource//3D//Minion//Mesh//Minion.meshinfo");

	CCubeMesh *pBoundingBoxMesh = new CCubeMesh(m_pCreateMgr,
		CONVERT_PaperUnit_to_InG(3), CONVERT_PaperUnit_to_InG(1.5), CONVERT_PaperUnit_to_InG(7),
		0, 0, -CONVERT_PaperUnit_to_InG(4));

	CSkeleton *pSIdle = new CSkeleton("Resource//3D//Minion//Animation//Sword//Minion_S_Idle.aniinfo");
	CSkeleton *pSAtk1 = new CSkeleton("Resource//3D//Minion//Animation//Sword//Minion_S_Attack1.aniinfo");
	CSkeleton *pSAtk2 = new CSkeleton("Resource//3D//Minion//Animation//Sword//Minion_S_Attack2.aniinfo");
	CSkeleton *pSWalkStart = new CSkeleton("Resource//3D//Minion//Animation//Sword//Minion_S_WalkStart.aniinfo");
	CSkeleton *pSWalk = new CSkeleton("Resource//3D//Minion//Animation//Sword//Minion_S_Walk.aniinfo");
					 
	CSkeleton *pBIdle = new CSkeleton("Resource//3D//Minion//Animation//Bow//Minion_B_Idle.aniinfo");
	CSkeleton *pBAtk = new CSkeleton("Resource//3D//Minion//Animation//Bow//Minion_B_Attack.aniinfo");
	CSkeleton *pBWalkStart = new CSkeleton("Resource//3D//Minion//Animation//Bow//Minion_B_WalkStart.aniinfo");
	CSkeleton *pBWalk = new CSkeleton("Resource//3D//Minion//Animation//Bow//Minion_B_Walk.aniinfo");
					 
	CSkeleton *pMIdle = new CSkeleton("Resource//3D//Minion//Animation//Magic//Minion_M_Idle.aniinfo");
	CSkeleton *pMAtk1 = new CSkeleton("Resource//3D//Minion//Animation//Magic//Minion_M_Attack1.aniinfo");
	CSkeleton *pMAtk2 = new CSkeleton("Resource//3D//Minion//Animation//Magic//Minion_M_Attack2.aniinfo");
	CSkeleton *pMWalkStart = new CSkeleton("Resource//3D//Minion//Animation//Magic//Minion_M_WalkStart.aniinfo");
	CSkeleton *pMWalk = new CSkeleton("Resource//3D//Minion//Animation//Magic//Minion_M_Walk.aniinfo");
					 
	CSkeleton *pDie = new CSkeleton("Resource//3D//Minion//Animation//Minion_Die.aniinfo");

	pMinionMesh->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(4)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(1.5), CONVERT_PaperUnit_to_InG(1.5), CONVERT_PaperUnit_to_InG(3.5)));

	m_ppBlues = new CBaseObject*[MAX_MINION];
	for (int i = 0; i < MAX_MINION; ++i)
	{
		CMinion *pMinionObject{ NULL };

		switch (m_kind)
		{
		case ObjectType::SwordMinion:
			pMinionObject = new CSwordMinion(m_pCreateMgr, 2);
			break;
		case ObjectType::StaffMinion:
			pMinionObject = new CMagicMinion(m_pCreateMgr, 2);
			break;
		case ObjectType::BowMinion:
			pMinionObject = new CBowMinion(m_pCreateMgr, 2);
			break;
		}

		pMinionObject->SetMesh(0, pMinionMesh);

		switch (m_kind)
		{
		case ObjectType::SwordMinion:
			pMinionObject->SetMesh(1, m_pWeapons[0]);
			break;
		case ObjectType::StaffMinion:
			pMinionObject->SetMesh(1, m_pWeapons[1]);
			break;
		case ObjectType::BowMinion:
			pMinionObject->SetMesh(1, m_pWeapons[2]);
			break;
		}

		pMinionObject->SetBoundingMesh(pBoundingBoxMesh);

		switch (m_kind)
		{
		case ObjectType::SwordMinion:
			pMinionObject->SetSkeleton(pSIdle);
			pMinionObject->SetSkeleton(pSAtk1);
			pMinionObject->SetSkeleton(pSAtk2);
			pMinionObject->SetSkeleton(pSWalkStart);
			pMinionObject->SetSkeleton(pSWalk);
			break;										
		case ObjectType::StaffMinion:		
			pMinionObject->SetSkeleton(pMIdle);
			pMinionObject->SetSkeleton(pMAtk1);
			pMinionObject->SetSkeleton(pMAtk2);
			pMinionObject->SetSkeleton(pMWalkStart);
			pMinionObject->SetSkeleton(pMWalk);
			break;										   
		case ObjectType::BowMinion:		   
			pMinionObject->SetSkeleton(pBIdle);
			pMinionObject->SetSkeleton(pBAtk);
			pMinionObject->SetSkeleton(pBWalkStart);
			pMinionObject->SetSkeleton(pBWalk);
			break;
		}

		pMinionObject->SetSkeleton(pDie);

		pMinionObject->SetTerrain(m_pTerrain);

		pMinionObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * i));
		pMinionObject->SetCbvGPUDescriptorHandlePtrForBB(m_pcbvGPUDescriptorStartHandle[1].ptr + (incrementSize * i));

		m_ppBlues[i] = pMinionObject;
		m_pGaugeManger->AddMinionObject(pMinionObject);
	}
	m_pGaugeManger->SetBlueMinionCnt(&m_nBlues);

	m_ppReds = new CBaseObject*[MAX_MINION];
	for (int i = 0; i < MAX_MINION; ++i)
	{
		CMinion *pMinionObject{ NULL };

		switch (m_kind)
		{
		case ObjectType::SwordMinion:
			pMinionObject = new CSwordMinion(m_pCreateMgr, 2);
			break;
		case ObjectType::StaffMinion:
			pMinionObject = new CMagicMinion(m_pCreateMgr, 2);
			break;
		case ObjectType::BowMinion:
			pMinionObject = new CBowMinion(m_pCreateMgr, 2);
			break;
		}

		pMinionObject->SetMesh(0, pMinionMesh);

		switch (m_kind)
		{
		case ObjectType::SwordMinion:
			pMinionObject->SetMesh(1, m_pWeapons[0]);
			break;
		case ObjectType::StaffMinion:
			pMinionObject->SetMesh(1, m_pWeapons[1]);
			break;
		case ObjectType::BowMinion:
			pMinionObject->SetMesh(1, m_pWeapons[2]);
			break;
		}

		switch (m_kind)
		{
		case ObjectType::SwordMinion:
			pMinionObject->SetSkeleton(pSIdle);
			pMinionObject->SetSkeleton(pSAtk1);
			pMinionObject->SetSkeleton(pSAtk2);
			pMinionObject->SetSkeleton(pSWalkStart);
			pMinionObject->SetSkeleton(pSWalk);
			break;
		case ObjectType::StaffMinion:
			pMinionObject->SetSkeleton(pMIdle);
			pMinionObject->SetSkeleton(pMAtk1);
			pMinionObject->SetSkeleton(pMAtk2);
			pMinionObject->SetSkeleton(pMWalkStart);
			pMinionObject->SetSkeleton(pMWalk);
			break;
		case ObjectType::BowMinion:
			pMinionObject->SetSkeleton(pBIdle);
			pMinionObject->SetSkeleton(pBAtk);
			pMinionObject->SetSkeleton(pBWalkStart);
			pMinionObject->SetSkeleton(pBWalk);
			break;
		}
		pMinionObject->SetSkeleton(pDie);

		pMinionObject->SetTerrain(m_pTerrain);
		pMinionObject->Rotate(90, 0, 0);

		pMinionObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * (MAX_MINION + i)));
		pMinionObject->SetCbvGPUDescriptorHandlePtrForBB(m_pcbvGPUDescriptorStartHandle[2].ptr + (incrementSize * (MAX_MINION + i)));

		m_ppReds[i] = pMinionObject;
		m_pGaugeManger->AddMinionObject(pMinionObject);
	}
	m_pGaugeManger->SetRedMinionCnt(&m_nReds);

	m_pGaugeManger->StartSpawn();

	m_pNetwork->SetBlueMinions(m_ppBlues);
	m_pNetwork->SetRedMinions(m_ppReds);
	m_pNetwork->SetBlueCount(&m_nBlues);
	m_pNetwork->SetRedCount(&m_nReds);

	Safe_Delete(pSIdle);
	Safe_Delete(pSAtk1);
	Safe_Delete(pSAtk2);
	Safe_Delete(pSWalkStart);
	Safe_Delete(pSWalk);

	Safe_Delete(pBIdle);
	Safe_Delete(pBAtk);
	Safe_Delete(pBWalkStart);
	Safe_Delete(pBWalk);

	Safe_Delete(pMIdle);
	Safe_Delete(pMAtk1);
	Safe_Delete(pMAtk2);
	Safe_Delete(pMWalkStart);
	Safe_Delete(pMWalk);

	Safe_Delete(pDie);
}

void CAniShader::ReleaseObjects()
{
	if (m_ppBlues)
	{
		for (int i = 0; i < m_nBlues; i++)
		{
			delete m_ppBlues[i];
		}
		Safe_Delete_Array(m_ppBlues);
	}

	if (m_ppReds)
	{
		for (int i = 0; i < m_nReds; i++)
		{
			delete m_ppReds[i];
		}
		Safe_Delete_Array(m_ppReds);
	}

	if (m_ppMaterials)
	{
		for (int i = 0; i < m_nMaterials; ++i)
		{
			if (m_ppMaterials[i]) delete m_ppMaterials[i];
		}
		Safe_Delete(m_ppMaterials);
	}
}
