#include "stdafx.h"
#include "Shader.h"

/// <summary>
/// 목적: 기본 쉐이터 코드, 인터페이스 용
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-09-11
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CShader::CShader()
{
}

CShader::~CShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
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
// 내부 함수
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
