#pragma once


struct KeyFrame {
	int m_iframe;
	XMFLOAT4X4  m_m4x4Matrix;
	XMFLOAT4X4  m_m4x4FinalMatrix;
};

struct Bone {
	int m_iID;
	int m_iParentID;
	XMFLOAT4X4  m_m4x4Matrix;
	std::vector<KeyFrame> m_Keyframe;

	XMFLOAT4X4 GetFrame(int frame) {
		if (m_Keyframe.size() == 0) {
			return Matrix4x4::Identity();
		}
		return m_Keyframe.at((int)(min(frame, m_Keyframe.size()))).m_m4x4FinalMatrix;
	}
};

class CSkeleton
{
public: // 생성자, 소멸자
	CSkeleton() {}
	CSkeleton(char* in);
	virtual ~CSkeleton();

public: // 공개 함수
	int GetAnimationLength() { return m_iAnimationLegth; }
	Bone GetBone(float id) { return m_vBone.at(static_cast<size_t>(id)); }
	int GetBoneCount() { return static_cast<int>(m_vBone.size()); }

private:	 // 변수
	int m_iAnimationLegth;
	std::vector<Bone> m_vBone;
};

