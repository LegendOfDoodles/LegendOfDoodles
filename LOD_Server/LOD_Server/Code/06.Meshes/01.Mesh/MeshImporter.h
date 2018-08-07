#pragma once
#include "stdafx.h"

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
