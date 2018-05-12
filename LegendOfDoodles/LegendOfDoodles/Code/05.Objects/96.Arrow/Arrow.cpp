#include "stdafx.h"
#include "Arrow.h"

/// <summary>
/// ����: �۾� �� x, y, z ���� ��� �� ȭ��ǥ ������Ʈ
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-05-04
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CArrow::CArrow(CCreateMgr *pCreateMgr, float length) : CBaseObject(pCreateMgr)
{
	CArrowMesh *pArrowMesh = new CArrowMesh(pCreateMgr, length);

	SetMesh(0, pArrowMesh);

	CreateShaderVariables(pCreateMgr);
}

CArrow::~CArrow()
{
}
