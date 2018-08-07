#include "stdafx.h"
#include "HeightMapTerrain.h"
#include "06.Meshes/01.Mesh/Mesh.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "04.Shaders/02.TerrainShader/TerrainShader.h"

/// <summary>
/// ����: ���� ��¿� ������Ʈ
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-06-01
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CHeightMapTerrain::CHeightMapTerrain(shared_ptr<CCreateMgr> pCreateMgr, LPCTSTR pFileName,
	XMFLOAT3 xmf3Scale) : CBaseObject(pCreateMgr, 0)
{
	//������ ����� ���� ���� ����, ������ ũ���̴�.
	m_nWidth = static_cast<int>(TERRAIN_IMAGE_WIDTH);
	m_nLength = static_cast<int>(TERRAIN_IMAGE_HEIGHT);

	//xmf3Scale�� ������ ������ �� �� Ȯ���� ���ΰ��� ��Ÿ����.
	m_xmf3Scale = xmf3Scale;

	//������ ����� ���� ���� �����Ѵ�.
	m_pHeightMapImage = new CHeightMapImage(pFileName, m_nWidth, m_nLength, xmf3Scale);

	//���� ��ü�� ǥ���ϱ� ���� ���� �޽��� �����̴�.
	m_nMeshes = 1;

	//���� ��ü�� ǥ���ϱ� ���� ���� �޽��� ���� ������ �迭�� �����Ѵ�.
	m_ppMeshes = new CMesh*[m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++) m_ppMeshes[i] = NULL;

	CHeightMapGridMesh *pHeightMapGridMesh = new CHeightMapGridMesh(
		pCreateMgr,
		static_cast<int>(TERRAIN_IMAGE_CELL_WIDTH),
		static_cast<int>(TERRAIN_IMAGE_CELL_HEIGHT));
	SetMesh(0, pHeightMapGridMesh);
}

CHeightMapTerrain::~CHeightMapTerrain(void)
{
	Safe_Delete(m_pHeightMapImage);
}