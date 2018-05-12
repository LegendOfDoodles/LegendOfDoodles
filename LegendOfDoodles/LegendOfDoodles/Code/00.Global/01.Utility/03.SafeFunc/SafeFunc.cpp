#include "stdafx.h"
#include"SafeFunc.h"

/// <summary>
/// 목적: 프로그램을 더 안전하게 만드는 함수 등을 만들어 관리하기 위한 용도
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-02-02
/// </summary>

////////////////////////////////////////////////////////////////////////
// 전역 함수
D3D12_RESOURCE_BARRIER CreateResourceBarrier(
	ID3D12Resource *pBuffer,
	D3D12_RESOURCE_STATES before,
	D3D12_RESOURCE_STATES after)
{
	D3D12_RESOURCE_BARRIER resourceBarrier;
	::ZeroMemory(&resourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resourceBarrier.Transition.pResource = pBuffer;
	resourceBarrier.Transition.StateBefore = before;
	resourceBarrier.Transition.StateAfter = after;
	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	return(resourceBarrier);
}