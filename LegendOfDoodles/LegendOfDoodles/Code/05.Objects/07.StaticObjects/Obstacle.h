#pragma once

class CCreateMgr;

class CObstacle : public CBaseObject
{
public: // 생성자, 소멸자
	CObstacle(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CObstacle();

public:	// 외부 함수

protected:	// 내부 함수

protected:	// 변수
};

