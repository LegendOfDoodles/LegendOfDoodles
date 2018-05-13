#pragma once

/// <summary>
/// 목적: Release 및 Delete 이후 해당 변수에 NULL을 대입하여 오류를 방지한다.
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-01-12
/// </summary>

#define NULL 0

template <typename T> 
inline void Safe_Delete(T& p)
{
	if (!p) return;

	delete p;
	p = NULL;
}

template <typename T> 
inline void Safe_Delete_Array(T& p)
{
	if (!p) return;

	delete[] p;
	p = NULL;
}

template <typename T> 
inline void Safe_Release(T& p)
{
	if (!p) return;

	p->Release();
	p = NULL;
}

////////////////////////////////////////////////////////////////////////
// 전역 함수
D3D12_RESOURCE_BARRIER CreateResourceBarrier(
	ID3D12Resource *pBuffer,
	D3D12_RESOURCE_STATES before,
	D3D12_RESOURCE_STATES after);