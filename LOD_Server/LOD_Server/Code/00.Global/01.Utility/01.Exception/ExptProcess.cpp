#include "stdafx.h"
#include "ExptProcess.h"

/// <summary>
/// ����: ���� ó�� ���� �Լ� �ۼ�
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-03-30
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