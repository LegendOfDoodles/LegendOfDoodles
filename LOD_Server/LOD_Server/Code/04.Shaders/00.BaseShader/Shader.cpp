#include "stdafx.h"
#include "Shader.h"

/// <summary>
/// ����: �⺻ ������ �ڵ�, �������̽� ��
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-09-11
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
	UNREFERENCED_PARAMETER(timeElapsed);
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CShader::BuildObjects(void *pContext)
{
	UNREFERENCED_PARAMETER(pContext);
}

void CShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			delete m_ppObjects[j];
		}
		Safe_Delete_Array(m_ppObjects);
	}
}
