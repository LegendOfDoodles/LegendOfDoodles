#pragma once
#include "03.Scenes/00.BaseScene/Scene.h"

class CScene;

class CFramework
{
public:	// 생성자, 소멸자
	CFramework();
	~CFramework();

public: // 공개 함수
		// Initialize and Release
	bool Initialize();
	void Finalize();
	void FrameAdvance(float timeElapsed);
	shared_ptr<CScene> GetScene() { return m_pScene; }

	bool IsRunning() { return m_running; }

private: // 내부 함수
	void BuildObjects();
	void ReleaseObjects();

private: // 변수
	shared_ptr<CScene> m_pScene;

	bool m_running{ true };
};

