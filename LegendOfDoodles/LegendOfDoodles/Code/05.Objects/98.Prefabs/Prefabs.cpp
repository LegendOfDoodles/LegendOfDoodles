#include "stdafx.h"
#include "Prefabs.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

/// <summary>
/// 목적: 사용하는 매터리얼 정리용
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단, 이용선
/// 최종 수정 날짜: 2018-08-12
/// </summary>

////////////////////////////////////////////////////////////////////////
// Terrain
CMaterial* Materials::CreateTerrainMaterial(shared_ptr<CCreateMgr> pCreateMgr,
	D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
	D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Terrain/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	pMaterial->SetAlbedo(XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f));
	pMaterial->SetRoughness(0.98f);
	pMaterial->SetMetalic(0.01f);

	return pMaterial;
}

////////////////////////////////////////////////////////////////////////
// SkyBox
CMaterial * Materials::CreateSkyBoxMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(6, RESOURCE_TEXTURE_2D, 0) };

	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/SkyBox/SkyBox_Front_0.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/SkyBox/SkyBox_Back_0.dds", 1);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/SkyBox/SkyBox_Left_0.dds", 2);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/SkyBox/SkyBox_Right_0.dds", 3);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/SkyBox/SkyBox_Top_0.dds", 4);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/SkyBox/Floor.dds", 5);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateFloorMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2D, 1) };

	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/SkyBox/Floor.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateCubeMapMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_CUBE, 1) };

	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/CubeMap/cubeMap.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

////////////////////////////////////////////////////////////////////////
// Building
CMaterial * Materials::CreateTresureBoxMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Building/TresureBox/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateShellMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Building/Shell/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateDuckMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Building/Duck/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateKeumOneBoMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Building/KeumOneBo/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateRoundSoapDispenserMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Building/RoundSoapDispenser/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateSquareSoapDispenserMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Building/SquareSoapDispenser/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

