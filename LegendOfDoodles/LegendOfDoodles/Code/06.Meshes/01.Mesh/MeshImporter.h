#pragma once
#include "stdafx.h"
#include <Vector>
struct MyVertex {
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT4 skinweight;
	XMFLOAT4 skinindex;
	XMFLOAT2 uv;
	~MyVertex() {}
};

class CMeshImporter
{
public:
	int m_iTriCnt;
	std::vector<XMFLOAT3> m_xmTriIndex;
	int m_iVerticesCnt;
	std::vector<MyVertex> m_xmVertex;
public:
	CMeshImporter();

	void LoadMeshData(char* in);
	void LoadStaticMeshData(char* in);

	~CMeshImporter();

};

struct Transform {
	XMFLOAT3 pos;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;
};
class CTransformImporter
{
public:
	CTransformImporter();
	~CTransformImporter();

public:
	void LoadMeshData(char* in);

public:
	int m_iTotalCnt;
	std::vector<Transform> m_Transform;
	int m_iKindMeshCnt[17];
	XMFLOAT3 BookScale[4];
};
