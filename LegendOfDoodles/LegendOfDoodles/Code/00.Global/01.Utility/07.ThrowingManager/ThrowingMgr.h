#pragma once
class CFlyingShader;

class CThrowingMgr
{
public: // ������, �Ҹ���
	CThrowingMgr();
	virtual ~CThrowingMgr();

public: // ���� �Լ�
	void RequestSpawn(const XMFLOAT3& position, const XMFLOAT3& direction, TeamType teamType, FlyingObjectType objectType);

	void RequestPause();
	void RequestResume();

	void SetFlyingShader(CFlyingShader* pFlyingShader) { m_pFlyingShader = pFlyingShader; }

protected: // ���� �Լ�

protected: // ����
	CFlyingShader * m_pFlyingShader{ NULL };
};

