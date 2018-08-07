#pragma once
#include "03.Scenes/00.BaseScene/Scene.h"

class CFramework
{
public:	// ������, �Ҹ���
	CFramework();
	~CFramework();

public: // ���� �Լ�
		// Initialize and Release
	bool Initialize();
	void Finalize();
	void FrameAdvance(float timeElapsed);

	bool IsRunning() { return m_running; }

private: // ���� �Լ�
	void BuildObjects();
	void ReleaseObjects();

private: // ����
	shared_ptr<CScene> m_pScene;

	bool m_running{ true };
};

