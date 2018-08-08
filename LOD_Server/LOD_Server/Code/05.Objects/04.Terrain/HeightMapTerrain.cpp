#include "stdafx.h"
#include "HeightMapTerrain.h"
#include "06.Meshes/01.Mesh/Mesh.h"

/// <summary>
/// ����: ���� ��¿� ������Ʈ
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-08-08
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CHeightMapTerrain::CHeightMapTerrain(LPCTSTR pFileName,
	XMFLOAT3 xmf3Scale) : CBaseObject()
{
	//������ ����� ���� ���� ����, ������ ũ���̴�.
	m_nWidth = static_cast<int>(TERRAIN_IMAGE_WIDTH);
	m_nLength = static_cast<int>(TERRAIN_IMAGE_HEIGHT);

	//xmf3Scale�� ������ ������ �� �� Ȯ���� ���ΰ��� ��Ÿ����.
	m_xmf3Scale = xmf3Scale;

	//������ ����� ���� ���� �����Ѵ�.
	m_pHeightMapImage = new CHeightMapImage(pFileName, m_nWidth, m_nLength, xmf3Scale);
}

CHeightMapTerrain::~CHeightMapTerrain(void)
{
	Safe_Delete(m_pHeightMapImage);
}