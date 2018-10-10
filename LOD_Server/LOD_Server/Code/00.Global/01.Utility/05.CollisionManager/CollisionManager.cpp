#include "stdafx.h"
#include "CollisionManager.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"
#include "05.Objects/08.Player/Player.h"

CCollisionManager::CCollisionManager()
{

}

void CCollisionManager::GameOver(TeamType type)
{
	g_GameFinished = true;

	if (type == TeamType::Blue) {
		m_Winner = TeamType::Red;
	}
	if (type == TeamType::Red) {
		m_Winner = TeamType::Blue;
	}
	for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
	{
		if ((*i)->GetTeam() == m_Winner) {
			(*i)->SetState(StatesType::Win);
		}
		else
			(*i)->SetState(StatesType::Defeat);
	}
}

void CCollisionManager::SetNodeMap(std::vector<NodeMap> map, float size, XMFLOAT2 wh)
{
	int width{ (int)wh.x };
	int height{ (int)wh.y };

	m_BlueSight = new NodeMap*[width];
	m_RedSight = new NodeMap*[width];
	for (int i = 0; i < width; ++i) {
		m_BlueSight[i] = new NodeMap[height];
		m_RedSight[i] = new NodeMap[height];
	}
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			m_BlueSight[j][i] = map[(i * width) + j];
			m_RedSight[j][i] = map[(i * width) + j];
		}
	}
	nodeSize = size;
	nodeWH = wh;
}

void CCollisionManager::AddCollider(CCollisionObject* pcol)
{
	if (pcol) {
		if (pcol->GetTeam() == m_User)
			pcol->SetDetected(true);
		else
			pcol->SetDetected(false);

		m_lstColliders.push_back(pcol);
	}
}