////////////////////////////////////////////////////////////////////////
// Environment
CMaterial * Materials::CreateEraserMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Environment/Eraser/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateDiceMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Environment/Dice/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateLongPencilMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Environment/LongPencil/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateNailClipperMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Environment/NailClipper/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreatePaperCupMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Environment/PaperCup/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreatePenMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Environment/Pen/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreatePencilCaseMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Environment/PencilCase/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreatePenCoverMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Environment/PenCover/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateRoundHeadPhoneMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Environment/RoundHeadPhone/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateShortPencilMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Environment/ShortPencil/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateSquareHeadPhoneMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Environment/SquareHeadPhone/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateBook1Material(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Environment/Books/Textures1.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateBook2Material(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Environment/Books/Textures2.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateBook3Material(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Environment/Books/Textures3.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateBook4Material(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Environment/Books/Textures4.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

////////////////////////////////////////////////////////////////////////
// Minion
CMaterial * Materials::CreateSwordMinionMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Minion/Sword/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateBowMinionMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Minion/Bow/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateStaffMinionMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Minion/Magic/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

////////////////////////////////////////////////////////////////////////
// Player
CMaterial * Materials::CreatePlayerMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Player/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateDumbbellMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Monster/Roider/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateArrowMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Common/Arrow/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateMagicMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Common/Magic/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateBlueTowerAtkMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Common/BlueTowerAtk/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateRedTowerAtkMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Common/RedTowerAtk/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreatePlayerArrowMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Player/Throwing/PlayerArrow.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreatePlayerMagicSkillQMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Player/Throwing/PlayerMagicSkillQ.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreatePlayerArrowSkillWMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Player/Throwing/PlayerArrowSkillW.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreatePlayerMagicSkillRMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Player/Throwing/PlayerMagicSkillR.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreatePlayerMagicMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Player/Throwing/PlayerMagic.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

////////////////////////////////////////////////////////////////////////
// Sketch Effect
CMaterial * Materials::CreateSketchMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/SketchEffect/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		4, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

////////////////////////////////////////////////////////////////////////
// UI
CMaterial * Materials::CreateUIMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture = new CTexture(6, RESOURCE_TEXTURE_2D, 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Frame/SquareFrame.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Frame/CircleFrame.dds", 1);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Frame/GameStatusB.dds", 2);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/System/KDA.dds", 3);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/System/Timer.dds", 4);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Frame/SelectSpecial.dds", 5);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateSpecialSelectWindowsMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture = new CTexture(2, RESOURCE_TEXTURE_2D, 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Frame/SelectcWeapon.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Frame/SpecialFrame.dds", 1);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateMinimapUIMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Minimap/Minimap.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateGreyMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Frame/Grey.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateNumberMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Number/Number.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateSpecialMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(3, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/System/Attack.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/System/Defence.dds", 1);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/System/Tec.dds", 2);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateStickGraySkillMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture = new CTexture(4, RESOURCE_TEXTURE_2D, 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Stick/QG.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Stick/WG.dds", 1);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Stick/EG.dds", 2);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Stick/RG.dds", 3);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateStickColorSkillMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture = new CTexture(4, RESOURCE_TEXTURE_2D, 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Stick/Q.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Stick/W.dds", 1);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Stick/E.dds", 2);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Stick/R.dds", 3);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateSwordGraySkillMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture = new CTexture(4, RESOURCE_TEXTURE_2D, 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Sword/QG.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Sword/WG.dds", 1);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Sword/EG.dds", 2);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Sword/RG.dds", 3);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateSwordColorSkillMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture = new CTexture(4, RESOURCE_TEXTURE_2D, 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Sword/Q.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Sword/W.dds", 1);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Sword/E.dds", 2);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Sword/R.dds", 3);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateMagicGraySkillMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture = new CTexture(4, RESOURCE_TEXTURE_2D, 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Magic/QG.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Magic/WG.dds", 1);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Magic/EG.dds", 2);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Magic/RG.dds", 3);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateMagicColorSkillMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture = new CTexture(4, RESOURCE_TEXTURE_2D, 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Magic/Q.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Magic/W.dds", 1);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Magic/E.dds", 2);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Magic/R.dds", 3);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateBowGraySkillMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture = new CTexture(4, RESOURCE_TEXTURE_2D, 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Bow/QG.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Bow/WG.dds", 1);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Bow/EG.dds", 2);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Bow/RG.dds", 3);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateBowColorSkillMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture = new CTexture(4, RESOURCE_TEXTURE_2D, 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Bow/Q.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Bow/W.dds", 1);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Bow/E.dds", 2);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Skill/Bow/R.dds", 3);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateHPGaugeMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(2, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Gauge/Red.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Gauge/Blue.dds", 1);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateNeutralityGagueMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(3, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Gauge/Blue.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Gauge/Red.dds", 1);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Gauge/Grey.dds", 2);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreatePlayerMPGagueMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Gauge/CircleMP.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreatePlayerBlueIconMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(4, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Blue/Stick.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Blue/Sword.dds", 1);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Blue/Magic.dds", 2);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Blue/Bow.dds", 3);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateTestMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(8, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Blue/Stick.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Blue/Sword.dds", 1);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Blue/Magic.dds", 2);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Blue/Bow.dds", 3);

	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Red/Stick.dds", 4);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Red/Sword.dds", 5);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Red/Magic.dds", 6);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Red/Bow.dds", 7);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreatePlayerRedIconMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(4, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Red/Stick.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Red/Sword.dds", 1);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Red/Magic.dds", 2);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Red/Bow.dds", 3);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateMinionIconMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(2, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Blue/Minion.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Red/Minion.dds", 1);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateDeathIconMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/System/Death.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateLevelUpMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/System/LevelUp.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateCamBossIconMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(3, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Common/CamBoss.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Blue/CamBoss.dds", 1);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Red/CamBoss.dds", 2);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateRoiderIconMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(3, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Common/Roider.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Blue/Roider.dds", 1);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Red/Roider.dds", 2);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateGolemIconMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(3, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Common/Golem.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Blue/Golem.dds", 1);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Red/Golem.dds", 2);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateTowerIconMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(2, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Blue/Tower.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Red/Tower.dds", 1);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateNexusIconMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(2, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Blue/Nexus.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/UI/Icon/Red/Nexus.dds", 1);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreatePlayerSkillEffectMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(6, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Effect/PlayerSkillEffect/Sword_Q.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Effect/PlayerSkillEffect/Sword_W.dds", 1);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Effect/PlayerSkillEffect/Sword_E.dds", 2);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Effect/PlayerSkillEffect/Sword_R.dds", 3);

	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Effect/PlayerSkillEffect/Staff_W.dds", 4);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Effect/PlayerSkillEffect/Staff_E.dds", 5);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreatePlayerAttackEffectMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(4, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Effect/PlayerAttackEffect/Arrow_Attack.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Effect/PlayerAttackEffect/Staff_Attack.dds", 1);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Effect/PlayerAttackEffect/Staff_Q.dds", 2);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Effect/PlayerAttackEffect/Staff_E.dds", 3);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateMinionAttackEffectMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(2, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Effect/MinionAttackEffect/Arrow_Attack.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Effect/MinionAttackEffect/Staff_Attack.dds", 1);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateFlyingObjectEffectMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(4, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Effect/FlyingObjectEffect/Minion_Arrow.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Effect/FlyingObjectEffect/Player_Arrow.dds", 1);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Effect/FlyingObjectEffect/Player_FireBall.dds", 2);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Effect/FlyingObjectEffect/TowerAttackExplosion.dds", 3);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateHitEffectMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(2, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Effect/HitEffect/Explosion.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Effect/HitEffect/NomalHit.dds", 1);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateGolemAttackEffectMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(3, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Effect/GolemAttackEffect/Standard.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Effect/GolemAttackEffect/Stump.dds", 1);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Effect/GolemAttackEffect/Special.dds", 2);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateLevelUpEffectMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(2, RESOURCE_TEXTURE_2D, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Effect/LevelUp/Circle.dds", 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Effect/LevelUp/Arrow.dds", 1);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		2, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateStickMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Equip/Stick/Stick.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateSwordMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Player/Sword/BasicSword/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateMaceMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Player/Sword/Mace/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateBFSwordMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Player/Sword/BFSword/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateSawMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Player/Sword/Saw/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateSabreMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Player/Sword/Sabre/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateStaffMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Player/Staff/Staff/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateEraserPenMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Player/Staff/EraserPen/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateLightingMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Player/Staff/Lighting/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateWatchMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Player/Staff/Watch/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateLolipopMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Player/Staff/Lolipop/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateBowMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Player/Bow/Bow/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateCompoundMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Player/Bow/Bow/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateFlightMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Player/Bow/FlightBone/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateBananaMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Player/Bow/Banana/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateBattleMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Player/Bow/Battle/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateShoulderMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Equip/Sword/Shoulder/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateBulletJaketMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Equip/Sword/BulletJaket/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateNikeMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Equip/Sword/Nike/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateMedicBoxMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Equip/Sword/MedicBox/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateShaShackMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Equip/Staff/ShaShack/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateMosMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Equip/Staff/Mos/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateSpringMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Equip/Staff/Spring/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateBloodPackMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Equip/Staff/BloodPack/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateMufflerMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Equip/Bow/Muffler/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateLinkClothMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Equip/Bow/LinkCloth/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::Create3DidasMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Equip/Bow/3Didas/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateCrossHairMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Equip/Bow/CrossHair/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateGlassesMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Equip/Special/Glasses/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateSunGlassesMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Equip/Special/SunGlasses/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateWinderMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Equip/Special/Winder/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

