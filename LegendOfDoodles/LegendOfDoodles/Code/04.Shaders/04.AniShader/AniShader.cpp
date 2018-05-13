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
/// 최종 수정 날짜: 2018-05-11
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
void CAniShader::ReleaseUploadBuffers()
{
#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
#endif
}

void CAniShader::UpdateShaderVariables()
{
	static UINT elementBytes = ((sizeof(CB_ANIOBJECT_INFO) + 255) & ~255);

	for (auto& iter = m_blueObjects.begin(); iter != m_blueObjects.end(); ++iter)
	{
		CB_ANIOBJECT_INFO *pMappedObject = (CB_ANIOBJECT_INFO *)(m_pMappedObjects + ((*iter)->GetIndex() * elementBytes));
		XMFLOAT4X4 tmp[128];
		memcpy(tmp, (*iter)->GetFrameMatrix(), sizeof(XMFLOAT4X4) * 128);
		memcpy(pMappedObject->m_xmf4x4Frame, tmp, sizeof(XMFLOAT4X4) * 128);

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World0,
			XMMatrixTranspose(XMLoadFloat4x4((*iter)->GetWorldMatrix())));
	}

	for (auto& iter = m_redObjects.begin(); iter != m_redObjects.end(); ++iter)
	{
		CB_ANIOBJECT_INFO *pMappedObject = (CB_ANIOBJECT_INFO *)(m_pMappedObjects + ((*iter)->GetIndex() * elementBytes));
		XMFLOAT4X4 tmp[128];
		memcpy(tmp, (*iter)->GetFrameMatrix(), sizeof(XMFLOAT4X4) * 128);
		memcpy(pMappedObject->m_xmf4x4Frame, tmp, sizeof(XMFLOAT4X4) * 128);

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World0,
			XMMatrixTranspose(XMLoadFloat4x4((*iter)->GetWorldMatrix())));
	}
}

void CAniShader::UpdateBoundingBoxShaderVariables()
{
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (auto& iter = m_blueObjects.begin(); iter != m_blueObjects.end(); ++iter)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedBoundingBoxes + ((*iter)->GetIndex() * boundingBoxElementBytes));

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4((*iter)->GetWorldMatrix())));
	}
	for (auto& iter = m_redObjects.begin(); iter != m_redObjects.end(); ++iter)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedBoundingBoxes + ((*iter)->GetIndex() * boundingBoxElementBytes));

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4((*iter)->GetWorldMatrix())));
	}
}

void CAniShader::AnimateObjects(float timeElapsed)
{
	m_spawnTime += timeElapsed;

	if (m_spawnTime >= 0.0f && m_spawnTime <= 5.0f)
	{
		bool spawned{ false };
		for (float time = m_spawnTime - m_preSpawnTime; time >= 0.25; time -= 0.25)
		{
			spawned = true;
			SpawnMinion();
		}
		if(spawned) m_preSpawnTime = m_spawnTime;
	}

	if (m_spawnTime >= 30.0f)
	{
		m_spawnTime -= 30.0f;
		m_preSpawnTime = -0.25f;
	}

	//for (auto& iter = m_blueObjects.begin(); iter != m_blueObjects.end();)
	//{
	//	if ((*iter)->GetState() == States::Remove)
	//	{
	//		CCollisionObject* temp{ *iter };
	//		ResetPossibleIndex(temp->GetIndex());
	//		Safe_Delete(temp);

	//		iter = m_blueObjects.erase(iter);
	//	}
	//	else
	//	{
	//		m_pFSMMgr->Update(timeElapsed, (*iter));
	//		++iter;
	//	}
	//}

	//for (auto& iter = m_redObjects.begin(); iter != m_redObjects.end();)
	//{
	//	if ((*iter)->GetState() == States::Remove)
	//	{
	//		CCollisionObject* temp{ *iter };
	//		ResetPossibleIndex(temp->GetIndex());
	//		Safe_Delete(temp);

	//		iter = m_redObjects.erase(iter);
	//	}
	//	else
	//	{
	//		m_pFSMMgr->Update(timeElapsed, (*iter));
	//		++iter;
	//	}
	//}
}

