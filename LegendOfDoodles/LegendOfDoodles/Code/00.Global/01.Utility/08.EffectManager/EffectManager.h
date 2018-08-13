#pragma once
class CEffectShader;

class CEffectMgr
{
public: // ������, �Ҹ���
	CEffectMgr();
	virtual ~CEffectMgr();

public: // ���� �Լ�
	void RequestSpawn(const XMFLOAT3& position, const XMFLOAT3& direction, int aniLength, EffectObjectType objectType);

	void RequestPause();
	void RequestResume();

	void SetEffectShader(CEffectShader* pFlyingShader) { m_pEffectShader = pFlyingShader; }

protected: // ���� �Լ�

protected: // ����
	CEffectShader * m_pEffectShader{ NULL };
};

