#include "stdafx.h"
#include"SafeFunc.h"

/// <summary>
/// ����: ���α׷��� �� �����ϰ� ����� �Լ� ���� ����� �����ϱ� ���� �뵵
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-02-02
/// </summary>

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
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