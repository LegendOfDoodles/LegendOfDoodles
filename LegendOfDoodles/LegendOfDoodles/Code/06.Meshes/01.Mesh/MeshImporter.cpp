#include "stdafx.h"
#include "MeshImporter.h"

CMeshImporter::CMeshImporter()
{

}


void CMeshImporter::LoadMeshData(char * in)
{
	std::ifstream ifs(in);
	std::string line;
	//삼각형 개수 확인
	while (std::getline(ifs, line))
	{
		if (line.find("triangles count", 0) != -1) {
			std::regex number("[0-9]+");
			std::sregex_iterator it(line.begin(), line.end(), number);
			std::sregex_iterator end;

			std::smatch m = *it;
			m_iTriCnt = atoi(m.str(0).c_str());
			m_xmTriIndex.reserve(m_iTriCnt);
			break;
		}
	}
	//인덱스 추출
	while (std::getline(ifs, line) && line.find("</triangles>", 0) == -1)
	{
		float index[3];
		int n = 0;
		char *end = NULL;
		index[0] = strtof(line.c_str(), &end);
		index[1] = strtof(end, &end);
		index[2] = strtof(end, NULL);
		m_xmTriIndex.push_back(XMFLOAT3(index[0], index[1], index[2]));

	}
	while (std::getline(ifs, line))
	{
		if (line.find("vertices count", 0) != -1) {
			std::regex number("[0-9]+");
			std::sregex_iterator it(line.begin(), line.end(), number);
			std::sregex_iterator end;

			std::smatch m = *it;
			m_iVerticesCnt = atoi(m.str(0).c_str());
			m_xmVertex.reserve(m_iVerticesCnt);
			break;
		}
	}

	while (std::getline(ifs, line) && line.find("</vertices>", 0) == -1)
	{
		MyVertex vertex;
		int LineNum = 0;
		float index[3];

		while (std::getline(ifs, line) && line.find("</vtx>", 0) == -1)
		{
			char *end = NULL;
			switch (LineNum)
			{
			case 0:

				index[0] = strtof(line.c_str(), &end);
				index[1] = strtof(end, &end);
				index[2] = strtof(end, NULL);
				vertex.pos = XMFLOAT3(index[0], index[1], index[2]);
				break;

			case 1:

				index[0] = strtof(line.c_str(), &end);
				index[1] = strtof(end, &end);
				index[2] = strtof(end, NULL);
				vertex.normal = XMFLOAT3(index[0], index[1], index[2]);
				break;;
			case 2:
				vertex.skinweight.x = strtof(line.c_str(), &end);
				vertex.skinweight.y = strtof(end, &end);
				vertex.skinweight.z = strtof(end, &end);
				vertex.skinweight.w = strtof(end, NULL);
				break;

			case 3:
				vertex.skinindex.x = strtof(line.c_str(), &end);
				vertex.skinindex.y = strtof(end, &end);
				vertex.skinindex.z = strtof(end, &end);
				vertex.skinindex.w = strtof(end, NULL);
				break;
			case 4:
				vertex.uv.x = strtof(line.c_str(), &end);
				vertex.uv.y = strtof(end, NULL);
				break;
			}
			LineNum++;
		}
		m_xmVertex.push_back(vertex);
	}
}

void CMeshImporter::LoadStaticMeshData(char * in)
{

	std::ifstream ifs(in);
	std::string line;
	//삼각형 개수 확인
	while (std::getline(ifs, line))
	{
		if (line.find("triangles count", 0) != -1) {
			std::regex number("[0-9]+");
			std::sregex_iterator it(line.begin(), line.end(), number);
			std::sregex_iterator end;

			std::smatch m = *it;
			m_iTriCnt = atoi(m.str(0).c_str());
			m_xmTriIndex.reserve(m_iTriCnt);
			break;
		}
	}
	//인덱스 추출
	while (std::getline(ifs, line) && line.find("</triangles>", 0) == -1)
	{
		float index[3];
		int n = 0;
		char *end = NULL;
		index[0] = strtof(line.c_str(), &end);
		index[1] = strtof(end, &end);
		index[2] = strtof(end, NULL);
		m_xmTriIndex.push_back(XMFLOAT3(index[0], index[1], index[2]));

	}
	while (std::getline(ifs, line))
	{
		if (line.find("vertices count", 0) != -1) {
			std::regex number("[0-9]+");
			std::sregex_iterator it(line.begin(), line.end(), number);
			std::sregex_iterator end;

			std::smatch m = *it;
			m_iVerticesCnt = atoi(m.str(0).c_str());
			m_xmVertex.reserve(m_iVerticesCnt);
			break;
		}
	}

	while (std::getline(ifs, line) && line.find("</vertices>", 0) == -1)
	{
		MyVertex vertex;
		int LineNum = 0;
		float index[3];

		while (std::getline(ifs, line) && line.find("</vtx>", 0) == -1)
		{
			char *end = NULL;
			switch (LineNum)
			{
			case 0:

				index[0] = strtof(line.c_str(), &end);
				index[1] = strtof(end, &end);
				index[2] = strtof(end, NULL);
				vertex.pos = XMFLOAT3(index[0], index[1], index[2]);
				break;

			case 1:

				index[0] = strtof(line.c_str(), &end);
				index[1] = strtof(end, &end);
				index[2] = strtof(end, NULL);
				vertex.normal = XMFLOAT3(index[0], index[1], index[2]);
				break;
			case 2:
				vertex.uv.x = strtof(line.c_str(), &end);
				vertex.uv.y = strtof(end, NULL);
				break;
			}
			LineNum++;
		}
		m_xmVertex.push_back(vertex);
	}
}

CMeshImporter::~CMeshImporter()
{
}

CTransformImporter::CTransformImporter()
{
}

void CTransformImporter::LoadMeshData(char * in)
{

	std::ifstream ifs(in);
	std::string line;
	m_iTotalCnt = 0;
	int i = 0;
	for (int k = 0; k < 4; ++k) {
		BookScale[k] = XMFLOAT3(0, 0, 0);
	}
	//인덱스 추출
	while (std::getline(ifs, line)) {
		std::getline(ifs, line);
		int cnt = strtof(line.c_str(), NULL);
		m_iTotalCnt += cnt;
		m_iKindMeshCnt[i] = cnt;
		
		while (std::getline(ifs, line) && line.find("<end>", 0) == -1)
		{
			Transform vertex;
			int LineNum = 0;
			float index[3];
			for(int k=0 ; k<3 ; ++k)
			{
				char *end = NULL;
				switch (LineNum)
				{
				case 0:

					index[0] = strtof(line.c_str(), &end);
					index[1] = strtof(end, &end);
					index[2] = strtof(end, NULL);
					vertex.pos = XMFLOAT3(index[0], index[1], index[2]);
					std::getline(ifs, line);
					break;

				case 1:

					index[0] = strtof(line.c_str(), &end);
					index[1] = strtof(end, &end);
					index[2] = strtof(end, NULL);
					vertex.rotation = XMFLOAT3(index[0], index[1], index[2]);
					std::getline(ifs, line);
					break;
				case 2:

					index[0] = strtof(line.c_str(), &end);
					index[1] = strtof(end, &end);
					index[2] = strtof(end, NULL);
					vertex.scale = XMFLOAT3(index[0], index[1], index[2]);
					if (i >= 13 && BookScale[i - 13].x == 0)
						BookScale[i - 13] = vertex.scale;
				}

				LineNum++;
			}
			m_Transform.push_back(vertex);
		}
		++i;
	}

}

CTransformImporter::~CTransformImporter()
{
}
