#include "stdafx.h"
#include "Shader.h"

/// <summary>
/// ����: �⺻ ������ �ڵ�, �������̽� ��
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-05-09
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CShader::CShader()
{
}

CShader::~CShader()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CShader::Initialize(void *pContext)
{
	BuildObjects(pContext);
}

void CShader::Finalize()
{
	ReleaseObjects();
	Release();
}

void CShader::AnimateObjects(float timeElapsed)
{
}

bool CShader::OnProcessKeyInput(UCHAR* pKeyBuffer)
{
	return true;
}

bool CShader::OnProcessMouseInput(WPARAM pKeyBuffer)
{
	return false;
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CShader::BuildObjects(void *pContext)
{
}

void CShader::ReleaseObjects()
{
}
