#pragma once
#include "05.Objects/96.Billboard/99.BillboardObject/Billboard.h"

class CNumberOjbect : public CBillboardObject
{
public:
	CNumberOjbect(shared_ptr<CCreateMgr> pCreateMgr);
	CNumberOjbect(shared_ptr<CCreateMgr> pCreateMgr, NumberType type);
	virtual ~CNumberOjbect();

public: // 공개함수
	virtual void Animate(float fTimeElapsed);
	virtual void Render(CCamera *pCamera, UINT istanceCnt = 1);


	virtual void SetDistance(float x) {
		m_xmf4x4World = Matrix4x4::Identity();

		m_fDistance = x;
	}

	void SetTexCoord(int cnt) { InputNum = cnt; }
	void SetOffset(int cnt);
	
	int GetNum() { return InputNum; }
	virtual NumberType GetType() { return m_type; }

protected: // 변수
	float		m_fDistance = 0.0f;
	NumberType  m_type;

	int InputNum = 0;
	float m_fOffset = 0;
};