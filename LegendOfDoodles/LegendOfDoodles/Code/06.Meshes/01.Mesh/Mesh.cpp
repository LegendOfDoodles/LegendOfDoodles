#include "stdafx.h"
#include "Mesh.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "06.Meshes/00.Vertex/Vertex.h"
#include "MeshImporter.h"
#include "05.Objects/97.Skeleton/Skeleton.h"
/// <summary>
/// 목적: 테스트 용 메쉬 클래스 생성
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-08-29
/// </summary>

////////////////////////////////////////////////////////////////////////
// 기본 메쉬
////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CMesh::CMesh(shared_ptr<CCreateMgr> pCreateMgr)
{
	m_pCommandList = pCreateMgr->GetCommandList();
}

CMesh::~CMesh()
{
	Safe_Delete(m_pBoundingBox);
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CMesh::ReleaseUploadBuffers()
{
	m_pVertexUploadBuffer.Reset();
	m_pIndexUploadBuffer.Reset();
}

void CMesh::Render(UINT istanceCnt)
{
	m_pCommandList->IASetPrimitiveTopology(m_primitiveTopology);
	m_pCommandList->IASetVertexBuffers(m_nSlot, 1, &m_vertexBufferView);
	if (m_pIndexBuffer)
	{
		m_pCommandList->IASetIndexBuffer(&m_indexBufferView);
		m_pCommandList->DrawIndexedInstanced(m_nIndices, istanceCnt, 0, 0, 0);
	}
	else
	{
		m_pCommandList->DrawInstanced(m_nVertices, istanceCnt, m_nOffset, 0);
	}
}

bool CMesh::CheckRayIntersection(XMFLOAT3& xmf3RayOrigin, XMFLOAT3& xmf3RayDirection, float &nearHitDistance)
{
	if (!m_pBoundingBox) return false;

	bool bIntersected{ false };
	XMVECTOR xmRayOrigin{ XMLoadFloat3(&xmf3RayOrigin) };
	XMVECTOR xmRayDirection{ XMLoadFloat3(&xmf3RayDirection) };

	bIntersected = m_pBoundingBox->Intersects(xmRayOrigin, xmRayDirection, nearHitDistance);

	return(bIntersected);
}

void CMesh::SetBoundingBox(const XMFLOAT3& center, const XMFLOAT3 & extents)
{
	m_pBoundingBox = new BoundingOrientedBox(center, extents, XMFLOAT4(0, 0, 0, 1));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMeshTextured
////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CMeshTextured::CMeshTextured(shared_ptr<CCreateMgr> pCreateMgr)
	: CMesh(pCreateMgr)
{
}

CMeshTextured::~CMeshTextured()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMeshIlluminated
////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CMeshIlluminated::CMeshIlluminated(shared_ptr<CCreateMgr> pCreateMgr) : CMesh(pCreateMgr)
{
}

CMeshIlluminated::~CMeshIlluminated()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CMeshIlluminated::CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices)
{
	int nPrimitives = nVertices / 3;
	UINT nIndex0, nIndex1, nIndex2;
	for (int i = 0; i < nPrimitives; i++)
	{
		nIndex0 = i * 3 + 0;
		nIndex1 = i * 3 + 1;
		nIndex2 = i * 3 + 2;
		XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
		XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
		pxmf3Normals[nIndex0] = pxmf3Normals[nIndex1] = pxmf3Normals[nIndex2] = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
	}
}

void CMeshIlluminated::CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices / 3) : (nVertices / 3);
	XMFLOAT3 xmf3SumOfNormal, xmf3Edge01, xmf3Edge02, xmf3Normal;
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = pnIndices[i * 3 + 0];
			nIndex1 = pnIndices[i * 3 + 1];
			nIndex2 = pnIndices[i * 3 + 2];
			if (pnIndices && ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j)))
			{
				xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
				xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
				xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, false);
				xmf3SumOfNormal = Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}

void CMeshIlluminated::CalculateTriangleStripVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices - 2) : (nVertices - 2);
	XMFLOAT3 xmf3SumOfNormal(0.0f, 0.0f, 0.0f);
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = ((i % 2) == 0) ? (i + 0) : (i + 1);
			if (pnIndices) nIndex0 = pnIndices[nIndex0];
			nIndex1 = ((i % 2) == 0) ? (i + 1) : (i + 0);
			if (pnIndices) nIndex1 = pnIndices[nIndex1];
			nIndex2 = (pnIndices) ? pnIndices[i + 2] : (i + 2);
			if ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j))
			{
				XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
				xmf3SumOfNormal = Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}

void CMeshIlluminated::CalculateVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices, UINT *pnIndices, int nIndices)
{
	switch (m_primitiveTopology)
	{
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		if (pnIndices)
			CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices, pnIndices, nIndices);
		else
			CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices);
		break;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		CalculateTriangleStripVertexNormals(pxmf3Normals, pxmf3Positions, nVertices, pnIndices, nIndices);
		break;
	default:
		break;
	}
}

