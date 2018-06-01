#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"

struct CB_TERRAIN_INFO
{
	XMFLOAT4X4					m_xmf4x4World;
};

class CHeightMapTerrain : public CBaseObject
{
public: // ������, �Ҹ���
	CHeightMapTerrain(CCreateMgr *pCreateMgr, LPCTSTR pFileName, XMFLOAT3 xmf3Scale);
	virtual ~CHeightMapTerrain();

public: // ���� �Լ�
	//������ ���̸� ����ϴ� �Լ��̴�(���� ��ǥ��). ���� ���� ���̿� �������� y�� ���� ���̴�.
	float GetHeight(float x, float z) {
		return(m_pHeightMapImage->GetHeight(x / m_xmf3Scale.x, z / m_xmf3Scale.z) * m_xmf3Scale.y - REVISE_HEIGHT_FOR_OBJECT);
	}

	//������ ���� ���͸� ����ϴ� �Լ��̴�(���� ��ǥ��). ���� ���� ���� ���͸� ����Ѵ�.
	XMFLOAT3 GetNormal(float x, float z) {
		return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z /
			m_xmf3Scale.z)));
	}

protected: // ���� �Լ�

private:	 // ����
	 //������ ���� ������ ����� �̹����̴�.
	CHeightMapImage *m_pHeightMapImage;
	//���� ���� ���ο� ���� ũ���̴�.
	int m_nWidth;
	int m_nLength;
	//������ ������ �� �� Ȯ���� ���ΰ��� ��Ÿ���� ������ �����̴�.
	XMFLOAT3 m_xmf3Scale;
};