void CCollisionManager::Update(shared_ptr<CWayFinder> pWayFinder)
{
	if (!g_GameFinished)
	{
		int cnt = 0;
		m_lstColliders.remove_if([](CCollisionObject* obj) { return obj->GetState() == States::Die; });
		for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
		{
			(*i)->SetDetected(false);
		}
		for (int i = 0; i < nodeWH.x; ++i) {
			for (int j = 0; j < nodeWH.y; ++j) {
				m_BlueSight[i][j].Detected = false;
				m_RedSight[i][j].Detected = false;
			}
		}

		m_lstRedSight.clear();
		m_lstBlueSight.clear();

		for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
		{
			if ((*i)->GetStaticType() != StaticType::Static) {

				for (auto j = m_lstColliders.begin(); j != m_lstColliders.end(); ++j)
				{
					if (i != j ){
						if ((*j)->GetStaticType() != StaticType::Static) {
							if (NearLevel((*i)->GetCollisionLevel(), (*j)->GetCollisionLevel()))
							{
								cnt++;
								float sizeA = (*i)->GetCollisionSize();
								float sizeB = (*j)->GetCollisionSize();

								float distance = Vector3::Distance((*i)->GetPosition(), (*j)->GetPosition());
								float collisionLength = sizeA + sizeB;
								if (distance < collisionLength)
								{
									float length = (collisionLength - distance);

									XMFLOAT3 vec3 = Vector3::Subtract((*i)->GetPosition(), (*j)->GetPosition());
									vec3.y = 0;
									vec3 = Vector3::Normalize(vec3);
									pWayFinder->AdjustValueByWallCollision((*i), vec3, length);
									pWayFinder->AdjustValueByWallCollision((*j), vec3, -length);
									(*i)->RegenerateLookAt();
									(*j)->RegenerateLookAt();
								}
							}
						}
						else if ((*j)->GetStaticType() == StaticType::Static) {
							if (NearLevel((*i)->GetCollisionLevel(), (*j)->GetCollisionLevel()))
							{
								float sizeA = (*i)->GetCollisionSize();
								float sizeB = (*j)->GetCollisionSize();

								float distance = Vector3::Distance((*i)->GetPosition(), (*j)->GetPosition());
								float collisionLength = sizeA + sizeB;
								if (distance < collisionLength)
								{
									float length = (collisionLength - distance);

									XMFLOAT3 vec3 = Vector3::Subtract((*i)->GetPosition(), (*j)->GetPosition());
									vec3.y = 0;
									vec3 = Vector3::Normalize(vec3);
									pWayFinder->AdjustValueByWallCollision((*i), vec3, length);
									(*i)->RegenerateLookAt();
								}
							}
						}
					}
				}
			}
		}
		//m_User
		for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
		{
			int x, y;
			x = static_cast<int>(CLAMP((*i)->GetPosition().x / nodeSize, 0, nodeWH.x - 1));
			y = static_cast<int>(CLAMP((*i)->GetPosition().z / nodeSize, 0, nodeWH.y - 1));
			int startLength = static_cast<int>((*i)->GetSightRange() / nodeSize);
			for (int dir = 0; dir < 8; ++dir) {
				SearchSight(x, y,
					dir, startLength, (*i)->GetTeam());
			}
		}

		for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
		{
			int posX = (int)CLAMP((*i)->GetPosition().x / nodeSize, 0, nodeWH.x - 1);
			int posY = (int)CLAMP((*i)->GetPosition().z / nodeSize, 0, nodeWH.y - 1);

			if ((*i)->GetTeam() == Red) {
				if (m_BlueSight[posX][posY].Detected) {
						(*i)->SetDetected(true);
					m_lstBlueSight.push_back((*i));
				}
			}
			else if ((*i)->GetTeam() == Blue) {
				if (m_RedSight[posX][posY].Detected) {
					(*i)->SetDetected(true);
					m_lstRedSight.push_back((*i));
				}
			}
			else {
				if (m_BlueSight[posX][posY].Detected) {
						(*i)->SetDetected(true);
					m_lstBlueSight.push_back((*i));
				}
				if (m_RedSight[posX][posY].Detected) {
						(*i)->SetDetected(true);
					m_lstRedSight.push_back((*i));
				}
			}
		}

	}
}
// type Sphere data1 = 거리 , data2 = 반지름;
// typeSecterForm data1 = 거리, data2 = 각도
void CCollisionManager::RequestCollide(CollisionType type, CCollisionObject * pCol, float data1, float data2, float damage)
{
	if (!g_GameFinished)
	{
		switch (type)
		{
			//8:1..
		case CollisionType::SPHERE:
		{
			for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
			{
				if ((*i) != pCol) {
					TeamType a = (*i)->GetTeam();
					TeamType b = pCol->GetTeam();
					if (a != b) {

						if (NearLevel((*i)->GetCollisionLevel(), pCol->GetCollisionLevel()), true)
						{
							XMFLOAT2 apos = XMFLOAT2((*i)->GetPosition().x, (*i)->GetPosition().z);
							XMFLOAT2 bpos = XMFLOAT2(pCol->GetPosition().x, pCol->GetPosition().z);
							XMFLOAT2 look = XMFLOAT2(pCol->GetLook().x, pCol->GetLook().z);
							float distance = Vector2::Distance(apos, Vector2::Add(bpos, Vector2::ScalarProduct(look, (data1))));
							float collisionLength = (*i)->GetCollisionSize() + (data2);
							if (distance <= collisionLength)
							{
								//std::cout << "col\n";
								(*i)->ReceiveDamage(damage, pCol);
								(*i)->NotifyDamageTeam(pCol->GetTeam());
							}
						}
					}
				}
			}

			break;
		}
		case CollisionType::SECTERFORM:
		{
			for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
			{
				if ((*i) != pCol) {
					TeamType a = (*i)->GetTeam();
					TeamType b = pCol->GetTeam();
					if (a != b) {
						if (NearLevel((*i)->GetCollisionLevel(), pCol->GetCollisionLevel()), true)
						{
							XMFLOAT3 apos = (*i)->GetPosition();
							XMFLOAT3 bpos = pCol->GetPosition();

							float distance = Vector3::Distance(apos, bpos);
							float collisionLength = (data1)+(*i)->GetCollisionSize();

							if (distance <= collisionLength)
							{
								XMFLOAT3 result = Vector3::Add(apos, bpos, -1.0f);

								float angle = Vector3::DotProduct(pCol->GetLook(), Vector3::Normalize(result));

								angle = acos(angle);
								angle = angle * 360 / PI;

								// 시야 각 안에 들어오는가
								if (angle <= data2)
								{
									//std::cout << "col\n";
									(*i)->ReceiveDamage(damage, pCol);
									(*i)->NotifyDamageTeam(pCol->GetTeam());
								}
							}
						}
					}
				}
			}
			break;
		}
		default:
			break;
		}
	}
}

