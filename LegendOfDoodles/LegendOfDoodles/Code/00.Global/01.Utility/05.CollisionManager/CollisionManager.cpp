#include "stdafx.h"
#include "CollisionManager.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"

CCollisionManager::CCollisionManager()
{

}

void CCollisionManager::GameOver(TeamType type)
{
	m_GameOver = true;

	if (type == TeamType::Blue) {
		m_Winner = TeamType::Red;
	}
	if (type == TeamType::Red) {
		m_Winner = TeamType::Blue;
	}
	for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
	{
		(*i)->GameOver();
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

	m_Sight = new NodeMap*[width];
	m_RedSight = new NodeMap*[width];
	for (int i = 0; i < width; ++i) {
		m_Sight[i] = new NodeMap[height];
	}
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			m_Sight[j][i] = map[(i * width) + j];
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

void CCollisionManager::AddNeutralCollider(CCollisionObject * pcol)
{
	if (pcol) {
		m_neutralList.push_back(pcol);
	}
}

void CCollisionManager::AddPlayerCollider(CCollisionObject * pcol)
{
	if (pcol) {
		m_playerList.push_back(pcol);
	}
}


void CCollisionManager::Update(shared_ptr<CWayFinder> pWayFinder)
{
	if (m_Winner == TeamType::None)
	{
		int cnt = 0;
		m_lstColliders.remove_if([](CCollisionObject* obj) { return obj->GetState() == States::Die; });
		for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
		{
			if ((*i)->GetTeam() != m_User) {
				(*i)->SetDetected(false);
			}
		}
		for (int i = 0; i < nodeWH.x; ++i) {
			for (int j = 0; j < nodeWH.y; ++j) {
				m_Sight[i][j].Detected = false;
			}
		}

		m_lstRedSight.clear();
		m_lstBlueSight.clear();

		for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
		{
			if ((*i)->GetStaticType() != StaticType::Static) {

				for (auto j = m_lstColliders.begin(); j != m_lstColliders.end(); ++j)
				{
					if (i != j && (*j)->GetStaticType() != StaticType::Static) {
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
								pWayFinder->AdjustValueByWallCollision((*i), vec3, length *sizeB / (sizeA + sizeB));
								pWayFinder->AdjustValueByWallCollision((*j), vec3, -length * sizeB / (sizeA + sizeB));
								(*i)->RegenerateLookAt();
								(*j)->RegenerateLookAt();
							}
						}
					}
				}
			}
		}
		//m_User


		for (int i = 0; i < NODE_WIDTH; ++i) {
			for (int j = 0; j < NODE_HEIGHT; ++j) {
				Fow[i][j] = 0;
			}
		}

		for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
		{//충돌 매니저에 있는 플레이어와 같은팀 유닛이 있는 공간 확인과 공간내 유닛의 최대 시야거리 저장
			if ((*i)->GetTeam() == m_User) {
				int x, y;
				x = static_cast<int>(CLAMP((*i)->GetPosition().x / nodeSize, 0, nodeWH.x - 1));
				y = static_cast<int>(CLAMP((*i)->GetPosition().z / nodeSize, 0, nodeWH.y - 1));
				Fow[x][y] = max(Fow[x][y], static_cast<int>((*i)->GetSightRange() / nodeSize));
			}
		}
		//시야가 저장된 공간을 중점으로 8방향 기준 시야 확인
		for (int i = 0; i < NODE_WIDTH; ++i) {
			for (int j = 0; j < NODE_HEIGHT; ++j) {
				if (Fow[i][j] != 0) {
					for (int dir = 0; dir < 8; ++dir) {
						SearchSight(i, j, dir, Fow[i][j], Blue);
					}
				}
			}
		}

		for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
		{
			int posX = (int)CLAMP((*i)->GetPosition().x / nodeSize, 0, nodeWH.x - 1);
			int posY = (int)CLAMP((*i)->GetPosition().z / nodeSize, 0, nodeWH.y - 1);

			if ((*i)->GetTeam() == Red) {
				if (m_Sight[posX][posY].Detected) {
					if (m_User == Blue) {
						(*i)->SetDetected(true);
					}
					m_lstBlueSight.push_back((*i));
				}
			}
			else if ((*i)->GetTeam() == Blue) {
				if (m_Sight[posX][posY].Detected) {
					if (m_User == Red) {
						(*i)->SetDetected(true);
					}
					m_lstRedSight.push_back((*i));
				}
			}
			else {
				if (m_Sight[posX][posY].Detected) {
					if (m_User == Blue) {
						(*i)->SetDetected(true);
					}
					m_lstBlueSight.push_back((*i));
				}
				if (m_Sight[posX][posY].Detected) {
					if (m_User == Red) {
						(*i)->SetDetected(true);
					}
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
	if (m_Winner == TeamType::None)
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

								if ((*i)->GetType() != ObjectType::FirstTower
									&& (*i)->GetType() != ObjectType::GOLEM
									&& (*i)->GetType() != ObjectType::Nexus)
								{
									if (pCol->GetType() == ObjectType::FlyingObject
										&&
										(pCol->GetFlyingObjectsType() == FlyingObjectType::Player_ArrowSkill_Q
											|| pCol->GetFlyingObjectsType() == FlyingObjectType::Player_Magic))
									{
										m_pEffectMgr->RequestSpawn((*i)->GetPosition(), pCol->GetLook(), 10, EffectObjectType::Player_ArrowAndFireBall_HitPosition_Effect);
									}
									else
									{
										m_pEffectMgr->RequestSpawn((*i)->GetPosition(), pCol->GetLook(), 10, EffectObjectType::NormallHit_Effect);
									}
								}
								(*i)->ReceiveDamage(damage);
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
									(*i)->ReceiveDamage(damage);
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
	if (m_Winner != TeamType::None) return NULL;

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
			float distance = Vector2::Distance(apos, bpos);
			if (distance <= lengh)
			{
				if (!nearObject) {
					nearDistance = distance;
					nearObject = (*i);
				}
				else if (nearDistance > distance) {
					nearDistance = distance;
					nearObject = (*i);
				}
			}
		}
	}
	return nearObject;
}

CCollisionObject * CCollisionManager::RequestObjectByTag(short tag)
{
	if (m_Winner != TeamType::None) return NULL;

	for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
	{
		if ((*i)->GetTag() == tag)
		{
			return (*i);
		}
	}
	return nullptr;
}

CCollisionObject * CCollisionManager::RequestNeutralByTag(short tag)
{
	if (m_Winner != TeamType::None) return NULL;

	for (auto i = m_neutralList.begin(); i != m_neutralList.end(); ++i)
	{
		if ((*i)->GetTag() == tag)
		{
			return (*i);
		}
	}
	return nullptr;
}

CCollisionObject * CCollisionManager::RequestPlayerByTag(short tag)
{
	if (m_Winner != TeamType::None) return NULL;

	for (auto i = m_playerList.begin(); i != m_playerList.end(); ++i)
	{
		if ((*i)->GetTag() == tag)
		{
			return (*i);
		}
	}
	return nullptr;
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
	//8방향 확인
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

	m_Sight[startX][startY].Detected = true;
	if (m_Sight[startX][startY].Static == true) {
		isBuilding = true;
	}

	bool buildingout = true;
	for (int i = 0; i < slength; ++i) {
		XMFLOAT2 nNext = Vector2::Normalize(XMFLOAT2(direction.x*slength + (next.x * i), direction.y*slength + (next.y * i)));
		if (isBuilding) buildingout = false;
		else buildingout = true;

		for (int j = 1; j < slength; ++j) {
			result = XMFLOAT2((float)startX, (float)startY);
			//
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

				m_Sight[resultX][resultY].Detected = true;
				if (m_Sight[resultX][resultY].Static == true) {
					if (buildingout)
					{
						for (int x = -2; x < 3; ++x) {
							for (int y = -2; y < 3; ++y) {
								if (result.x + x < 0 || result.y + y < 0 || result.x + x >= nodeWH.x || result.y + y >= nodeWH.y)
								{
									continue;
								}
								else if (m_Sight[resultX + x][resultY + y].Static == true) {
									m_Sight[resultX + x][resultY + y].Detected = true;
								}
							}
						}
						break;
					}
				}
				if (m_Sight[resultX][resultY].Static == false && buildingout == false) {
					buildingout = true;
				}
			}
			else break;
		}
	}

}


CCollisionManager::~CCollisionManager()
{

	if (m_Sight)
	{
		for (int i = 0; i < nodeWH.x; i++)
		{
			Safe_Delete_Array(m_Sight[i]);
		}
		Safe_Delete_Array(m_Sight);
	}
}

int(*CCollisionManager::GetFoW(TeamType type))[NODE_HEIGHT]
{
	for (int i = 0; i < NODE_WIDTH; ++i) {
		for (int j = 0; j < NODE_HEIGHT; ++j) {
			if (!m_Sight[i][j].Detected)
				Fow[i][j] = 0;
			else
				Fow[i][j] = 1;
		}
	}
	return Fow;
}
