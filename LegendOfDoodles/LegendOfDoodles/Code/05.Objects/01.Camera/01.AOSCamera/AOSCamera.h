#pragma once
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"

#define Window_Edge_Size 60
#define Window_Edge_Size_Half Window_Edge_Size * 0.5
#define Window_Edge_Power Window_Edge_Size * 3

#define Zoom_In_Max  30.0f
#define Zoom_Out_Max  90.0f
#define Zoom_Change_Value 5.0f

class CAOSCamera : public CCamera
{
public:	// 생성자, 소멸자
	CAOSCamera();
	virtual ~CAOSCamera();

public:	// 공개 함수
	virtual void Initialize(CCreateMgr *pCreateMgr) override;

	virtual void Move(float fTimeElapsed, bool bVelocity) override;

	virtual bool OnProcessMouseWheel(WPARAM wParam, LPARAM lParam);

	virtual bool OnProcessMouseInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);

protected: // 내부 함수

protected: // 변수
	RECT m_edgeSize{ 0, 0, 0, 0 };

	float m_angleDegree{ Zoom_Out_Max };

	XMFLOAT2 m_speedVector{ 0, 0 };
};

