#pragma once
#include <iostream>
#include <Windows.h>

namespace ExptProcess
{
	void PrintErrorBlob(ComPtr<ID3DBlob>& pErrorBlob);
	void ThrowIfFailed(HRESULT hr);
}