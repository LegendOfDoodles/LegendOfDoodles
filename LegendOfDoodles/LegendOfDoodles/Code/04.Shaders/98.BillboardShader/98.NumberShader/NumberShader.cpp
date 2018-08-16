#include "stdafx.h"
#include "NumberShader.h"
#include "05.Objects/96.Billboard/01.FrameObject/UIFrameObject.h"
#include "05.Objects/96.Billboard/98.NumberObject/NumberObject.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"
#include "05.Objects/08.Player/Player.h"
#include "07.Network/Network.h"


/// <summary>
/// 목적: Number Billboard 쉐이더 (시간, Kill Dead, Assist )
/// 최종 수정자:  김나단
/// 수정자 목록:  이용선, 김나단
/// 최종 수정 날짜: 2018-07-03
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자

CNumberShader::CNumberShader(shared_ptr<CCreateMgr> pCreateMgr) : CBillboardShader(pCreateMgr)
{
}

CNumberShader::~CNumberShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CNumberShader::UpdateShaderVariables(int opt)
{
	UNREFERENCED_PARAMETER(opt);
	static UINT elementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAUGE_INFO *pMappedObject = (CB_GAUGE_INFO *)(m_pMappedObjects + (i * elementBytes));
		pMappedObject->m_fCurrentHP = static_cast<float>(((CNumberOjbect*)m_ppObjects[i])->GetNum());
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CNumberShader::AnimateObjects(float timeElapsed)
{
	m_iTimer[0] += timeElapsed;
	if (m_iTimer[0] > 60.f) {
		m_iTimer[0] = 0.0f;
		m_iTimer[1] += 1.0f;
	}

	int nSec{ 0 };
	int nMin{ 0 };
	int nKill{ 0 };
	int nDeath{ 0 };
	int nLevel{ 0 };
	int nBlueTeam{ 0 };
	int nRedTeam{ 0 };

	for (int j = 0; j < m_nObjects; j++)
	{
		if (((CNumberOjbect*)m_ppObjects[j])->GetType() == TimeSec) {

			int checkNum = static_cast<int>(m_iTimer[0]);		// 자리 수 확인에서 사용할 변수

			m_iTimerPositionalNum[0] = 2;		
			
			for (int k = 0; k < m_iTimerPositionalNum[0]; ++k) {
				m_iTimerSignificnatNum[0][k] = checkNum % 10;
				checkNum /= 10;
			}

			((CNumberOjbect*)m_ppObjects[j])->SetTexCoord(m_iTimerSignificnatNum[0][(m_iTimerPositionalNum[0] - 1) - nSec]);
			((CNumberOjbect*)m_ppObjects[j])->SetOffset(nSec++);
		}

		if (((CNumberOjbect*)m_ppObjects[j])->GetType() == TimeMinute) {

			int checkNum = static_cast<int>(m_iTimer[1]);		// 자리 수 확인에서 사용할 변수

			m_iTimerPositionalNum[1] = 2;

			for (int k = 0; k < m_iTimerPositionalNum[1]; ++k) {
				m_iTimerSignificnatNum[1][k] = checkNum % 10;
				checkNum /= 10;
			}

			((CNumberOjbect*)m_ppObjects[j])->SetTexCoord(m_iTimerSignificnatNum[1][(m_iTimerPositionalNum[1] - 1) - nMin]);
			((CNumberOjbect*)m_ppObjects[j])->SetOffset(nMin++);
		}

		if (((CNumberOjbect*)m_ppObjects[j])->GetType() == PersonalKill) {

			int checkNum = static_cast<int>(m_ppPlayers[m_pNetwork->m_myid]->GetPlayerStatus()->Kill);		// 자리 수 확인에서 사용할 변수
			m_iPlayerKDA[0] = static_cast<int>(m_ppPlayers[m_pNetwork->m_myid]->GetPlayerStatus()->Kill);		// 자리 수 확인에서 사용할 변수

			m_iKDAPositionalNum[0] = 2;

			//if (checkNum == 0)
			//	m_iKDAPositionalNum[0] = 1;		// 0 이면 자리수는 1개
			//else
			//	for (m_iKDAPositionalNum[0] = 0; checkNum > 0; checkNum /= 10, m_iKDAPositionalNum[0]++);

			m_iKDASignificantNum[0] = new int[m_iKDAPositionalNum[0]];

			// Num[0] 부터 1의 자리 10의 자리 순차적 증가 저장
			// 30이면 0, 3 저장 (출력은 반대로 해야 함)
			for (int j = 0; j < m_iKDAPositionalNum[0]; ++j) {
				m_iKDASignificantNum[0][j] = checkNum % 10;

				checkNum /= 10;
			}

			((CNumberOjbect*)m_ppObjects[j])->SetTexCoord(m_iKDASignificantNum[0][(m_iKDAPositionalNum[0] - 1) - nKill ]);
			((CNumberOjbect*)m_ppObjects[j])->SetOffset(nKill++);
		}

		if (((CNumberOjbect*)m_ppObjects[j])->GetType() == PersonalDeath) {

			int checkNum = static_cast<int>(m_ppPlayers[m_pNetwork->m_myid]->GetPlayerStatus()->Death);		// 자리 수 확인에서 사용할 변수
			m_iPlayerKDA[1] = static_cast<int>(m_ppPlayers[m_pNetwork->m_myid]->GetPlayerStatus()->Death);		// 자리 수 확인에서 사용할 변수

			m_iKDAPositionalNum[1] = 2;

			m_iKDASignificantNum[1] = new int[m_iKDAPositionalNum[1]];

			// Num[0] 부터 1의 자리 10의 자리 순차적 증가 저장
			// 30이면 0, 3 저장 (출력은 반대로 해야 함)
			for (int j = 0; j < m_iKDAPositionalNum[1]; ++j) {
				m_iKDASignificantNum[1][j] = m_iPlayerKDA[1] % 10;

				m_iPlayerKDA[1] /= 10;
			}
			((CNumberOjbect*)m_ppObjects[j])->SetTexCoord(m_iKDASignificantNum[1][(m_iKDAPositionalNum[1] - 1) - nDeath]);
			((CNumberOjbect*)m_ppObjects[j])->SetOffset(nDeath++);
		}

		if (((CNumberOjbect*)m_ppObjects[j])->GetType() == PersonalAssist) {

			int checkNum = static_cast<int>(m_ppPlayers[m_pNetwork->m_myid]->GetPlayerStatus()->Level);		// 자리 수 확인에서 사용할 변수
			m_iPlayerKDA[2] = static_cast<int>(m_ppPlayers[m_pNetwork->m_myid]->GetPlayerStatus()->Level);		// 자리 수 확인에서 사용할 변수

			m_iKDAPositionalNum[2] = 2;

			m_iKDASignificantNum[2] = new int[m_iKDAPositionalNum[2]];

			// Num[0] 부터 1의 자리 10의 자리 순차적 증가 저장
			// 30이면 0, 3 저장 (출력은 반대로 해야 함)
			for (int j = 0; j < m_iKDAPositionalNum[2]; ++j) {
				m_iKDASignificantNum[2][j] = checkNum % 10;

				checkNum /= 10;
			}
			((CNumberOjbect*)m_ppObjects[j])->SetTexCoord(m_iKDASignificantNum[2][(m_iKDAPositionalNum[2] - 1) - nLevel]);
			((CNumberOjbect*)m_ppObjects[j])->SetOffset(nLevel++);
		}

		if (((CNumberOjbect*)m_ppObjects[j])->GetType() == TeamKILL::BlueTeam) {

			for (int i = 0; i < m_nPlayer; ++i) {
				// 플레이어의 스테이터스의 Kill을 팀별로 더한다.
				if (m_ppPlayers[i]->GetTeam() == TeamType::Blue)
					m_iTeamKill[TeamKILL::BlueTeam] += m_ppPlayers[i]->GetPlayerStatus()->Kill;
				else if (m_ppPlayers[i]->GetTeam() == TeamType::Red)
					m_iTeamKill[TeamKILL::RedTeam] += m_ppPlayers[i]->GetPlayerStatus()->Kill;
			}

			int checkNum = m_iTeamKill[0];	// 자리 수 확인에서 사용할 변수

			m_iTeamKillPositionalNum[0] = 2;		// 0 이면 자리수는 1개

			m_iTeamKillSignificantNum[0] = new int[m_iTeamKillPositionalNum[0]];

			// Num[0] 부터 1의 자리 10의 자리 순차적 증가 저장
			// 30이면 0, 3 저장 (출력은 반대로 해야 함)
			for (int j = 0; j < m_iTeamKillPositionalNum[0]; ++j) {
				m_iTeamKillSignificantNum[0][j] = m_iTeamKill[0] % 10;

				m_iTeamKill[0] /= 10;
			}
			((CNumberOjbect*)m_ppObjects[j])->SetTexCoord(m_iTeamKillSignificantNum[0][(m_iTeamKillPositionalNum[0] - 1) - nBlueTeam]);
			((CNumberOjbect*)m_ppObjects[j])->SetOffset(nBlueTeam++);
		}

		if (((CNumberOjbect*)m_ppObjects[j])->GetType() == TeamKILL::RedTeam) {

			for (int i = 0; i < m_nPlayer; ++i) {
				// 플레이어의 스테이터스의 Kill을 팀별로 더한다.
				if (m_ppPlayers[i]->GetTeam() == TeamType::Blue)
					m_iTeamKill[TeamKILL::BlueTeam] += m_ppPlayers[i]->GetPlayerStatus()->Kill;
				else if (m_ppPlayers[i]->GetTeam() == TeamType::Red)
					m_iTeamKill[TeamKILL::RedTeam] += m_ppPlayers[i]->GetPlayerStatus()->Kill;
			}

			int checkNum = m_iTeamKill[1];	// 자리 수 확인에서 사용할 변수
			
			m_iTeamKillPositionalNum[1] = 2;		// 0 이면 자리수는 1개

			m_iTeamKillSignificantNum[1] = new int[m_iTeamKillPositionalNum[1]];

			// Num[0] 부터 1의 자리 10의 자리 순차적 증가 저장
			// 30이면 0, 3 저장 (출력은 반대로 해야 함)
			for (int j = 0; j < m_iTeamKillPositionalNum[1]; ++j) {
				m_iTeamKillSignificantNum[1][j] = m_iTeamKill[1] % 10;

				m_iTeamKill[1] /= 10;
			}
			((CNumberOjbect*)m_ppObjects[j])->SetTexCoord(m_iTeamKillSignificantNum[1][(m_iTeamKillPositionalNum[1] - 1) - nRedTeam]);
			((CNumberOjbect*)m_ppObjects[j])->SetOffset(nRedTeam++);
		}

		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CNumberShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera);

	m_ppMaterials[0]->UpdateShaderVariables();
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Render(pCamera);
	}
}

