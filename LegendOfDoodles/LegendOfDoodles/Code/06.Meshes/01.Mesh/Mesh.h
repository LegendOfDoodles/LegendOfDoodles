#pragma once

class CCreateMgr;
class CSkinnedVertex;
////////////////////////////////////////////////////////////////////////
// 기본 메쉬
class CMesh
{
public: // 생성자, 소멸자
	CMesh(CCreateMgr *pCreateMgr);
	virtual ~CMesh();

public: // 공개 함수
	virtual void Render(UINT istanceCnt = 1);

	virtual void ReleaseUploadBuffers();

	bool CheckRayIntersection(XMFLOAT3& rayPosition, XMFLOAT3& rayDirection, float &nearHitDistance);

	void SetBoundingBox(XMFLOAT3& center, XMFLOAT3& extents);
	
	ID3D12Resource* GetVertexBuffer() { return m_pVertexBuffer; 	}
	ID3D12Resource* GetVertexUploadBuffer() { 	return m_pVertexUploadBuffer; }

	BoundingOrientedBox GetBoundingBox() { return(*m_pBoundingBox); }

	bool HasBoundingBox() { return m_pBoundingBox; }

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected: // 내부 함수

protected: // 변수
	int m_nReferences{ 0 };

	ID3D12Resource *m_pVertexBuffer{ NULL };
	ID3D12Resource *m_pVertexUploadBuffer{ NULL };

	ID3D12Resource *m_pIndexBuffer{ NULL };
	ID3D12Resource *m_pIndexUploadBuffer{ NULL };

	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

	D3D12_PRIMITIVE_TOPOLOGY m_primitiveTopology{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	UINT m_nVertices{ 0 };
	UINT m_nIndices{ 0 };

	UINT m_nSlot{ 0 };
	UINT m_nStride{ 0 };
	UINT m_nOffset{ 0 };

	UINT m_nStartIndex{ 0 };
	UINT m_nBaseVertex{ 0 };

	BoundingOrientedBox	 *m_pBoundingBox{ NULL };

	ID3D12GraphicsCommandList *m_pCommandList{ NULL };
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMeshTextured : public CMesh
{
public: // 생성자, 소멸자
	CMeshTextured(CCreateMgr *pCreateMgr);
	virtual ~CMeshTextured();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMeshIlluminated : public CMesh
{
public: // 생성자, 소멸자
	CMeshIlluminated(CCreateMgr *pCreateMgr);
	virtual ~CMeshIlluminated();

public: // 공개 함수
	void CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices);
	void CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices);
	void CalculateTriangleStripVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices);
	void CalculateVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices, UINT *pnIndices, int nIndices);

	void CalculateTriangleListVertexTangents(XMFLOAT3 *pxmf3Tangents, XMFLOAT3 *pxmf3Positions, UINT nVertices, XMFLOAT2 *xmf2TexCoord, UINT *pnIndices = NULL, UINT nIndices = NULL);
	void CalculateTriangleStripVertexTangents(XMFLOAT3 *pxmf3Tangents, XMFLOAT3 *pxmf3Positions, UINT nVertices, XMFLOAT2 *xmf2TexCoord, UINT *pnIndices = NULL, UINT nIndices = NULL);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMeshIlluminatedTextured : public CMeshIlluminated
{
public: // 생성자, 소멸자
	CMeshIlluminatedTextured(CCreateMgr *pCreateMgr);
	CMeshIlluminatedTextured(CCreateMgr *pCreateMgr, UINT nVertices, XMFLOAT3 *pxmf3Positions, XMFLOAT3 *pxmf3Normals, XMFLOAT2 *pxmf2UVs, UINT nIndices, UINT *pnIndices);
	virtual ~CMeshIlluminatedTextured();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSkinnedMesh : public CMeshIlluminatedTextured
{
public: // 생성자, 소멸자
	CSkinnedMesh(CCreateMgr* pCreateMgr, char* in);
	virtual ~CSkinnedMesh();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

class CStaticMesh : public CMeshIlluminatedTextured
{
public: // 생성자, 소멸자
	CStaticMesh(CCreateMgr* pCreateMgr, char* in, XMFLOAT3 scalevalue = XMFLOAT3(1,1,1));
	virtual ~CStaticMesh();
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

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
	BYTE * m_pHeightMapPixels;
	//높이 맵 이미지의 가로와 세로 크기이다.
	int m_nWidth;
	int m_nLength;
	//높이 맵 이미지를 실제로 몇 배 확대하여 사용할 것인가를 나타내는 스케일 벡터이다.
	XMFLOAT3 m_xmf3Scale;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CHeightMapGridMesh : public CMesh
{
public: // 생성자, 소멸자
	CHeightMapGridMesh(CCreateMgr *pCreateMgr, int nWidth, int nLength);
	virtual ~CHeightMapGridMesh();

public: // 공개 함수

protected: // 변수

};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CTexturedRectMesh : public CMesh
{
public: // 생성자, 소멸자
	CTexturedRectMesh(CCreateMgr *pCreateMgr, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 20.0f, float fxPosition = 0.0f, float fyPosition = 0.0f, float fzPosition = 0.0f);
	virtual ~CTexturedRectMesh();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CArrowMesh : public CMesh
{
public: // 생성자, 소멸자
	CArrowMesh(CCreateMgr *pCreateMgr, float length = 100.0f);
	virtual ~CArrowMesh();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CCubeMesh : public CMesh
{
public: // 생성자, 소멸자
	//직육면체의 가로, 세로, 깊이의 길이를 지정하여 직육면체 메쉬를 생성한다.
	CCubeMesh(CCreateMgr *pCreateMgr, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f, float xOffset = 0.0f, float yOffSet = 0.0f, float zOffSet = 0.0f);
	virtual ~CCubeMesh();
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
	BYTE * m_pCollisionMapPixels;
	int m_nWidth;
	int m_nLength;

	XMFLOAT3 m_xmf3Scale;
};