void CMeshIlluminated::CalculateTriangleListVertexTangents(XMFLOAT3 *pxmf3Tangents, XMFLOAT3 *pxmf3Positions, UINT nVertices,
	XMFLOAT2 *xmf2TexCoord, UINT *pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices / 3) : (nVertices / 3);
	XMFLOAT3 xmf3SumOfTangent(0.0f, 0.0f, 0.0f);
	UINT nIndex0, nIndex1, nIndex2;
	float deltaU0, deltaU1;
	float deltaV0, deltaV1;

	if (pnIndices)
	{
		for (UINT v = 0; v < nVertices; v++)
		{
			xmf3SumOfTangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
			for (UINT i = 0; i < nPrimitives; i++)
			{
				if (pnIndices[i * 3] == v)
				{
					nIndex0 = pnIndices[i * 3];		nIndex1 = pnIndices[i * 3 + 1];		nIndex2 = pnIndices[i * 3 + 2];
				}
				else if (pnIndices[i * 3 + 1] == v)
				{
					nIndex0 = pnIndices[i * 3 + 1];		nIndex1 = pnIndices[i * 3 + 2];		nIndex2 = pnIndices[i * 3];
				}
				else if (pnIndices[i * 3 + 2] == v)
				{
					nIndex0 = pnIndices[i * 3 + 2];		nIndex1 = pnIndices[i * 3];		nIndex2 = pnIndices[i * 3 + 1];
				}
				else { continue; }

				deltaU0 = xmf2TexCoord[nIndex1].x - xmf2TexCoord[nIndex0].x;
				deltaU1 = xmf2TexCoord[nIndex2].x - xmf2TexCoord[nIndex0].x;
				deltaV0 = xmf2TexCoord[nIndex1].y - xmf2TexCoord[nIndex0].y;
				deltaV1 = xmf2TexCoord[nIndex2].y - xmf2TexCoord[nIndex0].y;

				if (deltaU0 * deltaV1 == deltaU1 * deltaV0) continue;

				float proVal = 1.0f / abs(deltaU0 * deltaV1 - deltaU1 * deltaV0);

				XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Tangent(
					proVal * (deltaV1 * xmf3Edge01.x - deltaV0 * xmf3Edge02.x),
					proVal * (deltaV1 * xmf3Edge01.y - deltaV0 * xmf3Edge02.y),
					proVal * (deltaV1 * xmf3Edge01.z - deltaV0 * xmf3Edge02.z)
				);

				xmf3SumOfTangent = Vector3::Add(xmf3SumOfTangent, xmf3Tangent);
			}
			pxmf3Tangents[v] = Vector3::Normalize(xmf3SumOfTangent);
		}
	}
	else
	{
		for (UINT i = 0; i < nPrimitives; i++)
		{
			for (int j = 0, k = 1, l = 2; j < 3; ++j, ++k, ++l)
			{
				k = Wrap(k, 0, 3);
				l = Wrap(l, 0, 3);

				nIndex0 = i * 3 + j;		nIndex1 = i * 3 + k;		nIndex2 = i * 3 + l;

				deltaU0 = xmf2TexCoord[nIndex1].x - xmf2TexCoord[nIndex0].x;
				deltaU1 = xmf2TexCoord[nIndex2].x - xmf2TexCoord[nIndex0].x;
				deltaV0 = xmf2TexCoord[nIndex1].y - xmf2TexCoord[nIndex0].y;
				deltaV1 = xmf2TexCoord[nIndex2].y - xmf2TexCoord[nIndex0].y;

				if (deltaU0 * deltaV1 == deltaU1 * deltaV0) continue;

				float proVal = 1.0f / abs(deltaU0 * deltaV1 - deltaU1 * deltaV0);

				XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Tangent(
					proVal * (deltaV1 * xmf3Edge01.x - deltaV0 * xmf3Edge02.x),
					proVal * (deltaV1 * xmf3Edge01.y - deltaV0 * xmf3Edge02.y),
					proVal * (deltaV1 * xmf3Edge01.z - deltaV0 * xmf3Edge02.z)
				);

				pxmf3Tangents[nIndex0] = xmf3Tangent;
			}
		}
	}
}

void CMeshIlluminated::CalculateTriangleStripVertexTangents(XMFLOAT3 *pxmf3Tangents, XMFLOAT3 *pxmf3Positions, UINT nVertices,
	XMFLOAT2 *xmf2TexCoord, UINT *pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices - 2) : (nVertices - 2);
	XMFLOAT3 xmf3SumOfTangent(0.0f, 0.0f, 0.0f);
	UINT nIndex0, nIndex1, nIndex2;
	float deltaU0, deltaU1;
	float deltaV0, deltaV1;

	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfTangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = ((i % 2) == 0) ? (i + 0) : (i + 1);
			if (pnIndices) nIndex0 = pnIndices[nIndex0];
			nIndex1 = ((i % 2) == 0) ? (i + 1) : (i + 0);
			if (pnIndices) nIndex1 = pnIndices[nIndex1];
			nIndex2 = (pnIndices) ? pnIndices[i + 2] : (i + 2);

			if (nIndex0 == j) {}
			else if (nIndex1 == j)
			{
				int temp = nIndex0;
				nIndex0 = nIndex1;
				nIndex1 = nIndex2;
				nIndex2 = temp;
			}
			else if (nIndex2 == j)
			{
				int temp = nIndex0;
				nIndex0 = nIndex2;
				nIndex2 = nIndex1;
				nIndex1 = temp;
			}
			else { continue; }

			deltaU0 = xmf2TexCoord[nIndex1].x - xmf2TexCoord[nIndex0].x;
			deltaU1 = xmf2TexCoord[nIndex2].x - xmf2TexCoord[nIndex0].x;
			deltaV0 = xmf2TexCoord[nIndex1].y - xmf2TexCoord[nIndex0].y;
			deltaV1 = xmf2TexCoord[nIndex2].y - xmf2TexCoord[nIndex0].y;

			if (deltaU0 * deltaV1 == deltaU1 * deltaV0) continue;

			float proVal = 1.0f / abs(deltaU0 * deltaV1 - deltaU1 * deltaV0);

			XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
			XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
			XMFLOAT3 xmf3Tangent(
				proVal * (deltaV1 * xmf3Edge01.x - deltaV0 * xmf3Edge02.x),
				proVal * (deltaV1 * xmf3Edge01.y - deltaV0 * xmf3Edge02.y),
				proVal * (deltaV1 * xmf3Edge01.z - deltaV0 * xmf3Edge02.z)
			);
			xmf3SumOfTangent = Vector3::Add(xmf3SumOfTangent, xmf3Tangent);
		}
		pxmf3Tangents[j] = Vector3::Normalize(xmf3SumOfTangent);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMeshIlluminatedTextured
