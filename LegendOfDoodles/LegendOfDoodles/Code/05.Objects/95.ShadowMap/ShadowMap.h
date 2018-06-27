#pragma once

class CCreateMgr;

class CShadowMap
{
public:	// ������, �Ҹ���
	CShadowMap(CCreateMgr *pCreateMgr, UINT width, UINT height);
	virtual ~CShadowMap();

public:	// ���� �Լ�
	void BuildDescriptors(
		CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
		D3D12_GPU_DESCRIPTOR_HANDLE hGpuSrv, 
		D3D12_CPU_DESCRIPTOR_HANDLE hCpuDsv);

	UINT GetWidth() { return m_width; }
	UINT GetHeight() { return m_height; }

	ID3D12Resource * GetResource() { return m_shadowMap.Get(); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrv() const { return m_gpuSrv; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetDsv() const { return m_cpuDsv; }

	D3D12_VIEWPORT GetViewport() const { return m_viewport; }
	D3D12_RECT GetScissorRect() const { return m_scissorRect; }

protected: // ���� �Լ�
	void BuildDescriptors(CCreateMgr *pCreateMgr);
	void BuildResoucre(CCreateMgr *pCreateMgr);

private:	// ����
	UINT m_width{ 0 };
	UINT m_height{ 0 };

	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;

	DXGI_FORMAT m_format{ DXGI_FORMAT_R24G8_TYPELESS };

	D3D12_CPU_DESCRIPTOR_HANDLE m_cpuSrv;
	D3D12_GPU_DESCRIPTOR_HANDLE m_gpuSrv;
	D3D12_CPU_DESCRIPTOR_HANDLE m_cpuDsv;

	ComPtr<ID3D12Resource> m_shadowMap{ NULL };
};

