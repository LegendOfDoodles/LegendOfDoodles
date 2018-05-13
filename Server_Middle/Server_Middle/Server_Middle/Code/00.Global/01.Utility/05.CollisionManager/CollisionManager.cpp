#include "stdafx.h"
#include "CollisionManager.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"

CCollisionManager::CCollisionManager()
{

}

void CCollisionManager::GameOver(TeamType type)
{
	TeamType m_Defeat = type;
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

void CCollisionManager::AddCollider(CCollisionObject* pcol)
{
	if (pcol) {
		m_lstColliders.push_back(pcol);

		m_nCollisers++;
	}

}


void CCollisionManager::Update(CWayFinder* pWayFinder)
{
	if (m_Winner == TeamType::None)
	{

		int cnt = 0;

		m_lstColliders.remove_if([](CCollisionObject* obj) { return obj->GetState() == States::Die; });

		for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
		{
			for (auto& j = m_lstColliders.begin(); j != m_lstColliders.end(); ++j)
			{
				if (i != j && (*i)->GetStaticType() != StaticType::Static && (*j)->GetStaticType() != StaticType::Static) {
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
			for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
			{
				if ((*i) != pCol) {
					TeamType a = (*i)->GetTeam();
					TeamType b = pCol->GetTeam();
					if (a != b) {

						if (NearLevel((*i)->GetCollisionLevel(), pCol->GetCollisionLevel()))
						{
							XMFLOAT2 apos = XMFLOAT2((*i)->GetPosition().x, (*i)->GetPosition().z);
							XMFLOAT2 bpos = XMFLOAT2(pCol->GetPosition().x, pCol->GetPosition().z);
							XMFLOAT2 look = XMFLOAT2(pCol->GetLook().x, pCol->GetLook().z);
							float distance = Vector2::Distance(apos, Vector2::Add(bpos, Vector2::ScalarProduct(look, (data1))));
							float collisionLength = (*i)->GetCollisionSize() + (data2);
							if (distance <= collisionLength)
							{
								//std::cout << "col\n";
								(*i)->ReceiveDamage(damage);
							}
						}
					}
				}
			}

			break;
		case CollisionType::SECTERFORM:
			for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
			{
				if ((*i) != pCol) {
					TeamType a = (*i)->GetTeam();
					TeamType b = pCol->GetTeam();
					if (a != b) {
						if (NearLevel((*i)->GetCollisionLevel(), pCol->GetCollisionLevel()))
						{
							XMFLOAT3 apos = (*i)->GetPosition();
							XMFLOAT3 bpos = pCol->GetPosition();

							float distance = Vector3::Distance(apos, bpos);
							float collisionLength = (data1)+(*i)->GetCollisionSize();

							if (distance <= collisionLength)
							{
								XMFLOAT3 result = Vector3::Add(apos, bpos, -1.0f);

								float a = Vector3::DotProduct(pCol->GetLook(), Vector3::Normalize(result));

								a = acos(a);
								a = a * 360 / PI;

								// 시야 각 안에 들어오는가
								if (a <= data2)
								{
									//std::cout << "col\n";
									(*i)->ReceiveDamage(damage);
								}
							}
						}
					}
				}
			}

			break;
		default:
			break;
		}
	}
}

CCollisionObject* CCollisionManager::RequestNearObject(CCollisionObject * pCol, float lengh)
{
	if (m_Winner == TeamType::None)
	{
		CCollisionObject* nearObject{ NULL };
		float nearDistance = 0;
		for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
		{

			if ((*i) != pCol && (*i)->GetTeam() != pCol->GetTeam()) {
				if (NearLevel((*i)->GetCollisionLevel(), pCol->GetCollisionLevel()))
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
							//std::cout << "col\n";
						}
					}
				}
			}
		}
		return nearObject;
	}
	return NULL;
}

CCollisionManager::~CCollisionManager()
{
}
