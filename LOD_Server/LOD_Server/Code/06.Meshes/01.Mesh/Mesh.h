#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CHeightMapImage
{
public: // 생성자, 소멸자
	CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	virtual ~CHeightMapImage(void);

public: // 공개 함수
		//높이 맵 이미지에서 (x, z) 위치의 픽셀 값에 기반한 지형의 높이를 반환한다.
	float GetHeight(float x, float z);
	//높이 맵 이미지에서 (x, z) 위치의 법선 벡터를 반환한다.
	XMFLOAT3 GetHeightMapNormal(int x, int z);

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }

	BYTE *GetHeightMapPixels() { return(m_pHeightMapPixels); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }

private: // 변수
		 //높이 맵 이미지 픽셀(8-비트)들의 이차원 배열이다. 각 픽셀은 0~255의 값을 갖는다.
	BYTE * m_pHeightMapPixels{ NULL };
	//높이 맵 이미지의 가로와 세로 크기이다.
	int m_nWidth{ 0 };
	int m_nLength{ 0 };
	//높이 맵 이미지를 실제로 몇 배 확대하여 사용할 것인가를 나타내는 스케일 벡터이다.
	XMFLOAT3 m_xmf3Scale{};
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CCollisionMapImage
{
public: // 생성자, 소멸자
	CCollisionMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	virtual ~CCollisionMapImage(void);

public: // 공개 함수
	bool GetCollision(float x, float z);

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }

	BYTE *GetCollisionMapPixels() { return(m_pCollisionMapPixels); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }

private: // 변수
	BYTE * m_pCollisionMapPixels{ NULL };
	int m_nWidth{ 0 };
	int m_nLength{ 0 };

	XMFLOAT3 m_xmf3Scale{};
};