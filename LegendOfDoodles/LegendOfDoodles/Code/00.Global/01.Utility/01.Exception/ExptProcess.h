#pragma once
#include <iostream>
#include <Windows.h>

#define ToWString(x) L#x

class DxException
{
public:
	DxException() = default;
	DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

	std::wstring ToString()const;

	HRESULT ErrorCode = S_OK;
	std::wstring FunctionName;
	std::wstring Filename;
	int LineNumber = -1;
};

inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}

void PrintErrorBlob(ComPtr<ID3DBlob> pErrorBlob);

#define ThrowIfFailed(x)										\
{																\
	HRESULT _hResult = (x);										\
	if(FAILED(_hResult))										\
	{															\
		std::wstring wfn = AnsiToWString(__FILE__);				\
		throw DxException(_hResult, L#x, wfn, __LINE__);		\
	}															\
}

//void Trace(char* szFormat, ...);