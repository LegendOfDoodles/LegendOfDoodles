#include "stdafx.h"
#include "Billboard.h"

/// <summary>
/// ����: UI �̿��� ���� Bilboard Ŭ���� ����
/// ���� ������:  �質��
/// ������ ���:  �̿뼱, �質��
/// ���� ���� ��¥: 2018-05-22
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CBillboardObject::CBillboardObject(shared_ptr<CCreateMgr> pCreateMgr) : CBaseObject(pCreateMgr)
{
}

CBillboardObject::~CBillboardObject()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CBillboardObject::Animate(float fTimeElapsed)
{
	UNREFERENCED_PARAMETER(fTimeElapsed);

	if (m_pCamera != NULL) {
		SetLookAt();
	}
}

void CBillboardObject::SetLookAt()
{
	// Up Vector�� 1�� �����ϰ� ���
	XMFLOAT3 xmf3Up = m_pCamera->GetUpVector();
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT3 xmf3Look = Vector3::ScalarProduct(m_pCamera->GetLookVector(), -1);
	XMFLOAT3 xmf3Right = Vector3::ScalarProduct(m_pCamera->GetRightVector(), -1);

	// ���� Up ���͸� ī�޶��� �������� �����Ѵ�.
	//xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf4x4World._11 = xmf3Right.x;	m_xmf4x4World._12 = xmf3Right.y;	m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x;		m_xmf4x4World._22 = xmf3Up.y;		m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x;		m_xmf4x4World._32 = xmf3Look.y;
}

