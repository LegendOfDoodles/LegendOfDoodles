#include "stdafx.h"
#include "HeightMapTerrain.h"
#include "06.Meshes/01.Mesh/Mesh.h"

/// <summary>
/// 목적: 지형 출력용 오브젝트
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-08-08
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CHeightMapTerrain::CHeightMapTerrain(LPCTSTR pFileName,
	XMFLOAT3 xmf3Scale) : CBaseObject()
{
	//지형에 사용할 높이 맵의 가로, 세로의 크기이다.
	m_nWidth = static_cast<int>(TERRAIN_IMAGE_WIDTH);
	m_nLength = static_cast<int>(TERRAIN_IMAGE_HEIGHT);

	//xmf3Scale는 지형을 실제로 몇 배 확대할 것인가를 나타낸다.
	m_xmf3Scale = xmf3Scale;

	//지형에 사용할 높이 맵을 생성한다.
	m_pHeightMapImage = new CHeightMapImage(pFileName, m_nWidth, m_nLength, xmf3Scale);
}

CHeightMapTerrain::~CHeightMapTerrain(void)
{
	Safe_Delete(m_pHeightMapImage);
}