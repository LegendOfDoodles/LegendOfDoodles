#include "stdafx.h"
#include "Skeleton.h"



CSkeleton::CSkeleton(char * in)
{
	std::ifstream ifs(in);
	std::string line;

	while (getline(ifs, line) && line.find("<animations>", 0) == -1) {}
	getline(ifs, line);

	m_iAnimationLegth = strtof(line.c_str(), NULL);

	ifs.close();
}

CSkeleton::~CSkeleton()
{
}
