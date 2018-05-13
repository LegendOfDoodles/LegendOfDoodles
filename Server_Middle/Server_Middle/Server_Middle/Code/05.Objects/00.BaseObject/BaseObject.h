#pragma once
#include "06.Meshes/01.Mesh/Mesh.h"
#include "00.Global/01.Utility/Enumerations.h"

class CBaseObject
{
public:	// 생성자, 소멸자
	CBaseObject();
	virtual ~CBaseObject();

public: // 공개 함수
	virtual void Initialize();
	virtual void Finalize();

	virtual void Animate(float timeElapsed);

	void MoveStrafe(float fDistance = 1.0f);
	virtual void MoveUp(float fDistance = 1.0f);
	virtual void MoveForward(float fDistance = 1.0f);

	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

	void Translate(XMFLOAT3 *pxmf3Axis);
	void Translate(float x,float y, float z);

	void Scale(float x = 1.0f, float y = 1.0f, float z = 1.0f);

	XMFLOAT3 GetPosition();
	virtual XMFLOAT3 GetLook();
	virtual XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	virtual void SetPosition(float x, float z);
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	XMFLOAT4X4* GetWorldMatrix() { return &m_xmf4x4World; }

	void SaveIndex(int idx) { m_index = idx; }
	int GetIndex() { return m_index; }

	int GetType() {	return m_ObjectType; };
	void SetType(ObjectType newObjectType) {m_ObjectType = newObjectType;};

protected: // 내부 함수

protected: // 변수
	int m_nReferences{ 0 };

	int m_index{ 0 };

	XMFLOAT4X4 m_xmf4x4World;

	ObjectType m_ObjectType{ ObjectType::StickPlayer };
};
