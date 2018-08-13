#pragma once
class CEffectShader;

class CEffectMgr
{
public: // 생성자, 소멸자
	CEffectMgr();
	virtual ~CEffectMgr();

public: // 공개 함수
	void RequestSpawn(const XMFLOAT3& position, const XMFLOAT3& direction, int aniLength, EffectObjectType objectType);

	void RequestPause();
	void RequestResume();

	void SetEffectShader(CEffectShader* pFlyingShader) { m_pEffectShader = pFlyingShader; }

protected: // 내부 함수

protected: // 변수
	CEffectShader * m_pEffectShader{ NULL };
};