////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CMeshIlluminatedTextured::CMeshIlluminatedTextured(shared_ptr<CCreateMgr> pCreateMgr) : CMeshIlluminated(pCreateMgr)
{
}

CMeshIlluminatedTextured::CMeshIlluminatedTextured(shared_ptr<CCreateMgr> pCreateMgr, UINT nVertices, XMFLOAT3 *pxmf3Positions, XMFLOAT3 *pxmf3Normals, XMFLOAT2 *pxmf2UVs, UINT nIndices, UINT *pnIndices) : CMeshIlluminated(pCreateMgr)
{
	m_nStride = sizeof(CIlluminatedTexturedVertex);
	m_nVertices = nVertices;

	CIlluminatedTexturedVertex *pVertices = new CIlluminatedTexturedVertex[m_nVertices];
	for (UINT i = 0; i < m_nVertices; i++) pVertices[i] = CIlluminatedTexturedVertex(pxmf3Positions[i], pxmf3Normals[i], pxmf2UVs[i]);

	m_pVertexBuffer = pCreateMgr->CreateBufferResource(
		pVertices,
		m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		m_pVertexUploadBuffer.GetAddressOf());

	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = m_nStride;
	m_vertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	if (nIndices > 0)
	{
		m_nIndices = nIndices;

		m_pIndexBuffer = pCreateMgr->CreateBufferResource(
			pnIndices,
			sizeof(UINT) * m_nIndices,
			D3D12_HEAP_TYPE_DEFAULT,
			D3D12_RESOURCE_STATE_INDEX_BUFFER,
			m_pIndexUploadBuffer.GetAddressOf());

		m_indexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
		m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_indexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
	}
}