CCollisionObject* CCollisionManager::RequestNearObject(CCollisionObject * pCol, float lengh, TeamType type, bool player)
{
	if (g_GameFinished) return NULL;

	CCollisionObject* nearObject{ NULL };
	float nearDistance = 0;
	std::list<CCollisionObject*>::iterator i;
	std::list<CCollisionObject*>* curList;
	if (type == TeamType::Red)
	{
		curList = &m_lstRedSight;
	}
	else if (type == TeamType::Blue)
	{
		curList = &m_lstBlueSight;
	}
	else
		curList = &m_lstColliders;

	for (i = curList->begin(); i != curList->end(); ++i)
	{
		if ((*i)->GetTeam() == type) continue;
		if (((*i)->GetType() < 4 && player) || !player)
		{
			XMFLOAT2 apos = XMFLOAT2((*i)->GetPosition().x, (*i)->GetPosition().z);
			XMFLOAT2 bpos = XMFLOAT2(pCol->GetPosition().x, pCol->GetPosition().z);
			float distance = Vector2::DistanceSquare(apos, bpos);
			if (distance <= lengh * lengh)
			{
				if (!nearObject) {
					nearDistance = distance;
					nearObject = (*i);
				}
				else if (nearDistance > distance) {
					nearDistance = distance;
					nearObject = (*i);
					//std::cout << "col\n";
				}
			}
		}
	}
	return nearObject;
}
void CCollisionManager::RequestIncreaseExp(CCollisionObject * pCol, float sightRange, TeamType type, UINT exp)
{
	if (g_GameFinished) return;

	TeamType enemytype;
	
	// 요청한(사망한) 오브젝트의 반대 팀 플레이어에 경험치 부여
	if (type == TeamType::Blue)
		enemytype = TeamType::Red;
	else if (type == TeamType::Red)
		enemytype = TeamType::Blue;
	else return;

	for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
	{
		if ((*i)->GetTeam() == enemytype && (*i)->GetTag() >= 10000 && (*i)->GetTag() < 20000) 
		{
			// 해당 오브젝트와 플레이어의 거리가 해당 오브젝트 인근인 경우 경험치를 부여한다.
			XMFLOAT2 playerPos = XMFLOAT2((*i)->GetPosition().x, (*i)->GetPosition().z);
			XMFLOAT2 objectPos = XMFLOAT2(pCol->GetPosition().x, pCol->GetPosition().z);
			float distanceSqr = Vector2::DistanceSquare(playerPos, objectPos);
			if (distanceSqr <= sightRange * sightRange)
			{
				(*i)->ApplyExp(exp);
			}
		}
	}
}

