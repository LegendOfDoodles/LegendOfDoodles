#pragma once
class CSkeleton
{
public: // 생성자, 소멸자
	CSkeleton() {}
	CSkeleton(char* in);
	virtual ~CSkeleton();

public: // 공개 함수
	int GetAnimationLength() { return m_iAnimationLegth; }

private:	 // 변수
	int m_iAnimationLegth;
};