void CAniShader::Render(CCamera *pCamera)
{
	CShader::Render(pCamera);

	if (m_ppMaterials) m_ppMaterials[0]->UpdateShaderVariables();
	for (auto& iter = m_blueObjects.begin(); iter != m_blueObjects.end(); ++iter)
	{
		(*iter)->Render(pCamera);
	}
	if (m_ppMaterials) m_ppMaterials[1]->UpdateShaderVariables();
	for (auto& iter = m_redObjects.begin(); iter != m_redObjects.end(); ++iter)
	{
		(*iter)->Render(pCamera);
	}
}

void CAniShader::RenderBoundingBox(CCamera * pCamera)
{
	CShader::RenderBoundingBox(pCamera);
	for (auto& iter = m_blueObjects.begin(); iter != m_blueObjects.end(); ++iter)
	{
		(*iter)->RenderBoundingBox(pCamera);
	}
	for (auto& iter = m_redObjects.begin(); iter != m_redObjects.end(); ++iter)
	{
		(*iter)->RenderBoundingBox(pCamera);
	}
}

CBaseObject *CAniShader::PickObjectByRayIntersection(
	XMFLOAT3& pickPosition, XMFLOAT4X4& xmf4x4View, float &nearHitDistance)
{
	bool intersected = 0;

	nearHitDistance = FLT_MAX;
	float hitDistance = FLT_MAX;
	CCollisionObject *pSelectedObject{ NULL };

	for (auto& iter = m_blueObjects.begin(); iter != m_blueObjects.end(); ++iter)
	{
		intersected = (*iter)->PickObjectByRayIntersection(pickPosition, xmf4x4View, hitDistance);
		if (intersected && (hitDistance < nearHitDistance))
		{
			nearHitDistance = hitDistance;
			pSelectedObject = (*iter);
		}
	}

	for (auto& iter = m_redObjects.begin(); iter != m_redObjects.end(); ++iter)
	{
		intersected = (*iter)->PickObjectByRayIntersection(pickPosition, xmf4x4View, hitDistance);
		if (intersected && (hitDistance < nearHitDistance))
		{
			nearHitDistance = hitDistance;
			pSelectedObject = (*iter);
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
	if (GetAsyncKeyState('N') & 0x0001)
	{
		SpawnMinion();
	}

	return true;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CAniShader::CreateInputLayout()
{
	UINT nInputElementDescs = 6;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	UINT cnt = 0;
	pd3dInputElementDescs[0] = {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pd3dInputElementDescs[1] = {
		"NORMAL",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pd3dInputElementDescs[2] = {
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	pd3dInputElementDescs[3] = {
		"WEIGHTS",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pd3dInputElementDescs[4] = {
		"BONEINDICES",
		0,
		DXGI_FORMAT_R8G8B8A8_UINT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	pd3dInputElementDescs[5] = {
		"TANGENT",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CAniShader::CreateVertexShader(ID3DBlob **ppShaderBlob)
{
	//./Code/04.Shaders/99.GraphicsShader/
#if USE_INSTANCING
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "VSTexturedLightingInstancing", "vs_5_1", ppShaderBlob));
#else
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "VSBone", "vs_5_1", ppShaderBlob));
#endif
}

D3D12_SHADER_BYTECODE CAniShader::CreatePixelShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "PSBone", "ps_5_1", ppShaderBlob));
}

void CAniShader::CreateShader(CCreateMgr *pCreateMgr)
{
	m_nPipelineStates = 2;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	for (int i = 0; i < m_nPipelineStates; ++i)
	{
		m_ppPipelineStates[i] = NULL;
	}

	m_nHeaps = 3;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr);
	CShader::CreateBoundingBoxShader(pCreateMgr);
}

void CAniShader::CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers)
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
	UINT elementBytes = ((sizeof(CB_ANIOBJECT_INFO) + 255) & ~255);

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

void CAniShader::BuildObjects(CCreateMgr *pCreateMgr, void *pContext)
{
	if (pContext) m_pTerrain = (CHeightMapTerrain*)pContext;

#if USE_INSTANCING
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 0, 2);
	CreateShaderVariables(pCreateMgr);
#else
	UINT ncbElementBytes = ((sizeof(CB_ANIOBJECT_INFO) + 255) & ~255);
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateShaderVariables(pCreateMgr, MAX_MINION);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, MAX_MINION, 4);
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, MAX_MINION, 0, 1);
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, MAX_MINION, 4, 2);

	CreateConstantBufferViews(pCreateMgr, MAX_MINION, m_pConstBuffer, ncbElementBytes, 0);
	CreateConstantBufferViews(pCreateMgr, MAX_MINION, m_pBoundingBoxBuffer, boundingBoxElementBytes, 1);
	CreateConstantBufferViews(pCreateMgr, MAX_MINION, m_pConstBuffer, ncbElementBytes, 2);

