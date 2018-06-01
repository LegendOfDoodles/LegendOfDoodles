#include "stdafx.h"
#include "UIShader.h"
#include "05.Objects/96.Billboard/Billboard.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"

/// <summary>
/// 목적: UI 테스트 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  이용선, 김나단
/// 최종 수정 날짜: 2018-06-01
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CUIObjectShader::CUIObjectShader(CCreateMgr * pCreateMgr)
	: CShader(pCreateMgr)
{

}

CUIObjectShader::~CUIObjectShader()
{
}

////////////////////////////////////////////////////////////////////////
//
void CUIObjectShader::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			m_ppObjects[j]->ReleaseUploadBuffers();
		}
	}

	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
}

void CUIObjectShader::UpdateShaderVariables()
{
	static UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CUIObjectShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}

}

void CUIObjectShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera);

	m_ppMaterials[0]->UpdateShaderVariable(0);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (j == 1) m_ppMaterials[0]->UpdateShaderVariable(1);
		if (j == 3 && OnOFF) m_ppObjects[j]->Render(pCamera);
		else if (j != 3)	 m_ppObjects[j]->Render(pCamera);
	}
}

void CUIObjectShader::GetCamera(CCamera * pCamera)
{
	m_pCamera = pCamera;

	for (int i = 0; i < m_nObjects; ++i) {
		static_cast<CUIObject*>(m_ppObjects[i])->SetCamera(m_pCamera);
	}
}

void CUIObjectShader::OnStatus(int  ObjectType)
{

	if (OnOFF == false) OnOFF = true;

	switch (ObjectType)
	{
	default:
		break;
	}
}

bool CUIObjectShader::OnProcessKeyInput(UCHAR * pKeyBuffer)
{
	return true;
}

bool CUIObjectShader::OnProcessMouseInput(WPARAM pKeyBuffer)
{
	POINT cursorPos;

	GetCursorPos(&cursorPos);
	ScreenToClient(m_pCamera->GetHwnd(), &cursorPos);

	// 나중에 Define 하는 형식으로 변형
	XMFLOAT4 MinimapArea;
	XMFLOAT4 StatusArea;
	float RatioX;
	float RatioY;

	MinimapArea.x = FRAME_BUFFER_WIDTH  / 1.3034f;
	MinimapArea.y = FRAME_BUFFER_WIDTH  / 1.0078f;
	MinimapArea.z = FRAME_BUFFER_HEIGHT / 1.3333f;
	MinimapArea.w = FRAME_BUFFER_HEIGHT / 1.0526f;

	RatioX = MinimapArea.y - MinimapArea.x;
	RatioY = MinimapArea.w - MinimapArea.z;

	StatusArea.x = FRAME_BUFFER_WIDTH  / 2.2824f;
	StatusArea.y = FRAME_BUFFER_WIDTH  / 1.6177f;
	StatusArea.z = FRAME_BUFFER_HEIGHT / 1.2578f;
	StatusArea.w = FRAME_BUFFER_HEIGHT / 1.0471f;

	if(pKeyBuffer == MK_LBUTTON)
	{
		printf("%d, %d\n", cursorPos.x, cursorPos.y);

		// 미니맵 클릭
		if ((cursorPos.x > MinimapArea.x  && cursorPos.x < MinimapArea.y)
			&& (cursorPos.y > MinimapArea.z && cursorPos.y < MinimapArea.w))
		{
			XMFLOAT3 newCameraPos;
			newCameraPos.x = (cursorPos.x - MinimapArea.x) / RatioX * TERRAIN_SIZE_WIDTH;//* -(FRAME_BUFFER_WIDTH / 737.3f) * 9.f;
			newCameraPos.y = m_pCamera->GetPosition().y;
			newCameraPos.z = ((MinimapArea.w - cursorPos.y) / RatioY * TERRAIN_SIZE_HEIGHT) - 200.0f;
			
			m_pCamera->SetPosition(newCameraPos);
		}

		// 스테이터스 클릭
		if ((cursorPos.x > StatusArea.x  && cursorPos.x < StatusArea.y)
			&& (cursorPos.y > StatusArea.z && cursorPos.y < StatusArea.w))
		{
			printf("StatusArea Area\n");
		}
	}


	return true;
}

////////////////////////////////////////////////////////////////////////
// 내부함수
D3D12_INPUT_LAYOUT_DESC CUIObjectShader::CreateInputLayout()
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

D3D12_BLEND_DESC CUIObjectShader::CreateBlendState()
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

D3D12_SHADER_BYTECODE CUIObjectShader::CreateVertexShader(ID3DBlob ** ppShaderBlob)
{
	//./Code/04.Shaders/99.GraphicsShader/
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSTextured",
		"vs_5_1",
		ppShaderBlob));
}

D3D12_SHADER_BYTECODE CUIObjectShader::CreatePixelShader(ID3DBlob ** ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSTextured",
		"ps_5_1",
		ppShaderBlob));
}

void CUIObjectShader::CreateShader(CCreateMgr * pCreateMgr, UINT nRenderTargets)
{
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets);
}

void CUIObjectShader::CreateShaderVariables(CCreateMgr * pCreateMgr, int nBuffers)
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
}

void CUIObjectShader::BuildObjects(CCreateMgr * pCreateMgr, void * pContext)
{
	m_pCamera = (CCamera*)pContext;
	
	m_nObjects = 4;
	m_ppObjects = new CBaseObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 2);
	CreateShaderVariables(pCreateMgr, m_nObjects);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pConstBuffer, ncbElementBytes);
	
	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CUIObject *pUIObject{ NULL };

	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = Materials::CreateUIMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);

	for (int i = 0; i < m_nObjects; ++i)
	{
		pUIObject = new CUIObject(pCreateMgr, (UIType)i);
		pUIObject->SetCamera(m_pCamera);
		pUIObject->SetDistance(FRAME_BUFFER_WIDTH / 128);	 // distance 10
		pUIObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * i));

		m_ppObjects[i] = pUIObject;
	}
}

void CUIObjectShader::ReleaseShaderVariables()
{
	if (!m_pConstBuffer) return;

	m_pConstBuffer->Unmap(0, NULL);
	Safe_Release(m_pConstBuffer);

	CShader::ReleaseShaderVariables();
}

void CUIObjectShader::ReleaseObjects()
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
			if (m_ppMaterials[i]) delete m_ppMaterials[i];
		}
		Safe_Delete(m_ppMaterials);
	}
#endif
}