CMeshIlluminatedTextured::~CMeshIlluminatedTextured()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CSkinnedMesh
////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CSkinnedMesh::CSkinnedMesh(shared_ptr<CCreateMgr> pCreateMgr, char* in) : CMeshIlluminatedTextured(pCreateMgr)
{
	CMeshImporter importer;
	importer.LoadMeshData(in);
	m_nVertices = importer.m_iVerticesCnt;
	m_nStride = sizeof(CSkinnedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_nIndices = importer.m_iTriCnt * 3;
	UINT* pnIndices = new UINT[m_nIndices];
	int indicesCount = 0;
	for (auto d : importer.m_xmTriIndex) {
		pnIndices[indicesCount] = static_cast<UINT>(d.x);
		pnIndices[indicesCount + 1] = static_cast<UINT>(d.y);
		pnIndices[indicesCount + 2] = static_cast<UINT>(d.z);
		indicesCount += 3;
	}

	m_pIndexBuffer = pCreateMgr->CreateBufferResource(
		pnIndices,
		sizeof(UINT) * m_nIndices,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_INDEX_BUFFER,
		m_pIndexUploadBuffer.GetAddressOf());

	m_indexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	XMFLOAT3* pxmf3Positions = new XMFLOAT3[m_nVertices];
	XMFLOAT3* pxmf3Normals = new XMFLOAT3[m_nVertices];

	XMFLOAT2* pxmf2TexCoords = new XMFLOAT2[m_nVertices];
	XMFLOAT3* pxmf4SkinWeight = new XMFLOAT3[m_nVertices];
	XMFLOAT4* pxmf4SkinIndex = new XMFLOAT4[m_nVertices];


	int vecticesCount = 0;
	for (auto d : importer.m_xmVertex) {
		pxmf3Normals[vecticesCount] = XMFLOAT3(d.normal.x, d.normal.y, d.normal.z);
		pxmf2TexCoords[vecticesCount] = XMFLOAT2(d.uv.x, d.uv.y);
		pxmf4SkinWeight[vecticesCount] = XMFLOAT3(d.skinweight.x, d.skinweight.y, d.skinweight.z);
		pxmf4SkinIndex[vecticesCount] = XMFLOAT4(d.skinindex.x, d.skinindex.y, d.skinindex.z, d.skinindex.w);
		pxmf3Positions[vecticesCount] = XMFLOAT3(d.pos.x, d.pos.y, d.pos.z);
		++vecticesCount;
	}

	XMFLOAT3* pxmf3Tangents = new XMFLOAT3[m_nVertices];
	CalculateTriangleListVertexTangents(pxmf3Tangents, pxmf3Positions, m_nVertices, pxmf2TexCoords, pnIndices, m_nIndices);

	CSkinnedVertex *pVertices = new CSkinnedVertex[m_nVertices];
	for (UINT i = 0; i < m_nVertices; i++) {
		BYTE index[4];

		index[0] = (BYTE)pxmf4SkinIndex[i].x;
		index[1] = (BYTE)pxmf4SkinIndex[i].y;
		index[2] = (BYTE)pxmf4SkinIndex[i].z;
		index[3] = (BYTE)pxmf4SkinIndex[i].w;
		pVertices[i] = CSkinnedVertex(pxmf3Positions[i], index, pxmf3Normals[i], pxmf3Tangents[i], pxmf2TexCoords[i], pxmf4SkinWeight[i]);
	}
	m_pVertexBuffer = pCreateMgr->CreateBufferResource(
		pVertices,
		m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		m_pVertexUploadBuffer.GetAddressOf());

	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = m_nStride;
	m_vertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	delete[](pxmf3Positions);
	delete[](pnIndices);
	delete[](pxmf3Normals);
	delete[](pxmf2TexCoords);
	delete[](pxmf4SkinWeight);
	delete[](pxmf4SkinIndex);
	delete[](pxmf3Tangents);
}

CSkinnedMesh::~CSkinnedMesh()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CStaticMesh
////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CStaticMesh::CStaticMesh(shared_ptr<CCreateMgr> pCreateMgr, char * in, XMFLOAT3 scalevalue) : CMeshIlluminatedTextured(pCreateMgr)
{
	CMeshImporter importer;
	importer.LoadStaticMeshData(in);
	m_nVertices = importer.m_iVerticesCnt;
	m_nStride = sizeof(CIlluminatedTexturedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_nIndices = importer.m_iTriCnt * 3;
	UINT* pnIndices = new UINT[m_nIndices];
	int indicesCount = 0;
	for (auto d : importer.m_xmTriIndex) {
		pnIndices[indicesCount] = static_cast<UINT>(d.x);
		pnIndices[indicesCount + 1] = static_cast<UINT>(d.y);
		pnIndices[indicesCount + 2] = static_cast<UINT>(d.z);
		indicesCount += 3;
	}

	m_pIndexBuffer = pCreateMgr->CreateBufferResource(
		pnIndices,
		sizeof(UINT) * m_nIndices,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_INDEX_BUFFER,
		m_pIndexUploadBuffer.GetAddressOf());

	m_indexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	XMFLOAT3* pxmf3Positions = new XMFLOAT3[m_nVertices];
	XMFLOAT3* pxmf3Normals = new XMFLOAT3[m_nVertices];

	XMFLOAT2* pxmf2TexCoords = new XMFLOAT2[m_nVertices];

	int vecticesCount = 0;
	for (auto d : importer.m_xmVertex) {
		pxmf3Normals[vecticesCount] = XMFLOAT3(d.normal.x, d.normal.y, d.normal.z);
		pxmf2TexCoords[vecticesCount] = XMFLOAT2(d.uv.x, d.uv.y);
		pxmf3Positions[vecticesCount] = XMFLOAT3(d.pos.x*scalevalue.x, d.pos.y*scalevalue.y, d.pos.z*scalevalue.z);
		++vecticesCount;
	}

	XMFLOAT3* pxmf3Tangents = new XMFLOAT3[m_nVertices];
	CalculateTriangleListVertexTangents(pxmf3Tangents, pxmf3Positions, m_nVertices, pxmf2TexCoords, pnIndices, m_nIndices);

	CIlluminatedTexturedVertex *pVertices = new CIlluminatedTexturedVertex[m_nVertices];
	for (UINT i = 0; i < m_nVertices; i++) {
		pVertices[i] = CIlluminatedTexturedVertex(pxmf3Positions[i], pxmf3Normals[i], pxmf2TexCoords[i], pxmf3Tangents[i]);
	}
	m_pVertexBuffer = pCreateMgr->CreateBufferResource(
		pVertices,
		m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		m_pVertexUploadBuffer.GetAddressOf());

	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = m_nStride;
	m_vertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	delete[](pxmf3Positions);
	delete[](pnIndices);
	delete[](pxmf3Normals);
	delete[](pxmf2TexCoords);
	delete[](pxmf3Tangents);
}

CStaticMesh::~CStaticMesh()
{
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHeightMapImage
////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CHeightMapImage::CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale)
{
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;
	BYTE *pHeightMapPixels = new BYTE[m_nWidth * m_nLength];
	//파일을 열고 읽는다. 높이 맵 이미지는 파일 헤더가 없는 RAW 이미지이다.
	HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead;
	::ReadFile(hFile, pHeightMapPixels, (m_nWidth * m_nLength), &dwBytesRead, NULL);
	::CloseHandle(hFile);

	/*이미지의 y-축과 지형의 z-축이 방향이 반대이므로 이미지를 상하대칭 시켜 저장한다. 그러면 다음 그림과 같이 이
	미지의 좌표축과 지형의 좌표축의 방향이 일치하게 된다.*/

	m_pHeightMapPixels = new BYTE[m_nWidth * m_nLength];
	for (int y = 0; y < m_nLength; y++)
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			m_pHeightMapPixels[x + ((m_nLength - 1 - y)*m_nWidth)] = pHeightMapPixels[x + (y*m_nWidth)];
		}
	}

	if (pHeightMapPixels) Safe_Delete_Array(pHeightMapPixels);
}

CHeightMapImage::~CHeightMapImage()
{
	if (m_pHeightMapPixels) Safe_Delete_Array(m_pHeightMapPixels);
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
XMFLOAT3 CHeightMapImage::GetHeightMapNormal(int x, int z)
{
	//x-좌표와 z-좌표가 높이 맵의 범위를 벗어나면 지형의 법선 벡터는 y-축 방향 벡터이다.
	if ((x < 0.0f) || (z < 0.0f) || (x >= m_nWidth) || (z >= m_nLength))
		return(XMFLOAT3(0.0f, 1.0f, 0.0f));

	/*높이 맵에서 (x, z) 좌표의 픽셀 값과 인접한 두 개의 점 (x+1, z), (z, z+1)에 대한 픽셀 값을 사용하여 법선 벡터를
	계산한다.*/
	int nHeightMapIndex = x + (z * m_nWidth);
	int xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -m_nWidth;
	//(x, z), (x+1, z), (z, z+1)의 픽셀에서 지형의 높이를 구한다.
	float y1 = (float)m_pHeightMapPixels[nHeightMapIndex] * m_xmf3Scale.y;
	float y2 = (float)m_pHeightMapPixels[nHeightMapIndex + xHeightMapAdd] * m_xmf3Scale.y;
	float y3 = (float)m_pHeightMapPixels[nHeightMapIndex + zHeightMapAdd] * m_xmf3Scale.y;
	//xmf3Edge1은 (0, y3, m_xmf3Scale.z) - (0, y1, 0) 벡터이다.
	XMFLOAT3 xmf3Edge1 = XMFLOAT3(0.0f, y3 - y1, m_xmf3Scale.z);
	//xmf3Edge2는 (m_xmf3Scale.x, y2, 0) - (0, y1, 0) 벡터이다.
	XMFLOAT3 xmf3Edge2 = XMFLOAT3(m_xmf3Scale.x, y2 - y1, 0.0f);
	//법선 벡터는 xmf3Edge1과 xmf3Edge2의 외적을 정규화하면 된다.
	XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge1, xmf3Edge2, true);

	return(xmf3Normal);
}

#define _WITH_APPROXIMATE_OPPOSITE_CORNER

float CHeightMapImage::GetHeight(float fx, float fz)
{
	/*지형의 좌표 (fx, fz)는 이미지 좌표계이다. 높이 맵의 x-좌표와 z-좌표가 높이 맵의 범위를 벗어나면 지형의 높이는
	0이다.*/
	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= m_nWidth) || (fz >= m_nLength)) return(0.0f);
	//높이 맵의 좌표의 정수 부분과 소수 부분을 계산한다.
	int x = (int)fx;
	int z = (int)fz;
	float fxPercent = fx - x;
	float fzPercent = fz - z;

	float fBottomLeft = (float)m_pHeightMapPixels[x + (z*m_nWidth)];
	float fBottomRight = (float)m_pHeightMapPixels[(x + 1) + (z*m_nWidth)];
	float fTopLeft = (float)m_pHeightMapPixels[x + ((z + 1)*m_nWidth)];
	float fTopRight = (float)m_pHeightMapPixels[(x + 1) + ((z + 1)*m_nWidth)];
