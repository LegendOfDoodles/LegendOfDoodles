#pragma once
class CFlyingShader;

class CThrowingMgr
{
public: // 생성자, 소멸자
	CThrowingMgr();
	virtual ~CThrowingMgr();

public: // 공개 함수
	void RequestSpawn(const XMFLOAT3& position, const XMFLOAT3& direction, TeamType teamType, FlyingObjectType objectType);

	void RequestPause();
	void RequestResume();

	void SetFlyingShader(CFlyingShader* pFlyingShader) { m_pFlyingShader = pFlyingShader; }

protected: // 내부 함수

protected: // 변수
	CFlyingShader * m_pFlyingShader{ NULL };
};

