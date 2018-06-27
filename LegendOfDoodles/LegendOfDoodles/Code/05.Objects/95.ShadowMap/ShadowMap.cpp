#include "stdafx.h"
#include "ShadowMap.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

/// <summary>
/// 목적: 쉐도우 매핑용 리소스 관리 오브젝트
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-06-28
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CShadowMap::CShadowMap(CCreateMgr *pCreateMgr, UINT width, UINT height)
{
	m_width = width;
	m_height = height;

	m_viewport = { 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
	m_scissorRect = { 0, 0, static_cast<int>(width), static_cast<int>(height) };

	BuildResoucre(pCreateMgr);
}

CShadowMap::~CShadowMap()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CShadowMap::BuildDescriptors(CCreateMgr *pCreateMgr, D3D12_CPU_DESCRIPTOR_HANDLE hCpuSrv, D3D12_GPU_DESCRIPTOR_HANDLE hGpuSrv, D3D12_CPU_DESCRIPTOR_HANDLE hCpuDsv)
{
	m_cpuSrv = hCpuSrv;
	m_gpuSrv = hGpuSrv;
	m_cpuDsv = hCpuDsv;

	BuildDescriptors(pCreateMgr);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CShadowMap::BuildDescriptors(CCreateMgr *pCreateMgr)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	srvDesc.Texture2D.PlaneSlice = 0;
	pCreateMgr->GetDevice()->CreateShaderResourceView(m_shadowMap.Get(), &srvDesc, m_cpuSrv);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;
	pCreateMgr->GetDevice()->CreateDepthStencilView(m_shadowMap.Get(), &dsvDesc, m_cpuDsv);
}

void CShadowMap::BuildResoucre(CCreateMgr *pCreateMgr)
{
	D3D12_CLEAR_VALUE optClear;
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	
	m_shadowMap = pCreateMgr->CreateTexture2DResource(
		pCreateMgr->GetWindowWidth(),
		pCreateMgr->GetWindowHeight(),
		DXGI_FORMAT_R24G8_TYPELESS, 
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, 
		D3D12_RESOURCE_STATE_GENERIC_READ, 
		&optClear);
}