#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER
	//z-좌표가 1, 3, 5, ...인 경우 인덱스가 오른쪽에서 왼쪽으로 나열된다.
	bool bRightToLeft = ((z % 2) != 0);

	if (bRightToLeft)
	{
		if (fzPercent >= fxPercent)
			fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
		else
			fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}
	else
	{
		if (fzPercent < (1.0f - fxPercent))
			fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
		else
			fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}
#endif
	//사각형의 네 점을 보간하여 높이(픽셀 값)를 계산한다.
	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;
	return(fHeight);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHeightMapGridMesh
////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CHeightMapGridMesh::CHeightMapGridMesh(shared_ptr<CCreateMgr> pCreateMgr, int nWidth, int nLength) : CMesh(pCreateMgr)
{
	XMFLOAT3 xmf3Scale = TERRAIN_IMAGE_CELL_SCALE;

	//격자의 교점(정점)의 개수는 (nWidth * nLength)이다.
	m_nVertices = nWidth * nLength;
	m_nStride = sizeof(CTerrainVertex);

	//격자는 삼각형 스트립으로 구성한다.
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;

	m_nIndices = (nWidth * 2) * (nLength - 1) * 2 - 4;

	CTerrainVertex *pVertices = new CTerrainVertex[m_nVertices];

	for (int i = 0, z = 0; z < nLength; z++)
	{
		for (int x = 0; x < nWidth; x++, i++)
		{
			pVertices[i] = CTerrainVertex(XMFLOAT3((x * xmf3Scale.x), -REVISE_HEIGHT * xmf3Scale.y, (z * xmf3Scale.z)), XMFLOAT2((float)x / (nWidth - 1), 1 - (float)z / (nLength - 1)));
		}
	}

	m_pVertexBuffer = pCreateMgr->CreateBufferResource(
		pVertices,
		m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		m_pVertexUploadBuffer.GetAddressOf());

	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = m_nStride;
	m_vertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	delete[] pVertices;

	UINT *pnIndices = new UINT[m_nIndices];
	for (int j = 0, z = 0; z < nLength - 1; z++)
	{
		if ((z % 2) == 0)
		{
			//홀수 번째 줄이므로(z = 0, 2, 4, ...) 인덱스의 나열 순서는 왼쪽에서 오른쪽 방향이다.
			for (int x = 0; x < nWidth; x++)
			{
				//위(x, z+1), 아래(x, z)의 순서로 인덱스를 추가한다.
				if ((x != 0 || z != 0) && (x != nWidth - 1 || z != nLength - 2))
				{
					pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
					pnIndices[j++] = (UINT)(x + (z * nWidth));
				}
				pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
				pnIndices[j++] = (UINT)(x + (z * nWidth));
			}
		}
		else
		{
			//짝수 번째 줄이므로(z = 1, 3, 5, ...) 인덱스의 나열 순서는 오른쪽에서 왼쪽 방향이다.
			for (int x = nWidth - 1; x >= 0; x--)
			{
				if (x != 0 || z != nLength - 2)
				{
					pnIndices[j++] = (UINT)(x + (z * nWidth));
					pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
				}
				pnIndices[j++] = (UINT)(x + (z * nWidth));
				pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
	}

	m_pIndexBuffer = pCreateMgr->CreateBufferResource(
		pnIndices,
		sizeof(UINT) * m_nIndices,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_INDEX_BUFFER,
		m_pIndexUploadBuffer.GetAddressOf());

	m_indexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	delete[] pnIndices;
}

CHeightMapGridMesh::~CHeightMapGridMesh()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CTexturedRectMesh
////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CTexturedRectMesh::CTexturedRectMesh(shared_ptr<CCreateMgr> pCreateMgr, float fWidth, float fHeight, float fDepth, float fxPosition, float fyPosition, float fzPosition) : CMesh(pCreateMgr)
{
	m_nVertices = 6;
	m_nStride = sizeof(CTexturedVertex);
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CTexturedVertex pVertices[6];

	float fx = (fWidth * 0.5f) + fxPosition, fy = (fHeight * 0.5f) + fyPosition, fz = (fDepth * 0.5f) + fzPosition;

	if (fWidth == 0.0f)
	{
		if (fxPosition > 0.0f)
		{
			pVertices[0] = CTexturedVertex(XMFLOAT3(fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CTexturedVertex(XMFLOAT3(fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CTexturedVertex(XMFLOAT3(fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CTexturedVertex(XMFLOAT3(fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CTexturedVertex(XMFLOAT3(fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CTexturedVertex(XMFLOAT3(fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f));
		}
		else
		{
			pVertices[0] = CTexturedVertex(XMFLOAT3(fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CTexturedVertex(XMFLOAT3(fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CTexturedVertex(XMFLOAT3(fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CTexturedVertex(XMFLOAT3(fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CTexturedVertex(XMFLOAT3(fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CTexturedVertex(XMFLOAT3(fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f));
		}
	}
	else if (fHeight == 0.0f)
	{
		if (fyPosition > 0.0f)
		{
			pVertices[0] = CTexturedVertex(XMFLOAT3(+fx, fy, -fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CTexturedVertex(XMFLOAT3(+fx, fy, +fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CTexturedVertex(XMFLOAT3(-fx, fy, +fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CTexturedVertex(XMFLOAT3(-fx, fy, +fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CTexturedVertex(XMFLOAT3(-fx, fy, -fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CTexturedVertex(XMFLOAT3(+fx, fy, -fz), XMFLOAT2(1.0f, 0.0f));
		}
		else
		{
			pVertices[0] = CTexturedVertex(XMFLOAT3(+fx, fy, +fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CTexturedVertex(XMFLOAT3(+fx, fy, -fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CTexturedVertex(XMFLOAT3(-fx, fy, -fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CTexturedVertex(XMFLOAT3(-fx, fy, -fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CTexturedVertex(XMFLOAT3(-fx, fy, +fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CTexturedVertex(XMFLOAT3(+fx, fy, +fz), XMFLOAT2(1.0f, 0.0f));
		}
	}
	else if (fDepth == 0.0f)
	{
		if (fzPosition > 0.0f)
		{
			pVertices[0] = CTexturedVertex(XMFLOAT3(+fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CTexturedVertex(XMFLOAT3(+fx, -fy, fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CTexturedVertex(XMFLOAT3(-fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CTexturedVertex(XMFLOAT3(-fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CTexturedVertex(XMFLOAT3(-fx, +fy, fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CTexturedVertex(XMFLOAT3(+fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));
		}
		else
		{
			pVertices[0] = CTexturedVertex(XMFLOAT3(-fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));
			pVertices[1] = CTexturedVertex(XMFLOAT3(-fx, -fy, fz), XMFLOAT2(1.0f, 1.0f));
			pVertices[2] = CTexturedVertex(XMFLOAT3(+fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[3] = CTexturedVertex(XMFLOAT3(+fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
			pVertices[4] = CTexturedVertex(XMFLOAT3(+fx, +fy, fz), XMFLOAT2(0.0f, 0.0f));
			pVertices[5] = CTexturedVertex(XMFLOAT3(-fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));
		}
	}

	m_pVertexBuffer = pCreateMgr->CreateBufferResource(
		pVertices,
		m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		m_pVertexUploadBuffer.GetAddressOf());

	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = m_nStride;
	m_vertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CTexturedRectMesh::~CTexturedRectMesh()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CArrowMesh
////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CArrowMesh::CArrowMesh(shared_ptr<CCreateMgr> pCreateMgr, float length) : CMesh(pCreateMgr)
{
	m_nVertices = 24;
	m_nIndices = 108;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = 3, fy = 3, fz = 3;

	CDiffusedVertex pVertices[24];

	pVertices[0] = CDiffusedVertex(-fx, fy, -fz, XMFLOAT4(Colors::Red));
	pVertices[1] = CDiffusedVertex(length, fy, -fz, XMFLOAT4(Colors::Red));
	pVertices[2] = CDiffusedVertex(length, -fy, -fz, XMFLOAT4(Colors::Red));
	pVertices[3] = CDiffusedVertex(-fx, -fy, -fz, XMFLOAT4(Colors::Red));
	pVertices[4] = CDiffusedVertex(-fx, fy, fz, XMFLOAT4(Colors::Red));
	pVertices[5] = CDiffusedVertex(length, fy, fz, XMFLOAT4(Colors::Red));
	pVertices[6] = CDiffusedVertex(length, -fy, fz, XMFLOAT4(Colors::Red));
	pVertices[7] = CDiffusedVertex(-fx, -fy, fz, XMFLOAT4(Colors::Red));

	pVertices[8] = CDiffusedVertex(-fx, length, -fz, XMFLOAT4(Colors::Green));
	pVertices[9] = CDiffusedVertex(fx, length, -fz, XMFLOAT4(Colors::Green));
	pVertices[10] = CDiffusedVertex(fx, -fy, -fz, XMFLOAT4(Colors::Green));
	pVertices[11] = CDiffusedVertex(-fx, -fy, -fz, XMFLOAT4(Colors::Green));
	pVertices[12] = CDiffusedVertex(-fx, length, fz, XMFLOAT4(Colors::Green));
	pVertices[13] = CDiffusedVertex(fx, length, fz, XMFLOAT4(Colors::Green));
	pVertices[14] = CDiffusedVertex(fx, -fy, fz, XMFLOAT4(Colors::Green));
	pVertices[15] = CDiffusedVertex(-fx, -fy, fz, XMFLOAT4(Colors::Green));

	pVertices[16] = CDiffusedVertex(-fx, fy, -fz, XMFLOAT4(Colors::Blue));
	pVertices[17] = CDiffusedVertex(fx, fy, -fz, XMFLOAT4(Colors::Blue));
	pVertices[18] = CDiffusedVertex(fx, -fy, -fz, XMFLOAT4(Colors::Blue));
	pVertices[19] = CDiffusedVertex(-fx, -fy, -fz, XMFLOAT4(Colors::Blue));
	pVertices[20] = CDiffusedVertex(-fx, fy, length, XMFLOAT4(Colors::Blue));
	pVertices[21] = CDiffusedVertex(fx, fy, length, XMFLOAT4(Colors::Blue));
	pVertices[22] = CDiffusedVertex(fx, -fy, length, XMFLOAT4(Colors::Blue));
	pVertices[23] = CDiffusedVertex(-fx, -fy, length, XMFLOAT4(Colors::Blue));

	m_pVertexBuffer = pCreateMgr->CreateBufferResource(
		pVertices,
		m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		m_pVertexUploadBuffer.GetAddressOf());

	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = m_nStride;
	m_vertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	UINT pnIndices[108];

	pnIndices[0] = 0; pnIndices[1] = 1; pnIndices[2] = 3;
	pnIndices[3] = 1; pnIndices[4] = 2; pnIndices[5] = 3;
	pnIndices[6] = 4; pnIndices[7] = 5; pnIndices[8] = 0;
	pnIndices[9] = 5; pnIndices[10] = 1; pnIndices[11] = 0;
	pnIndices[12] = 5; pnIndices[13] = 6; pnIndices[14] = 1;
	pnIndices[15] = 6; pnIndices[16] = 2; pnIndices[17] = 1;
	pnIndices[18] = 6; pnIndices[19] = 7; pnIndices[20] = 2;
	pnIndices[21] = 7; pnIndices[22] = 3; pnIndices[23] = 2;
	pnIndices[24] = 7; pnIndices[25] = 4; pnIndices[26] = 0;
	pnIndices[27] = 7; pnIndices[28] = 0; pnIndices[29] = 3;
	pnIndices[30] = 5; pnIndices[31] = 4; pnIndices[32] = 7;
	pnIndices[33] = 5; pnIndices[34] = 7; pnIndices[35] = 6;

	for (int j = 1; j < 3; ++j)
	{
		for (int i = 0; i < 36; ++i)
		{
			pnIndices[36 * j + i] = pnIndices[i] + 8 * j;
		}
	}

	m_pIndexBuffer = pCreateMgr->CreateBufferResource(
		pnIndices,
		sizeof(UINT) * m_nIndices,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_INDEX_BUFFER,
		m_pIndexUploadBuffer.GetAddressOf());

	m_indexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}

CArrowMesh::~CArrowMesh()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CCubeMesh
////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CCubeMesh::CCubeMesh(shared_ptr<CCreateMgr> pCreateMgr, float fWidth, float fHeight, float fDepth, float xOffset, float yOffSet, float zOffSet) : CMesh(pCreateMgr)
{
	m_nVertices = 8;
	m_nIndices = 36;
	m_nStride = sizeof(CDiffusedVertex);
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 0.5f, fy = fHeight, fz = fDepth * 0.5f;

	CDiffusedVertex pVertices[8];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(-fx + xOffset, +fy + yOffSet, -fz + zOffSet), RANDOM_COLOR);
	pVertices[1] = CDiffusedVertex(XMFLOAT3(+fx + xOffset, +fy + yOffSet, -fz + zOffSet), RANDOM_COLOR);
	pVertices[2] = CDiffusedVertex(XMFLOAT3(+fx + xOffset, +fy + yOffSet, +fz + zOffSet), RANDOM_COLOR);
	pVertices[3] = CDiffusedVertex(XMFLOAT3(-fx + xOffset, +fy + yOffSet, +fz + zOffSet), RANDOM_COLOR);
	pVertices[4] = CDiffusedVertex(XMFLOAT3(-fx + xOffset, -fy + yOffSet, -fz + zOffSet), RANDOM_COLOR);
	pVertices[5] = CDiffusedVertex(XMFLOAT3(+fx + xOffset, -fy + yOffSet, -fz + zOffSet), RANDOM_COLOR);
	pVertices[6] = CDiffusedVertex(XMFLOAT3(+fx + xOffset, -fy + yOffSet, +fz + zOffSet), RANDOM_COLOR);
	pVertices[7] = CDiffusedVertex(XMFLOAT3(-fx + xOffset, -fy + yOffSet, +fz + zOffSet), RANDOM_COLOR);

	m_pVertexBuffer = pCreateMgr->CreateBufferResource(
		pVertices,
		m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		m_pVertexUploadBuffer.GetAddressOf());

	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = m_nStride;
	m_vertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	UINT pIndices[36];
	pIndices[0] = 3; pIndices[1] = 1; pIndices[2] = 0;
	pIndices[3] = 2; pIndices[4] = 1; pIndices[5] = 3;
	pIndices[6] = 0; pIndices[7] = 5; pIndices[8] = 4;
	pIndices[9] = 1; pIndices[10] = 5; pIndices[11] = 0;
	pIndices[12] = 3; pIndices[13] = 4; pIndices[14] = 7;
	pIndices[15] = 0; pIndices[16] = 4; pIndices[17] = 3;
	pIndices[18] = 1; pIndices[19] = 6; pIndices[20] = 5;
	pIndices[21] = 2; pIndices[22] = 6; pIndices[23] = 1;
	pIndices[24] = 2; pIndices[25] = 7; pIndices[26] = 6;
	pIndices[27] = 3; pIndices[28] = 7; pIndices[29] = 2;
	pIndices[30] = 6; pIndices[31] = 4; pIndices[32] = 5;
	pIndices[33] = 7; pIndices[34] = 4; pIndices[35] = 6;

	m_pIndexBuffer = pCreateMgr->CreateBufferResource(
		pIndices,
		sizeof(UINT) * m_nIndices,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_INDEX_BUFFER,
		m_pVertexUploadBuffer.GetAddressOf());

	m_indexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}

CCubeMesh::~CCubeMesh()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CCollisionMapImage
////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CCollisionMapImage::CCollisionMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale)
{
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;
	BYTE *pCollisionMapPixels = new BYTE[m_nWidth * m_nLength];

	HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead;
	::ReadFile(hFile, pCollisionMapPixels, (m_nWidth * m_nLength), &dwBytesRead, NULL);
	::CloseHandle(hFile);

	m_pCollisionMapPixels = new BYTE[m_nWidth * m_nLength];
	for (int y = 0; y < m_nLength; y++)
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			m_pCollisionMapPixels[x + ((m_nLength - 1 - y)*m_nWidth)] = pCollisionMapPixels[x + (y*m_nWidth)];
		}
	}

	if (pCollisionMapPixels) Safe_Delete_Array(pCollisionMapPixels);
}

CCollisionMapImage::~CCollisionMapImage(void)
{
	if (m_pCollisionMapPixels) Safe_Delete_Array(m_pCollisionMapPixels);
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
bool CCollisionMapImage::GetCollision(float fx, float fz)
{
	fx = fx / m_xmf3Scale.x;
	fz = fz / m_xmf3Scale.z;

	// 좌표를 벗어나면 항상 콜리전이 일어나도록 한다.
	if ((fx <= 1.0f) || (fz <= 1.0f) || (fx >= m_nWidth - 1) || (fz >= m_nLength - 1)) return(true);

	//높이 맵의 좌표의 정수 부분과 소수 부분을 계산한다.
	int x = (int)fx;
	int z = (int)fz;
	float fxPercent = fx - x;
	float fzPercent = fz - z;

	// 정수부 기준으로 사각형을 짠다.
	float fBottomLeft{ (float)m_pCollisionMapPixels[x + (z * m_nWidth)] };
	float fBottomRight{ (float)m_pCollisionMapPixels[(x + 1) + (z * m_nWidth)] };
	float fTopLeft{ (float)m_pCollisionMapPixels[x + ((z + 1) * m_nWidth)] };
	float fTopRight{ (float)m_pCollisionMapPixels[(x + 1) + ((z + 1) * m_nWidth)] };

	// 소수부 위치를 처리하기 위해 보간 처리 한다.
	float xUpCenter{ fTopLeft * fxPercent + fTopRight * (1 - fxPercent) };
	float xDownCenter{ fBottomLeft * fxPercent + fBottomRight * (1 - fxPercent) };

	return xUpCenter * (1 - fzPercent) + xDownCenter * fzPercent > 250.f;
}
