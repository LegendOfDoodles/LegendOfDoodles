#include "stdafx.h"
#include "ExptProcess.h"

/// <summary>
/// 목적: 예외 처리 관련 함수 작성
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-03-30
/// </summary>

void ExptProcess::PrintErrorBlob(ComPtr<ID3DBlob>& pErrorBlob)
{
	if (!pErrorBlob) return;

	LPVOID pValue = pErrorBlob->GetBufferPointer();
	size_t size = pErrorBlob->GetBufferSize();

	char *pData = new char[size];
	sprintf_s(pData, size, (const char*)pValue);
	OutputDebugStringA(pData);

	delete[] pData;
}

void ExptProcess::ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw std::exception();
	}
}