/*
1 0  7
2 ㅁ 6
3 4  5
*/
void CCollisionManager::SearchSight(int startX, int startY, int dir, int slength, TeamType team)
{
	XMFLOAT2 result;
	XMFLOAT2 direction;
	XMFLOAT2 next;

	bool isBuilding = false;
	switch (dir) {
	case 0:
		direction = XMFLOAT2(0, -1);
		next = XMFLOAT2(1, 0);
		break;
	case 1:
		direction = XMFLOAT2(-1, -1);
		next = XMFLOAT2(1, 0);
		break;
	case 2:
		direction = XMFLOAT2(-1, 0);
		next = XMFLOAT2(0, -1);
		break;
	case 3:
		direction = XMFLOAT2(-1, 1);
		next = XMFLOAT2(0, -1);
		break;
	case 4:
		direction = XMFLOAT2(0, 1);
		next = XMFLOAT2(-1, 0);
		break;
	case 5:
		direction = XMFLOAT2(1, 1);
		next = XMFLOAT2(-1, 0);
		break;
	case 6:
		direction = XMFLOAT2(1, 0);
		next = XMFLOAT2(0, 1);
		break;
	case 7:
		direction = XMFLOAT2(1, -1);
		next = XMFLOAT2(0, 1);
		break;
	default:
		direction = XMFLOAT2(0, 0);
		next = XMFLOAT2(0, 0);
		break;
	}

	if (team == Blue)
	{
		m_BlueSight[startX][startY].Detected = true;
		if (m_BlueSight[startX][startY].Static == true) {
			isBuilding = true;
		}
	}
	else if (team == Red) {
		m_RedSight[startX][startY].Detected = true;
		if (m_RedSight[startX][startY].Static == true) {
			isBuilding = true;
		}
	}
	bool buildingout;
	if (isBuilding) {
		buildingout = false;
	}
	else
		buildingout = true;

	for (int i = 0; i < slength; ++i) {
		XMFLOAT2 nNext = Vector2::Normalize(XMFLOAT2(direction.x*slength + (next.x * i), direction.y*slength + (next.y * i)));
		for (int j = 1; j < slength; ++j) {
			result = XMFLOAT2((float)startX, (float)startY);
			if (next.x == 0) {
				result.x += j * direction.x;
				result.y += roundf((j* direction.x*nNext.y / nNext.x));
			}
			else if (next.y == 0) {
				result.y += j * direction.y;
				result.x += roundf((j* direction.y*nNext.x / nNext.y));
			}

			if (result.x < 0 || result.y < 0 || result.x >= nodeWH.x || result.y >= nodeWH.y)
				break;
			float dstSqr = (result.x - startX) * (result.x - startX) + (result.y - startY) * (result.y - startY);

			if (dstSqr <= slength * slength) {
				int resultX{ (int)result.x };
				int resultY{ (int)result.y };
				if (team == Blue)
				{
					m_BlueSight[resultX][resultY].Detected = true;
					if (m_BlueSight[resultX][resultY].Static == true) {
						if (buildingout)
						{
							break;
						}
					}
					else if (m_BlueSight[resultX][resultY].Static == false && buildingout == false) {
						buildingout = true;
					}
				}
				else if (team == Red) {
					m_RedSight[resultX][resultY].Detected = true;
					if (m_RedSight[resultX][resultY].Static == true) {
						if (buildingout)
						{
							break;
						}
					}
					else if (m_RedSight[resultX][resultY].Static == false && buildingout == false) {
						buildingout = true;
					}
				}

			}
			else break;
		}
	}

}


CCollisionManager::~CCollisionManager()
{

	if (m_BlueSight)
	{
		for (int i = 0; i < nodeWH.x; i++)
		{
			Safe_Delete_Array(m_BlueSight[i]);
		}
		Safe_Delete_Array(m_BlueSight);
	}
	if (m_RedSight)
	{
		for (int i = 0; i < nodeWH.x; i++)
		{
			Safe_Delete_Array(m_RedSight[i]);
		}
		Safe_Delete_Array(m_RedSight);
	}
}

int(*CCollisionManager::GetFoW(void))[NODE_HEIGHT]
{
	for (int i = 0; i < NODE_WIDTH; ++i) {
		for (int j = 0; j < NODE_HEIGHT; ++j) {
			if (!m_BlueSight[i][j].Detected)
				Fow[i][j] = 0;
			else
				Fow[i][j] = 1;
		}
	}
	return Fow;
}
