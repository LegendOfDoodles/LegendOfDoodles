#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"

struct CB_TERRAIN_INFO
{
	XMFLOAT4X4					m_xmf4x4World;
};

class CHeightMapTerrain : public CBaseObject
{
public: // 생성자, 소멸자
	CHeightMapTerrain(CCreateMgr *pCreateMgr, LPCTSTR pFileName, XMFLOAT3 xmf3Scale);
	virtual ~CHeightMapTerrain();

public: // 공개 함수
	//지형의 높이를 계산하는 함수이다(월드 좌표계). 높이 맵의 높이에 스케일의 y를 곱한 값이다.
	float GetHeight(float x, float z) {
		return(m_pHeightMapImage->GetHeight(x / m_xmf3Scale.x, z / m_xmf3Scale.z) * m_xmf3Scale.y - REVISE_HEIGHT_FOR_OBJECT);
	}

	//지형의 법선 벡터를 계산하는 함수이다(월드 좌표계). 높이 맵의 법선 벡터를 사용한다.
	XMFLOAT3 GetNormal(float x, float z) {
		return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z /
			m_xmf3Scale.z)));
	}

protected: // 내부 함수

private:	 // 변수
	 //지형의 높이 맵으로 사용할 이미지이다.
	CHeightMapImage *m_pHeightMapImage;
	//높이 맵의 가로와 세로 크기이다.
	int m_nWidth;
	int m_nLength;
	//지형을 실제로 몇 배 확대할 것인가를 나타내는 스케일 벡터이다.
	XMFLOAT3 m_xmf3Scale;
};