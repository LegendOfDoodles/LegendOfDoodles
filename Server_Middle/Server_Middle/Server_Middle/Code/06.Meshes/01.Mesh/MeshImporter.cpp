#include "stdafx.h"
#include "MeshImporter.h"

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
	//ÀÎµ¦½º ÃßÃâ
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
	ifs.close();
}

CTransformImporter::~CTransformImporter()
{
}
