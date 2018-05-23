#pragma once
#include "05.Objects/99.Material/Material.h"

class CCreateMgr;

namespace Materials
{
	// Terrain
	CMaterial* CreateTerrainMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// SkyBox
	CMaterial* CreateSkyBoxMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateFloorMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateCubeMapMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// Building
	CMaterial* CreateTresureBoxMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateShellMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateDuckMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateKeumOneBoMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateRoundSoapDispenserMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateSquareSoapDispenserMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// Environment
	CMaterial* CreateEraserMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateDiceMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateLongPencilMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateNailClipperMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreatePaperCupMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreatePenMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreatePencilCaseMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreatePenCoverMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateRoundHeadPhoneMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateShortPencilMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateSquareHeadPhoneMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateBook1Material(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateBook2Material(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateBook3Material(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateBook4Material(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// Minion
	CMaterial* CreateMinionMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// Player
	CMaterial* CreatePlayerMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// Sketch Effect
	CMaterial* CreateSketchMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);
	
	// UI----------------------------------------------------------
	CMaterial* CreateUIMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateGreyMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// HP, Minion Icon
	CMaterial* CreateRedMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// Icon
	//Player
	CMaterial* CreateStickIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateSwordIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateStaffIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateBowIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateDeathIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// Common Monster
	CMaterial* CreateCamIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateLoydeIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateGolemIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// Tower
	CMaterial* CreateRedTowerIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateBlueTowerIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// Nexus
	CMaterial* CreateBoxNexusIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateShellNexusIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);



	// SRV 생성 함수
	void GetShaderResourceViewDesc(
		D3D12_RESOURCE_DESC resourceDesc,
		UINT nTextureType,
		D3D12_SHADER_RESOURCE_VIEW_DESC *pShaderResourceViewDesc);
		
	void CreateShaderResourceViews(
		CCreateMgr *pCreateMgr, CTexture *pTexture,
		UINT nRootParameterStartIndex, bool bAutoIncrement,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);
}