#include "stdafx.h"
#include "Skeleton.h"




CSkeleton::CSkeleton(char * in)
{
	std::ifstream ifs(in);
	std::string line;

	while (getline(ifs, line) && line.find("<skeleton count>", 0) == -1)
	{
	}
	getline(ifs, line);
	int skcount;
	char *end = NULL;
	skcount = strtof(line.c_str(), &end);
	m_vBone.reserve(skcount);
	for (int i = 0; i < skcount; ++i) {
		Bone skeleton;
		getline(ifs, line);
		float IDindex[2];
		char *end = NULL;
		skeleton.m_iID = strtof(line.c_str(), &end);
		skeleton.m_iParentID = strtof(end, NULL);

		char *Matend = NULL;
		getline(ifs, line);
		skeleton.m_m4x4Matrix._11 = strtof(line.c_str(), &Matend);
		skeleton.m_m4x4Matrix._12 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._13 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._14 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._21 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._22 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._23 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._24 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._31 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._32 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._33 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._34 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._41 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._42 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._43 = strtof(Matend, &Matend);
		skeleton.m_m4x4Matrix._44 = strtof(Matend, NULL);

		m_vBone.push_back(skeleton);

	}


	while (getline(ifs, line) && line.find("<animations>", 0) == -1)
	{
	}
	getline(ifs, line);

	end = NULL;
	m_iAnimationLegth = strtof(line.c_str(), &end);

	auto it = m_vBone.begin();
	for (int i = 0; i < skcount; ++i) {
		getline(ifs, line);
		end = NULL;
		KeyFrame aniMatrix;
		aniMatrix.m_iframe = strtof(line.c_str(), &end);

		while (getline(ifs, line) && line.find("</Joint>", 0) == -1)
		{
			end = NULL;
			aniMatrix.m_iframe = strtof(line.c_str(), &end);
			getline(ifs, line);
			
			aniMatrix.m_m4x4Matrix._11 = strtof(line.c_str(), &end);
			aniMatrix.m_m4x4Matrix._12 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._13 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._14 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._21 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._22 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._23 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._24 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._31 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._32 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._33 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._34 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._41 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._42 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._43 = strtof(end, &end);
			aniMatrix.m_m4x4Matrix._44 = strtof(end, NULL);

			aniMatrix.m_m4x4FinalMatrix = ( Matrix4x4::Multiply(aniMatrix.m_m4x4Matrix, it->m_m4x4Matrix));
			it->m_Keyframe.push_back(aniMatrix);

		}

		++it;
	}



}

CSkeleton::~CSkeleton()
{
}
