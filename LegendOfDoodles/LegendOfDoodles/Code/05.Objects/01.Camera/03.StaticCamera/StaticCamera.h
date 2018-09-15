#pragma once
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"

class CStaticCamera : public CCamera
{
public:	// ������, �Ҹ���
	CStaticCamera();
	virtual ~CStaticCamera();

public:	// ���� �Լ�
	virtual void Initialize(shared_ptr<CCreateMgr> pCreateMgr);

	virtual void Update(float) { }

	virtual bool OnProcessMouseWheel(WPARAM, LPARAM) { return true; }

	virtual bool OnProcessMouseInput(UCHAR*) { return true; }
	virtual bool OnProcessKeyInput(UCHAR*) { return true; }

	virtual bool IsInFrustum(BoundingOrientedBox&) { return true; }

protected: // ���� �Լ�

protected: // ����
};