CMaterial * Materials::CreateIcecreamMaterial(shared_ptr<CCreateMgr> pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE * pSrvCPUDescriptorStartHandle, D3D12_GPU_DESCRIPTOR_HANDLE * pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	CTexture *pTexture{ new CTexture(1, RESOURCE_TEXTURE_2DARRAY, 0) };
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Equip/Special/Icecream/Textures.dds", 0);

	CreateShaderResourceViews(
		pCreateMgr, pTexture,
		3, false,
		pSrvCPUDescriptorStartHandle,
		pSrvGPUDescriptorStartHandle);

	pMaterial->Initialize(pCreateMgr);
	pMaterial->SetTexture(pTexture);

	return pMaterial;
}

// SRV 생성 함수 ...
void Materials::GetShaderResourceViewDesc(
	D3D12_RESOURCE_DESC resourceDesc,
	UINT nTextureType,
	D3D12_SHADER_RESOURCE_VIEW_DESC *pShaderResourceViewDesc)
{
	pShaderResourceViewDesc->Format = resourceDesc.Format;
	pShaderResourceViewDesc->Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	switch (nTextureType)
	{
	case RESOURCE_TEXTURE_2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	case RESOURCE_TEXTURE_2D_ARRAY:
		pShaderResourceViewDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		pShaderResourceViewDesc->Texture2D.MipLevels = static_cast<UINT>(-1);
		pShaderResourceViewDesc->Texture2D.MostDetailedMip = 0;
		pShaderResourceViewDesc->Texture2D.PlaneSlice = 0;
		pShaderResourceViewDesc->Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_TEXTURE_2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		pShaderResourceViewDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		pShaderResourceViewDesc->Texture2DArray.MipLevels = static_cast<UINT>(-1);
		pShaderResourceViewDesc->Texture2DArray.MostDetailedMip = 0;
		pShaderResourceViewDesc->Texture2DArray.PlaneSlice = 0;
		pShaderResourceViewDesc->Texture2DArray.ResourceMinLODClamp = 0.0f;
		pShaderResourceViewDesc->Texture2DArray.FirstArraySlice = 0;
		pShaderResourceViewDesc->Texture2DArray.ArraySize = resourceDesc.DepthOrArraySize;
		break;
	case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
		pShaderResourceViewDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		pShaderResourceViewDesc->TextureCube.MipLevels = 1;
		pShaderResourceViewDesc->TextureCube.MostDetailedMip = 0;
		pShaderResourceViewDesc->TextureCube.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		pShaderResourceViewDesc->ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		pShaderResourceViewDesc->Buffer.FirstElement = 0;
		pShaderResourceViewDesc->Buffer.NumElements = 0;
		pShaderResourceViewDesc->Buffer.StructureByteStride = 0;
		pShaderResourceViewDesc->Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	}
}

