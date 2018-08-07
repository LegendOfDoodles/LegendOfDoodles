#include "stdafx.h"
#include <comdef.h>
#include "ExptProcess.h"

/// <summary>
/// ����: ���� ó�� ���� �Լ� �ۼ�
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-07-03
/// </summary>

DxException::DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber) :
	ErrorCode(hr),
	FunctionName(functionName),
	Filename(filename),
	LineNumber(lineNumber)
{
}

std::wstring DxException::ToString()const
{
	// Get the string description of the error code.
	_com_error err(ErrorCode);
	std::wstring msg = err.ErrorMessage();
	std::wstring fullMessage{ FunctionName + L" failed in " + Filename + L", line " + std::to_wstring(LineNumber) + L"\n" + L"error: " + msg };
	OutputDebugStringW(fullMessage.c_str());
	return fullMessage;
}

void PrintErrorBlob(ComPtr<ID3DBlob> pErrorBlob)
{
	OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
}