void CNumberShader::SetCamera(CCamera * pCamera)
{
	m_pCamera = pCamera;
}

////////////////////////////////////////////////////////////////////////
// 내부함수
D3D12_INPUT_LAYOUT_DESC CNumberShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
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
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_BLEND_DESC CNumberShader::CreateBlendState()
{
	D3D12_BLEND_DESC blendDesc;
	::ZeroMemory(&blendDesc, sizeof(D3D12_BLEND_DESC));

	blendDesc.AlphaToCoverageEnable = TRUE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	blendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(blendDesc);
}

D3D12_SHADER_BYTECODE CNumberShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	//./Code/04.Shaders/99.GraphicsShader/
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSTexturedGauge",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CNumberShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSTexturedNumber",
		"ps_5_1",
		pShaderBlob));
}

void CNumberShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CNumberShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void * pContext)
{
	m_pCamera = (CCamera*)pContext;

	/* Team K */
	// 플레이어 4명을 받아와서
	for (int i = 0; i < m_nPlayer; ++i) {
		// 플레이어의 스테이터스의 Kill을 팀별로 더한다.
		if (m_ppPlayers[i]->GetTeam() == TeamType::Blue)
			m_iTeamKill[TeamKILL::BlueTeam] += m_ppPlayers[i]->GetPlayerStatus()->Kill;
		else if (m_ppPlayers[i]->GetTeam() == TeamType::Red)
			m_iTeamKill[TeamKILL::RedTeam] += m_ppPlayers[i]->GetPlayerStatus()->Kill;
	}

	for (int i = 0; i < TeamKILL::EnumCnt; ++i) {
		int checkNum = m_iTeamKill[i];		// 자리 수 확인에서 사용할 변수

		m_iTeamKillPositionalNum[i] = 2;

		//if (checkNum == 0)
		//	m_iTeamKillPositionalNum[i] = 1;		// 0 이면 자리수는 1개
		//else
		//	for (m_iTeamKillPositionalNum[i] = 0; checkNum > 0; checkNum /= 10, m_iTeamKillPositionalNum[i]++);

		m_iTeamKillSignificantNum[i] = new int[m_iTeamKillPositionalNum[i]];

		// Num[0] 부터 1의 자리 10의 자리 순차적 증가 저장
		// 30이면 0, 3 저장 (출력은 반대로 해야 함)
		for (int j = 0; j < m_iTeamKillPositionalNum[i]; ++j) {
			m_iTeamKillSignificantNum[i][j] = m_iTeamKill[i] % 10;

			m_iTeamKill[i] /= 10;
		}
	}
	
	/* KDA */
	m_iPlayerKDA[0] = m_ppPlayers[m_pNetwork->m_myid]->GetPlayerStatus()->Kill;
	m_iPlayerKDA[1] = m_ppPlayers[m_pNetwork->m_myid]->GetPlayerStatus()->Death;
	m_iPlayerKDA[2] = m_ppPlayers[m_pNetwork->m_myid]->GetPlayerStatus()->Level;
	
	for (int i = 0; i < 3; ++i) {
		int checkNum = m_iPlayerKDA[i];		// 자리 수 확인에서 사용할 변수

		m_iKDAPositionalNum[i] = 2;

		m_iKDASignificantNum[i] = new int[m_iKDAPositionalNum[i]];

		// Num[0] 부터 1의 자리 10의 자리 순차적 증가 저장
		// 30이면 0, 3 저장 (출력은 반대로 해야 함)
		for (int j = 0; j < m_iKDAPositionalNum[i]; ++j) {
			m_iKDASignificantNum[i][j] = m_iPlayerKDA[i] % 10;

			m_iPlayerKDA[i] /= 10;
		}
	}

	/* Timer */
	for (int i = 0; i < 2; ++i) {
		m_iTimerPositionalNum[i] = 2;

		// Num[0] 부터 1의 자리 10의 자리 순차적 증가 저장
		// 30이면 0, 3 저장 (출력은 반대로 해야 함)
		for (int j = 0; j < m_iTimerPositionalNum[i]; ++j) {
			m_iTimerSignificnatNum[i][j] = static_cast<int>(m_iTimer[i]) % 10;

			m_iTimer[i] /= 10;
		}
	}

	for (int i = 0; i < 2; ++i) m_nObjects += m_iTeamKillPositionalNum[i];
	for (int i = 0; i < 3; ++i) m_nObjects += m_iKDAPositionalNum[i];
	for (int i = 0; i < 2; ++i) m_nObjects += m_iTimerPositionalNum[i];

	m_ppObjects = new CBaseObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 1);
	CreateShaderVariables(pCreateMgr, ncbElementBytes, m_nObjects);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pConstBuffer.Get(), ncbElementBytes);

	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CNumberOjbect *pNumber{ NULL };

	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = Materials::CreateNumberMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);

	int objectCnt = 0;

	/* Team K */
	for (int j = 0; j < TeamKILL::EnumCnt;++j) {
		for (int i = 0; i < m_iTeamKillPositionalNum[j]; ++i)
		{
			pNumber = new CNumberOjbect(pCreateMgr, NumberType(BlueTeam + j));
			pNumber->SetCamera(m_pCamera);
			pNumber->SetDistance(FRAME_BUFFER_WIDTH / 128);	 // distance 10
			pNumber->SetTexCoord(m_iTeamKillSignificantNum[j][(m_iTeamKillPositionalNum[j] - 1) - i]);
			pNumber->SetOffset(i);

			pNumber->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * (objectCnt)));
			m_ppObjects[objectCnt] = pNumber;

			++objectCnt;
		}
	}

	/* Persoanl KDA */
	for (int j = 0; j < 3; ++j) {
		for (int i = 0; i < m_iKDAPositionalNum[j]; ++i)
		{
			pNumber = new CNumberOjbect(pCreateMgr, NumberType(PersonalKill + j));
			pNumber->SetCamera(m_pCamera);
			pNumber->SetDistance(FRAME_BUFFER_WIDTH / 128);	 // distance 10
			pNumber->SetTexCoord(m_iKDASignificantNum[j][(m_iKDAPositionalNum[j] - 1) - i]);
			pNumber->SetOffset(i);
			
			pNumber->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * (objectCnt)));
			m_ppObjects[objectCnt] = pNumber;

			++objectCnt;
		}
	}

	/* Game Timer */
	for (int j = 0; j < 2; ++j) {
		for (int i = 0; i < m_iTimerPositionalNum[j]; ++i)
		{
			pNumber = new CNumberOjbect(pCreateMgr, NumberType(TimeMinute + j));
			pNumber->SetCamera(m_pCamera);
			pNumber->SetDistance(FRAME_BUFFER_WIDTH / 128);	 // distance 10
			pNumber->SetTexCoord(m_iTimerSignificnatNum[j][(m_iTimerPositionalNum[j] - 1) - i]);
			pNumber->SetOffset(i);

			pNumber->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * (objectCnt)));
			m_ppObjects[objectCnt] = pNumber;

			++objectCnt;
		}
	}

}

void CNumberShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			Safe_Delete(m_ppObjects[j]);
		}
		Safe_Delete_Array(m_ppObjects);
	}

#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i < m_nMaterials; ++i)
		{
			Safe_Delete(m_ppMaterials[i]);
		}
		Safe_Delete_Array(m_ppMaterials);
	}
#endif
}