void Materials::CreateShaderResourceViews(
	shared_ptr<CCreateMgr> pCreateMgr, CTexture *pTexture,
	UINT nRootParameterStartIndex, bool bAutoIncrement,
	D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
	D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle)
{
	UINT incrementSize = pCreateMgr->GetCbvSrvDescriptorIncrementSize();
	D3D12_CPU_DESCRIPTOR_HANDLE srvCPUDescriptorHandle = *pSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE srvGPUDescriptorHandle = *pSrvGPUDescriptorStartHandle;
	D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	int nTextures = pTexture->GetTextureCount();
	int nTextureType = pTexture->GetTextureType();
	for (int i = 0; i < nTextures; i++)
	{
		ComPtr<ID3D12Resource> pShaderResource = pTexture->GetTexture(i);
		D3D12_RESOURCE_DESC resourceDesc = pShaderResource->GetDesc();
		GetShaderResourceViewDesc(resourceDesc, nTextureType, &shaderResourceViewDesc);
		pCreateMgr->GetDevice()->CreateShaderResourceView(pShaderResource.Get(), &shaderResourceViewDesc, srvCPUDescriptorHandle);
		srvCPUDescriptorHandle.ptr += incrementSize;

		pTexture->SetRootArgument(i, (bAutoIncrement) ? (nRootParameterStartIndex + i) : nRootParameterStartIndex, srvGPUDescriptorHandle);
		srvGPUDescriptorHandle.ptr += incrementSize;
	}
}
// ... SRV 생성 함수