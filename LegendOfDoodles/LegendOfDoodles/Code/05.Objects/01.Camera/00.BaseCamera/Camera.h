#pragma once

//카메라의 종류(모드: Mode)를 나타내는 상수를 다음과 같이 선언한다.
#define	SPACESHIP_CAMERA					0x02

#define	ASPECT_RATIO							(float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

#define DIR_FORWARD 0x01
#define DIR_BACKWARD 0x02
#define DIR_LEFT 0x04
#define DIR_RIGHT 0x08
#define DIR_UP 0x10
#define DIR_DOWN 0x20

#define DIR_LEFT_FAST 0x05
#define DIR_RIGHT_FAST 0x09
#define DIR_UP_FAST 0x11
#define DIR_DOWN_FAST 0x21

class CCreateMgr;
class CCollisionObject;

struct VS_CB_CAMERA_INFO
{
	XMFLOAT4X4 m_xmf4x4View;
	XMFLOAT4X4 m_xmf4x4Projection;
	XMFLOAT3	   m_xmf3Position;
};

class CCamera
{
public:	// 생성자, 소멸자
	CCamera();
	virtual ~CCamera();

public:	// 공개 함수
	virtual void Initialize(shared_ptr<CCreateMgr> pCreateMgr);
	void Finalize();

	virtual void UpdateShaderVariables(int rootSignatureIndex = 0);

	virtual void SetViewportsAndScissorRects();

	virtual void Move(float fTimeElapsed, bool bVelocity);
	void Move(XMFLOAT3& xmf3Shift);
	void Rotate(float x = 0.0f, float y = 0.0f, float z = 0.0f);
	void Update(float fTimeElapsed);
	virtual void SetLookAt(XMFLOAT3& xmf3LookAt);

	void SetOffset(XMFLOAT3 xmf3Offset);

	void RegenerateViewMatrix();

	void GeneratePerspectiveProjectionMatrix(
		float fNearPlaneDistance, float fFarPlaneDistance,
		float fAspectRatio, float fFOVAngle);

	void GenerateOrthographicProjectionMatrix(
		float fWidth, float fHeight,
		float fNearPlaneDistance, float fFarPlaneDistance);

	void GenerateOrthographicOffCenterProjectionMatrix(
		float left, float right,
		float top, float bottom,
		float fNearPlaneDistance, float fFarPlaneDistance);

	void SavePickedPos();

	void SetMaster(CCollisionObject* master);

	virtual bool OnProcessMouseWheel(WPARAM wParam, LPARAM lParam);

	virtual bool OnProcessMouseInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);

	void GenerateFrustum();
	virtual bool IsInFrustum(BoundingOrientedBox& xmBoundingBox);

	void GoToMaster();

	void SetPosition(float x, float y, float z);
	void SetPosition(float x, float z);

	void SetMode(DWORD nMode) { m_nMode = nMode; }
	DWORD GetMode() { return(m_nMode); }

	void SetPosition(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	XMFLOAT3& GetPosition() { return(m_xmf3Position); }

	void SetLookAtPosition(XMFLOAT3 xmf3LookAtWorld) { m_xmf3LookAtWorld = xmf3LookAtWorld; }

	XMFLOAT3& GetLookAtPosition() { return(m_xmf3LookAtWorld); }
	XMFLOAT3& GetRightVector() { return(m_xmf3Right); }
	XMFLOAT3& GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3& GetLookVector() { return(m_xmf3Look); }

	float& GetPitch() { return(m_fPitch); }
	float& GetRoll() { return(m_fRoll); }
	float& GetYaw() { return(m_fYaw); }

	XMFLOAT3& GetOffset() { return(m_xmf3Offset); }

	XMFLOAT4X4 GetViewMatrix() { return(m_xmf4x4View); }
	XMFLOAT4X4 GetProjectionMatrix() { return(m_xmf4x4Projection); }
	D3D12_VIEWPORT GetViewport() { return(m_viewport); }
	D3D12_RECT GetScissorRect() { return(m_scissorRect); }

	HWND GetHwnd() { return(m_hWnd); }

	void SetRotation(float x, float y, float z) { m_rotation.x = x; m_rotation.y = y, m_rotation.z = z; }

protected: // 내부 함수
	void GenerateViewMatrix();
	void GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up = XMFLOAT3(0, 0, 0));

	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);

	virtual void CreateShaderVariables(shared_ptr<CCreateMgr> pCreateMgr, UINT stride);
	virtual void ReleaseShaderVariables();

protected: // 변수
	XMFLOAT3 m_xmf3Right{ 1.0f, 0.0f, 0.0f };
	XMFLOAT3 m_xmf3Up{ 0.0f,1.0f,0.0f };
	XMFLOAT3 m_xmf3Look{ 0.0f,0.0f,1.0f };
	XMFLOAT3 m_xmf3Position{ 0.0f,0.0f,0.0f };

	float m_fPitch{ 0.0f };
	float m_fRoll{ 0.0f };
	float m_fYaw{ 0.0f };

	DWORD m_nMode{ 0x00 };

	XMFLOAT3 m_xmf3LookAtWorld{ 0.0f,0.0f,0.0f };

	XMFLOAT3 m_xmf3Offset{ 0.0f,0.0f,0.0f };

	XMFLOAT4X4 m_xmf4x4View;
	XMFLOAT4X4 m_xmf4x4Projection;
	XMFLOAT4X4 m_xmf4x4ShadowViewProjTex;

	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;

	float m_speed{ 1000.0f };

	DWORD m_direction{ NULL };
	XMFLOAT3 m_rotation{ 0.0f, 0.0f, 0.0f };

	POINT m_pickCursorPos{ 0, 0 };

	ComPtr<ID3D12Resource> m_pConstBuffer;
	UINT8 *m_pMappedCamera{ NULL };

	HWND m_hWnd{ NULL };
	ComPtr<ID3D12GraphicsCommandList> m_pCommandList;

	BoundingFrustum m_xmFrustum;

	CCollisionObject* m_pMaster{ NULL };
};

