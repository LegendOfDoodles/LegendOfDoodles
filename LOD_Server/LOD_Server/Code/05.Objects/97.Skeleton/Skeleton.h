#pragma once
class CSkeleton
{
public: // ������, �Ҹ���
	CSkeleton() {}
	CSkeleton(char* in);
	virtual ~CSkeleton();

public: // ���� �Լ�
	int GetAnimationLength() { return m_iAnimationLegth; }

private:	 // ����
	int m_iAnimationLegth;
};