#endif

#if USE_BATCH_MATERIAL
	m_nMaterials = 2;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	// Blue
	m_ppMaterials[0] = Materials::CreateMinionMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[0]->SetAlbedo(XMFLOAT4(0.6, 0.6, 1.0, 1.0));
	// Red
	m_ppMaterials[1] = Materials::CreateMinionMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[2], &m_psrvGPUDescriptorStartHandle[2]);
	m_ppMaterials[1]->SetAlbedo(XMFLOAT4(1.0, 0.6, 0.6, 1.0));
#else
	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#endif

	m_pWeapons[0] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Minion//Weapon//Minion_Sword.meshinfo");
	m_pWeapons[1] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Minion//Weapon//Minion_Staff.meshinfo");
	m_pWeapons[2] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Minion//Weapon//Minion_Bow2.meshinfo");

	for (int j = 0; j < 3; ++j) {
		m_pWeapons[j]->AddRef();
	}

	SpawnMinion();
}

void CAniShader::ReleaseObjects()
{
	for (auto& iter = m_blueObjects.begin(); iter != m_blueObjects.end();)
	{
		iter = m_blueObjects.erase(iter);
	}
	m_blueObjects.clear();
	for (auto& iter = m_redObjects.begin(); iter != m_redObjects.end(); )
	{
		iter = m_redObjects.erase(iter);
	}
	m_redObjects.clear();

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

int CAniShader::GetPossibleIndex()
{
	for (int idx = 0; idx < MAX_MINION; ++idx)
	{
		if (!m_indexArr[idx])
		{
			m_indexArr[idx] = true;
			return idx;
		}
	}
	return NONE;
}

void CAniShader::SpawnMinion()
{
	static bool dataPrepared{ false };
	static CSkinnedMesh *pMinionMesh = new CSkinnedMesh(m_pCreateMgr, "Resource//3D//Minion//Mesh//Minion.meshinfo");
	static UINT incrementSize{ m_pCreateMgr->GetCbvSrvDescriptorIncrementSize() };

	static CSkeleton SIdle("Resource//3D//Minion//Animation//Sword//Minion_S_Idle.aniinfo");
	static CSkeleton SAtk1("Resource//3D//Minion//Animation//Sword//Minion_S_Attack1.aniinfo");
	static CSkeleton SAtk2("Resource//3D//Minion//Animation//Sword//Minion_S_Attack2.aniinfo");
	static CSkeleton SWalkStart("Resource//3D//Minion//Animation//Sword//Minion_S_WalkStart.aniinfo");
	static CSkeleton SWalk("Resource//3D//Minion//Animation//Sword//Minion_S_Walk.aniinfo");

	static CSkeleton BIdle("Resource//3D//Minion//Animation//Bow//Minion_B_Idle.aniinfo");
	static CSkeleton BAtk("Resource//3D//Minion//Animation//Bow//Minion_B_Attack.aniinfo");
	static CSkeleton BWalkStart("Resource//3D//Minion//Animation//Bow//Minion_B_WalkStart.aniinfo");
	static CSkeleton BWalk("Resource//3D//Minion//Animation//Bow//Minion_B_Walk.aniinfo");

	static CSkeleton MIdle("Resource//3D//Minion//Animation//Magic//Minion_M_Idle.aniinfo");
	static CSkeleton MAtk1("Resource//3D//Minion//Animation//Magic//Minion_M_Attack1.aniinfo");
	static CSkeleton MAtk2("Resource//3D//Minion//Animation//Magic//Minion_M_Attack2.aniinfo");
	static CSkeleton MWalkStart("Resource//3D//Minion//Animation//Magic//Minion_M_WalkStart.aniinfo");
	static CSkeleton MWalk("Resource//3D//Minion//Animation//Magic//Minion_M_Walk.aniinfo");

	static CSkeleton Die("Resource//3D//Minion//Animation//Minion_Die.aniinfo");

	if (!dataPrepared)
	{
		pMinionMesh->SetBoundingBox(
			XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(4)),
			XMFLOAT3(CONVERT_PaperUnit_to_InG(1.5), CONVERT_PaperUnit_to_InG(1.5), CONVERT_PaperUnit_to_InG(3.5)));
		pMinionMesh->AddRef();
		dataPrepared = true;
		return;
	}

	m_pCreateMgr->ResetCommandList();

	int kind{ 0 };
	int makeCnt{ 0 };
	for (; kind < 4; ++kind)
	{
		int index = GetPossibleIndex();

		if (index == NONE) break;
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

		pMinionObject->SetBoundingMesh(m_pCreateMgr,
			CONVERT_PaperUnit_to_InG(3), CONVERT_PaperUnit_to_InG(3), CONVERT_PaperUnit_to_InG(7),
			0, 0, -CONVERT_PaperUnit_to_InG(4));

		switch (m_kind)
		{
		case ObjectType::SwordMinion:
			pMinionObject->SetSkeleton(&SIdle);
			pMinionObject->SetSkeleton(&SAtk1);
			pMinionObject->SetSkeleton(&SAtk2);
			pMinionObject->SetSkeleton(&SWalkStart);
			pMinionObject->SetSkeleton(&SWalk);
			break;
		case ObjectType::StaffMinion:
			pMinionObject->SetSkeleton(&MIdle);
			pMinionObject->SetSkeleton(&MAtk1);
			pMinionObject->SetSkeleton(&MAtk2);
			pMinionObject->SetSkeleton(&MWalkStart);
			pMinionObject->SetSkeleton(&MWalk);
			break;
		case ObjectType::BowMinion:
			pMinionObject->SetSkeleton(&BIdle);
			pMinionObject->SetSkeleton(&BAtk);
			pMinionObject->SetSkeleton(&BWalkStart);
			pMinionObject->SetSkeleton(&BWalk);
			break;
		}

		pMinionObject->SetSkeleton(&Die);
		pMinionObject->SetTerrain(m_pTerrain);

		pMinionObject->Rotate(90, 0, 0);

		pMinionObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * index));
		pMinionObject->SetCbvGPUDescriptorHandlePtrForBB(m_pcbvGPUDescriptorStartHandle[1].ptr + (incrementSize * index));

		pMinionObject->SaveIndex(index);

		if (kind == Minion_Species::Blue_Up || kind == Minion_Species::Blue_Down)
		{
			pMinionObject->SetTeam(TeamType::Blue);
			m_blueObjects.emplace_back(pMinionObject);
		}
		else if (kind == Minion_Species::Red_Up || kind == Minion_Species::Red_Down)
		{
			pMinionObject->SetTeam(TeamType::Red);
			m_redObjects.emplace_back(pMinionObject);
		}

		makeCnt++;
	}
	m_pCreateMgr->ExecuteCommandList();

	if (!makeCnt) return;
	
	CollisionObjectList::reverse_iterator &blueBegin{ m_blueObjects.rbegin() };
	CollisionObjectList::reverse_iterator &redBegin{ m_redObjects.rbegin() };

	if (makeCnt > 1) blueBegin++;
	if (makeCnt > 3) redBegin++;

	for (kind = 0; makeCnt > 0; --makeCnt, ++kind)
	{
		if (kind == Minion_Species::Blue_Up || kind == Minion_Species::Blue_Down) {
			(*blueBegin)->ReleaseUploadBuffers();

			m_pGaugeManger->AddMinionObject((*blueBegin));
			if(blueBegin != m_blueObjects.rbegin()) --blueBegin;
		}
		else if (kind == Minion_Species::Red_Up || kind == Minion_Species::Red_Down) {
			(*redBegin)->ReleaseUploadBuffers();

			m_pGaugeManger->AddMinionObject((*redBegin));
			if (redBegin != m_redObjects.rbegin()) --redBegin;
		}

		//미니언 생성 패킷 조립, 전송
		CS_MsgMoCreate p;
		p.type = CS_PUT_MINION;
		p.size = sizeof(p);
		m_pNetwork->SendPacket(m_pNetwork->m_myid, &p);
		m_pNetwork->ReadPacket(m_pNetwork->m_mysocket, NULL);
	}
}
