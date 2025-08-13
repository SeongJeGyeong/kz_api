#include "pch.h"
#include "CollisionManager.h"
#include "../Components/Collider.h"
#include "../Components/OBBCollider.h"
#include "../Components/AABBCollider.h"
#include "../Components/LineCollider.h"
#include "../Objects/Actors/Actor.h"
#include "../Objects/Actors/Player.h"
#include "../Game/Game.h"

void CollisionManager::Init()
{
	// �浹�� ������ ��ü�� ����
	SetBitFlag(ECollisionLayer::PLAYER, ECollisionLayer::PLAYER, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::PLAYER, ECollisionLayer::PLAYER_HITBOX, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::ENEMY, ECollisionLayer::ENEMY, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::ENEMY, ECollisionLayer::ENEMY_HITBOX, ECollisionResponse::C_IGNORE, true);

	SetBitFlag(ECollisionLayer::GROUND, ECollisionLayer::GROUND, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::GROUND, ECollisionLayer::WALL, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::GROUND, ECollisionLayer::CEILING, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::GROUND, ECollisionLayer::PLATFORM, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::GROUND, ECollisionLayer::STAIR, ECollisionResponse::C_IGNORE, true);

	SetBitFlag(ECollisionLayer::WALL, ECollisionLayer::WALL, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::WALL, ECollisionLayer::GROUND, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::WALL, ECollisionLayer::CEILING, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::WALL, ECollisionLayer::PLATFORM, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::WALL, ECollisionLayer::STAIR, ECollisionResponse::C_IGNORE, true);

	SetBitFlag(ECollisionLayer::CEILING, ECollisionLayer::CEILING, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::CEILING, ECollisionLayer::GROUND, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::CEILING, ECollisionLayer::WALL, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::CEILING, ECollisionLayer::PLATFORM, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::CEILING, ECollisionLayer::STAIR, ECollisionResponse::C_IGNORE, true);

	SetBitFlag(ECollisionLayer::PLATFORM, ECollisionLayer::PLATFORM, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::PLATFORM, ECollisionLayer::GROUND, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::PLATFORM, ECollisionLayer::WALL, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::PLATFORM, ECollisionLayer::CEILING, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::PLATFORM, ECollisionLayer::STAIR, ECollisionResponse::C_IGNORE, true);

	SetBitFlag(ECollisionLayer::STAIR, ECollisionLayer::STAIR, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::STAIR, ECollisionLayer::GROUND, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::STAIR, ECollisionLayer::WALL, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::STAIR, ECollisionLayer::CEILING, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::STAIR, ECollisionLayer::PLATFORM, ECollisionResponse::C_IGNORE, true);
}

void CollisionManager::Update()
{
	for (int32 receiveLayer = 0; receiveLayer < ECollisionLayer::END; ++receiveLayer)
	{
		for (auto receiveCollider : _colliderList[(ECollisionLayer)receiveLayer])
		{
			for (int32 sendLayer = 0; sendLayer < ECollisionLayer::END; ++sendLayer)
			{
				if (COLLISION_BIT_MASK_IGNORE[receiveLayer] & (uint8)(1 << sendLayer)) continue;

				for (auto sendCollider : _colliderList[(ECollisionLayer)sendLayer])
				{
					bool first = true;

					CollisionMapID mapId = CollisionMapID(receiveCollider->GetColliderId(), sendCollider->GetColliderId());

					auto iter = _collisionMap.find(mapId.ToUInt64());

					if (_collisionMap.end() != iter)
					{
						// �̹� �浹 �˻� �Ϸ������� �ѱ��.
						if (iter->second)
						{
							continue;
						}
						else
						{
							first = false;
						}
					}

					CollisionInfo info;
					if (CheckCollision(receiveCollider, sendCollider, info))
					{
						if (first)
						{
							//sendCollider->GetOwner()->OnCollisionBeginOverlap(receiveCollider);
							receiveCollider->GetOwner()->OnCollisionBeginOverlap(info);
							_collisionMap.emplace(mapId.ToUInt64(), true);
						}
						else
						{
							//sendCollider->GetOwner()->OnCollisionStayOverlap(receiveCollider);
							receiveCollider->GetOwner()->OnCollisionStayOverlap(info);
						}

						//if (COLLISION_BIT_MASK_BLOCK[receiveLayer] & (uint8)(1 << sendLayer))
						//{
						//	sendCollider->GetOwner()->OnCollisionHit(receiveCollider);
						//	receiveCollider->GetOwner()->OnCollisionHit(sendCollider);
						//}
						//else
						//{
						//	sendCollider->GetOwner()->OnCollisionBeginOverlap(receiveCollider);
						//	receiveCollider->GetOwner()->OnCollisionBeginOverlap(sendCollider);
						//}
					}
					else
					{
						if (!first)
						{
							//sendCollider->GetOwner()->OnCollisionEndOverlap(receiveCollider);
							receiveCollider->GetOwner()->OnCollisionEndOverlap(info);
							_collisionMap.erase(iter);
						}
					}
				}
			}
		}
	}

}

void CollisionManager::Render(HDC hdc)
{
}

void CollisionManager::AddCollider(Collider* collider)
{
	_colliderList[collider->GetCollisionLayer()].push_back(collider);
}

void CollisionManager::PostUpdate()
{
	for (auto& col : _collisionMap)
	{
		col.second = false;
	}
}

void CollisionManager::SetIgnoreFlag(ECollisionLayer type, ECollisionLayer ignore)
{
	COLLISION_BIT_MASK_IGNORE[(int8)type] |= ((uint8)1 << (int8)ignore);
	COLLISION_BIT_MASK_IGNORE[(int8)ignore] |= ((uint8)1 << (int8)type);
}

void CollisionManager::SetBitFlag(ECollisionLayer layer1, ECollisionLayer layer2, ECollisionResponse responseType, bool on)
{
	switch (responseType)
	{
	case C_IGNORE:
		if (on)
		{
			// ����
			COLLISION_BIT_MASK_IGNORE[(int8)layer1] |= ((uint8)1 << (int8)layer2);
			COLLISION_BIT_MASK_IGNORE[(int8)layer2] |= ((uint8)1 << (int8)layer1);
		}
		else
		{
			// ����
			COLLISION_BIT_MASK_IGNORE[(int8)layer1] &= ~((uint8)1 << (int8)layer2);
			COLLISION_BIT_MASK_IGNORE[(int8)layer2] &= ~((uint8)1 << (int8)layer1);
		}
		break;
	case C_BLOCK:
		if (on)
		{
			// ����
			COLLISION_BIT_MASK_BLOCK[(int8)layer1] |= ((uint8)1 << (int8)layer2);
			COLLISION_BIT_MASK_BLOCK[(int8)layer2] |= ((uint8)1 << (int8)layer1);
		}
		else
		{
			// ����
			COLLISION_BIT_MASK_BLOCK[(int8)layer1] &= ~((uint8)1 << (int8)layer2);
			COLLISION_BIT_MASK_BLOCK[(int8)layer2] &= ~((uint8)1 << (int8)layer1);
		}
		break;
	default:
		break;
	}
}

bool CollisionManager::CheckCollision(Collider* receive, Collider* send, CollisionInfo& info)
{
	if (receive->GetCollisionLayer() == ECollisionLayer::PLAYER)
	{
		float playerWidth = receive->GetWidth();
		float playerHeight = receive->GetHeight();
		float halfWidth = playerWidth * 0.5f;
		float halfHeight = playerHeight * 0.5f;

		Player* player = static_cast<Player*>(receive->GetOwner());

		// �÷��̾� AABB ���
		RECT playerOldRect = {
			receive->GetPos().x - halfWidth,
			receive->GetPos().y - halfHeight,
			receive->GetPos().x + halfWidth,
			receive->GetPos().y + halfHeight
		};

		RECT playerNewRect = {
			player->GetNewPos().x - halfWidth,
			player->GetNewPos().y - halfHeight,
			player->GetNewPos().x + halfWidth,
			player->GetNewPos().y + halfHeight
		};

		switch (send->GetCollisionLayer())
		{
		case ECollisionLayer::GROUND :
			info = CheckAABBGroundCollision(playerOldRect, playerNewRect, send);
			break;
		case ECollisionLayer::WALL :
			info = CheckLineWallCollision(player->GetPos(), player->GetNewPos(), playerWidth, playerHeight, send);
			break;
		case ECollisionLayer::STAIR :
			info = CheckLineStiarCollision2(player->GetPos(), player->GetNewPos(), playerWidth, playerHeight, send, player->GetOnStair());
			break;
		case ECollisionLayer::PLATFORM :
			info = CheckLinePlatformCollision(player->GetPos(), player->GetNewPos(), playerWidth, playerHeight, send);
			break;
		default:
			break;
		}

		if (info.isColliding) return true;
	}
	else
	{
		EColliderType receiveType = receive->GetColliderType();
		EColliderType sendType = send->GetColliderType();

		if (receiveType == EColliderType::OBB && receiveType == sendType)
		{
			return CheckBetweenOBB(static_cast<OBBCollider*>(receive), static_cast<OBBCollider*>(send), info);
		}
		else if (receiveType == EColliderType::AABB && receiveType == sendType)
		{
			return CheckBetweenAABB(static_cast<AABBCollider*>(receive), static_cast<AABBCollider*>(send), info);
		}
		else if (receiveType == EColliderType::AABB && sendType == EColliderType::LINE)
		{
			return CheckAABBtoLine(static_cast<AABBCollider*>(receive), static_cast<LineCollider*>(send), info);
		}
	}

	return false;
}

bool CollisionManager::CheckBetweenOBB(OBBCollider* receive, OBBCollider* send, CollisionInfo& info)
{
	pair<Vector2, Vector2> receiveAxes = receive->GetAxes();
	pair<Vector2, Vector2> sendAxes = send->GetAxes();

	Vector2 axes[4] = { receiveAxes.first, receiveAxes.second, sendAxes.first, sendAxes.second };
	Vector2 minAxis;
	float minOverlap = INFINITY;

	for (int i = 0; i < 4; ++i)
	{
		// first : min, second : max
		pair<float, float> proj1 = receive->ProjectionAxis(axes[i]);
		pair<float, float> proj2 = send->ProjectionAxis(axes[i]);

		if (!CheckSeparatingAxis(proj1, proj2))
		{
			return false;
		}

		float overlap = min(proj1.second, proj2.second) - max(proj1.first, proj2.first);
		if (overlap < minOverlap) {
			minOverlap = overlap;
			minAxis = axes[i];
		}

	}

	if (receive->GetPos().x < send->GetPos().x) minAxis = { -minAxis.x, -minAxis.y };

	receive->SetHittedNormal(minAxis);
	Vector2 hitPos = CalculateHitPos(receive, send, minAxis);
	receive->SetHitPos(hitPos);
	
	// MTV(�ּ��̵�����) : �� ������ ������ ��, �� ������ ���� ��ġ�� �ʵ��� �� ������ �̵���Ű�� ���� �� ���� ���� ����
	// SAT���� MTV ã�� �� : ������ ���������� ������ ��, ������������ ��ġ�� �κ��� ���� ���� ���� MTV
	// MTV�� ������ +-�� �����Ѵ�. ���� ������ �������� �о�� ���� ���� -, ������ ������ ���������� �о�� ���� ���� +
	receive->SetHitMTV(minAxis * minOverlap);

	// ��� �࿡�� ��ġ�Ƿ� �浹
	return true;
}

bool CollisionManager::CheckBetweenSAT(OBBCollider* receive, OBBCollider* send, CollisionInfo& info)
{
	pair<Vector2, Vector2> receiveAxes = receive->GetAxes();
	pair<Vector2, Vector2> sendAxes = send->GetAxes();

	Vector2 axes[4] = { receiveAxes.first, receiveAxes.second, sendAxes.first, sendAxes.second };
	Vector2 minAxis;
	float minOverlap = INFINITY;

	for (int i = 0; i < 4; ++i)
	{
		// first : min, second : max
		pair<float, float> proj1 = receive->ProjectionAxis(axes[i]);
		pair<float, float> proj2 = send->ProjectionAxis(axes[i]);

		if (!CheckSeparatingAxis(proj1, proj2))
		{
			return false;
		}

		float overlap = min(proj1.second, proj2.second) - max(proj1.first, proj2.first);
		if (overlap < minOverlap) {
			minOverlap = overlap;
			minAxis = axes[i];
		}

	}

	if (receive->GetPos().x < send->GetPos().x) minAxis = { -minAxis.x, -minAxis.y };

	receive->SetHittedNormal(minAxis);
	Vector2 hitPos = CalculateHitPos(receive, send, minAxis);
	receive->SetHitPos(hitPos);

	// MTV(�ּ��̵�����) : �� ������ ������ ��, �� ������ ���� ��ġ�� �ʵ��� �� ������ �̵���Ű�� ���� �� ���� ���� ����
	// SAT���� MTV ã�� �� : ������ ���������� ������ ��, ������������ ��ġ�� �κ��� ���� ���� ���� MTV
	// MTV�� ������ +-�� �����Ѵ�. ���� ������ �������� �о�� ���� ���� -, ������ ������ ���������� �о�� ���� ���� +
	receive->SetHitMTV(minAxis * minOverlap);

	// ��� �࿡�� ��ġ�Ƿ� �浹
	return true;
}

bool CollisionManager::CheckBetweenAABB(AABBCollider* receive, AABBCollider* send, CollisionInfo& info)
{
	Vector2 receiveMin = receive->GetAABBMin();
	Vector2 receiveMax = receive->GetAABBMax();

	Vector2 sendMin = send->GetAABBMin();
	Vector2 sendMax = send->GetAABBMax();

	Vector2 receiveCenter = receive->GetCenter();
	Vector2 sendCenter = send->GetCenter();

	// �浹ü���� �߽��� �մ� ����
	double nx = sendCenter.x - receiveCenter.x;
	double ny = sendCenter.y - receiveCenter.y;

	// �浹ü�� ����, ������ ����
	Vector2 receiveExtent = (receiveMax - receiveMin) * 0.5f;
	Vector2 sendExtent = (sendMax - sendMin) * 0.5f;

	// n.x�� 1�̰ų� -1�̸� x���� �浹
	double overlapX = receiveExtent.x + sendExtent.x - abs(nx);
	// n.y�� 1�̰ų� -1�̸� y���� �浹
	double overlapY = receiveExtent.y + sendExtent.y - abs(ny);

	if (overlapX < 0 || overlapY < 0) return false;

	Vector2 normal;
	Vector2 hitPos;

	// ��, �Ʒ� �浹
	if(overlapX > overlapY)
	{
		if (ny < 0)
		{
			normal = Vector2(0, -1);
		}
		else
		{
			normal = Vector2(0, 1);
		}

		double penetration = overlapY;
		// �簢������ ��ġ�� ��ġ�� 2����
		int32 contactCount = 2;
		Vector2 contactPoint1 = Vector2(max(receiveMin.x, sendMin.x), (ny < 0 ? receiveMin.y : receiveMax.y));
		Vector2 contactPoint2 = Vector2(max(receiveMax.x, sendMax.x), contactPoint1.y);
		hitPos = { (contactPoint1.x + contactPoint2.x) * 0.5f, (contactPoint1.y + contactPoint2.y) * 0.5f };
	}
	// ����, ������ �浹
	else
	{
		if (nx < 0)
		{
			normal = Vector2(-1, 0);
		}
		else
		{
			normal = Vector2(1, 0);
		}

		double penetration = overlapX;
		int32 contactCount = 2;
		Vector2 contactPoint1 = Vector2((nx < 0 ? receiveMin.x : receiveMax.x), max(receiveMin.y, sendMin.y));
		Vector2 contactPoint2 = Vector2(contactPoint1.x, min(receiveMax.y, sendMax.y));
		hitPos = { (contactPoint1.x + contactPoint2.x) * 0.5f, (contactPoint1.y + contactPoint2.y) * 0.5f };
	}


	receive->SetHittedNormal(normal);
	receive->SetHitPos(hitPos);
	receive->SetHitInfo(hitPos, normal);
	return true;
}

bool CollisionManager::CheckAABBtoLine(AABBCollider* receive, LineCollider* send, CollisionInfo& info)
{
	RECT rect = receive->GetRect();

	float t = 0.f;

	Vector2 start = send->GetStartPoint();
	Vector2 end = send->GetEndPoint();
	Vector2 dir = end - start;
	float lineLength = dir.Length();

	// start �������� �浹
	float tMin = 0.0f;
	// end �������� �浹
	float tMax = lineLength;
	//float tMax = 1.0f;

	// x�� �˻�
	if (dir.x != 0)
	{
		float tx1 = (rect.left - start.x) / dir.x;
		float tx2 = (rect.right - start.x) / dir.x;

		Vector2 nx = (tx1 < tx2) ? Vector2(-1, 0) : Vector2(1, 0);
		if (tx1 > tx2) std::swap(tx1, tx2);

		if (tx1 > tMin)
		{
			info.vHitNormal = nx;
		}

		tMin = max(tMin, tx1);
		tMax = min(tMax, tx2);

		if (tMin > tMax)
			return false; // �浹 �� ��
	}
	else if (start.x < rect.left || start.x > rect.right)
	{
		return false; // Y������ �����ϰ� �ۿ� ����
	}

	// y�� �˻�
	if (dir.y != 0)
	{
		float ty1 = (rect.top - start.y) / dir.y;
		float ty2 = (rect.bottom - start.y) / dir.y;

		// ���� �������κ��� �������ִ� �Ÿ� ����
		// ty1 �� ty2���� �۴ٸ�, ������ �Ʒ� �����̴�.
		Vector2 ny = (ty1 < ty2) ? Vector2(0, -1) : Vector2(0, 1);

		if (ty1 > ty2) std::swap(ty1, ty2);

		if (ty1 > tMin)
		{
			info.vHitNormal = ny;
		}

		tMin = max(tMin, ty1);
		tMax = min(tMax, ty2);

		if (tMin > tMax)
			return false; // �浹 �� ��
	}
	else if (start.y < rect.top || start.y > rect.bottom)
	{
		return false;
	}

	// ������ �簢�� ���ο� �ִ� ��� ����ó��
	// ������ ������⿡ ���� �� �������� ���ݴ� ������ �������� normal �� �������ش�.
	if (info.vHitNormal == Vector2(0, 0))
	{
		if (fabs(dir.x) > fabs(dir.y))
		{
			info.vHitNormal = (dir.x > 0) ? Vector2(-1, 0) : Vector2(1, 0);
		}
		else
		{
			info.vHitNormal = (dir.y > 0) ? Vector2(0, -1) : Vector2(0, 1);
		}
	}

	// �浹 ��� ����
	info.collisionPoint = start + dir * tMin;

	if (tMax <= lineLength) {
		Vector2 exitPoint = start + dir * tMax;
		info.collisionPoint = (info.collisionPoint + exitPoint) * 0.5f;
	}

	// ����ó�� �ϳ���.
	info.collisionPoint += info.vHitNormal * 0.01f; // �ణ�� �������� �־� ��Ȯ�� �浹 ��ġ�� ����

	// �浹 ��.
	if (t < tMin)
	{
		return true;
	}

	return false;
}

bool CollisionManager::CheckSeparatingAxis(pair<float, float> proj1, pair<float, float> proj2)
{
	if (proj1.second < proj2.first || proj2.second < proj1.first)
	{
		return false; // �и��� �߰�
	}

	return true;
}

Vector2 CollisionManager::CalculateHitPos(OBBCollider* receive, OBBCollider* send, Vector2 normal)
{
	std::vector<Vector2> vertices1 = receive->GetVertices();
	std::vector<Vector2> vertices2 = send->GetVertices();

	// ���� �������� ���� ����� �������� ã��
	float maxDist1 = -INFINITY;
	float maxDist2 = -INFINITY;
	Vector2 closestVertex1, closestVertex2;

	// obb1���� ���� �������� ���� �� ��
	for (auto& vertex : vertices1) {
		float dist = vertex.Dot(normal);
		if (dist > maxDist1) {
			maxDist1 = dist;
			closestVertex1 = vertex;
		}
	}

	// obb2���� ���� �ݴ� �������� ���� �� ��
	Vector2 invNormal = { -normal.x,-normal.y };
	for (auto& vertex : vertices2) {
		float dist = vertex.Dot(invNormal);
		if (dist > maxDist2) {
			maxDist2 = dist;
			closestVertex2 = vertex;
		}
	}

	// �� ���� ������ ���������� ���
	return (closestVertex1 + closestVertex2) * 0.5f;
}

void CollisionManager::ExecuteCollisionFunc(Collider* receive, Collider* send)
{
	//send->GetCollider()->
}

bool CollisionManager::CheckCollision2(Collider* receive, Collider* send, CollisionInfo& info)
{
	return false;
}

// �÷��̾��� ���� line�� �ٸ� �浹ü�� �浹�ϴ��� �˻�
bool CollisionManager::CheckBlockedCollision(Player* player, Vector2 start, Vector2 end, CollisionInfo& info)
{
	// ��ü ���͸� �� ��ȸ�ϸ鼭, Line, AABB �浹üũ
		// 1 : N

	float tMin = FLT_MAX;
	Actor* closestActor = nullptr;

	for (int32 type = ECollisionLayer::GROUND; type <= ECollisionLayer::PLATFORM; ++type)
	{
		for (auto collider : _colliderList[type])
		{
			float playerWidth = player->GetCollider()->GetWidth();
			float playerHeight = player->GetCollider()->GetHeight();

			if (collider->GetColliderType() == EColliderType::AABB)
			{
				float width = collider->GetWidth();
				float height = collider->GetHeight();

				RECT rect = static_cast<AABBCollider*>(collider)->GetRect();

				rect.left -= playerWidth * 0.5f;
				rect.right += playerWidth * 0.5f;
				rect.top -= playerHeight * 0.5f;
				rect.bottom += playerHeight * 0.5f;

				float t = 0.f;
				Vector2 normal;
				Vector2 hitPos;
				if (LineIntersectsAABB(start, end, rect, normal, hitPos, t))
				{
					if (normal.y < 0)
					{
						player->SetIsGround(true);
					}
					if (normal.x != 0)
					{
						player->SetIsWall(true);
					}
					// �� ����� �浹ü�� �浹 ����
					if (t < tMin)
					{
						info.vHitNormal = normal;
						info.collisionPoint = hitPos;
						tMin = t;
						closestActor = collider->GetOwner();
					}
				}
			}
			else
			{
				Vector2 playerMoveDir = end - start;
				Vector2 lineDir = collider->GetEndPoint() - collider->GetStartPoint();
				// ���� ��������
				Vector2 lineNormal = Vector2(-lineDir.y, lineDir.x).GetNormalize();

				// �÷��̾� AABB�� ������ ���� �������� ������ ���� ���
				float projectedExtent = fabs(lineNormal.x * (playerWidth * 0.5f)) + fabs(lineNormal.y * (playerHeight * 0.5f));

				// ������ �������� Ȯ��
				Vector2 expandedStart1 = collider->GetStartPoint() + lineNormal * projectedExtent;
				Vector2 expandedEnd1 = collider->GetEndPoint() + lineNormal * projectedExtent;
				Vector2 expandedStart2 = collider->GetStartPoint() - lineNormal * projectedExtent;
				Vector2 expandedEnd2 = collider->GetEndPoint() - lineNormal * projectedExtent;

				Vector2 normal, hitPos;
				float t;

				// �÷��̾� �߽����� �̵� ��ο� Ȯ��� ���ε��� ���� �˻�
				if (LineIntersectsLine(start, end, expandedStart1, expandedEnd1, normal, hitPos, t))
				{
					if (t < tMin && t >= 0.0f && t <= 1.0f)
					{
						tMin = t;
						info.vHitNormal = { -normal.x, -normal.y};
						info.collisionPoint = hitPos;
						closestActor = collider->GetOwner();
					}
				}
				else if (LineIntersectsLine(start, end, expandedStart2, expandedEnd2, normal, hitPos, t))
				{
					if (t < tMin && t >= 0.0f && t <= 1.0f)
					{
						tMin = t;
						info.vHitNormal = normal;
						info.collisionPoint = hitPos;
						closestActor = collider->GetOwner();
					}
				}
			}
		}
	}

	if (closestActor != nullptr)
	{
		return true;
	}

	return false;
}

CollisionInfo CollisionManager::CheckAABBGroundCollision(const RECT& playerOldRect, const RECT& playerNewRect, Collider* groundCollider)
{
	CollisionInfo result = {};
	result.collisionLayer = groundCollider->GetCollisionLayer();
	float groundWidth = groundCollider->GetWidth();
	float groundHeight = groundCollider->GetHeight();
	Vector2 groundPos = groundCollider->GetPos();

	RECT groundRect = {
		groundPos.x - groundWidth * 0.5f,
		groundPos.y - groundHeight * 0.5f,
		groundPos.x + groundWidth * 0.5f,
		groundPos.y + groundHeight * 0.5f
	};

	// ���� ��ġ���� ��ġ���� Ȯ��
	if (!(playerNewRect.left < groundRect.right &&
		playerNewRect.right > groundRect.left &&
		playerNewRect.top < groundRect.bottom &&
		playerNewRect.bottom > groundRect.top))
	{
		return result; // ��ġ�� ����
	}

	// ���� ��ġ������ ��ġ�� �ʾҴ��� Ȯ�� (���ο� �浹����)
	bool wasOverlapping = (playerOldRect.left < groundRect.right &&
		playerOldRect.right > groundRect.left &&
		playerOldRect.top < groundRect.bottom &&
		playerOldRect.bottom > groundRect.top);

	if (wasOverlapping)
	{
		return result; // �̹� �����ִ� ���¶�� ���ο� �ٴ� �浹�� �ƴ�
	}

	// �÷��̾ ������ �Ʒ��� �̵��ߴ��� Ȯ��
	//if (playerOldRect.bottom <= groundRect.top && playerNewRect.bottom > groundRect.top)
	//{
	//	result.isColliding = true;
	//	result.penetrationDepth = playerNewRect.bottom - groundRect.top;
	//	result.collisionPoint = Vector2(
	//		(max(playerNewRect.left, groundRect.left) + min(playerNewRect.right, groundRect.right)) * 0.5f,
	//		groundRect.top
	//	);
	//	result.vHitNormal = Vector2(0, -1); // ���� ����
	//	result.groundActor = groundCollider->GetOwner();
	//}

		// ��� ���⿡�� �浹�ߴ��� Ȯ��
	bool fromTop = (playerOldRect.bottom <= groundRect.top && playerNewRect.bottom > groundRect.top);		// ������ �Ʒ���
	bool fromBottom = (playerOldRect.top >= groundRect.bottom && playerNewRect.top < groundRect.bottom);	// �Ʒ����� ����
	bool fromLeft = (playerOldRect.right <= groundRect.left && playerNewRect.right > groundRect.left);	// ���ʿ��� ����������
	bool fromRight = (playerOldRect.left >= groundRect.right && playerNewRect.left < groundRect.right);	// �����ʿ��� ��������

	// ���� ���⿡�� ���ÿ� �浹�� ���, ħ�� ���̰� ���� ���� ������ ����
	float topPenetration = fromTop ? (playerNewRect.bottom - groundRect.top) : FLT_MAX;
	float bottomPenetration = fromBottom ? (groundRect.bottom - playerNewRect.top) : FLT_MAX;
	float leftPenetration = fromLeft ? (playerNewRect.right - groundRect.left) : FLT_MAX;
	float rightPenetration = fromRight ? (groundRect.right - playerNewRect.left) : FLT_MAX;

	float minPenetration = min(topPenetration, bottomPenetration);
	minPenetration = min(minPenetration, leftPenetration);
	minPenetration = min(minPenetration, rightPenetration);

	if (minPenetration == FLT_MAX)
		return result; // �浹���� ����

	result.isColliding = true;
	result.penetrationDepth = minPenetration;
	result.groundActor = groundCollider->GetOwner();

	// ���� ���� ħ�� ���̿� ���� �浹 ���� ����
	if (minPenetration == topPenetration) // ������ �Ʒ��� (�ٴ�)
	{
		result.collisionPoint = Vector2(
			(max(playerNewRect.left, groundRect.left) + min(playerNewRect.right, groundRect.right)) * 0.5f,
			groundRect.top
		);
		result.vHitNormal = Vector2(0, -1); // ���� ����
		result.hitCorner = 0; // �ٴ� �浹
	}
	else if (minPenetration == bottomPenetration) // �Ʒ����� ���� (õ��)
	{
		result.collisionPoint = Vector2(
			(max(playerNewRect.left, groundRect.left) + min(playerNewRect.right, groundRect.right)) * 0.5f,
			groundRect.bottom
		);
		result.vHitNormal = Vector2(0, 1); // �Ʒ��� ����
		result.hitCorner = 3; // õ�� �浹
	}
	else if (minPenetration == leftPenetration) // ���ʿ��� ���������� (������ ��)
	{
		result.collisionPoint = Vector2(
			groundRect.left,
			(max(playerNewRect.top, groundRect.top) + min(playerNewRect.bottom, groundRect.bottom)) * 0.5f
		);
		result.vHitNormal = Vector2(-1, 0); // ���� ����
		result.hitCorner = 1; // ���� �� �浹
	}
	else if (minPenetration == rightPenetration) // �����ʿ��� �������� (���� ��)
	{
		result.collisionPoint = Vector2(
			groundRect.right,
			(max(playerNewRect.top, groundRect.top) + min(playerNewRect.bottom, groundRect.bottom)) * 0.5f
		);
		result.vHitNormal = Vector2(1, 0); // ������ ����
		result.hitCorner = 2; // ������ �� �浹
	}

	return result;
}

CollisionInfo CollisionManager::CheckLinePlatformCollision(Vector2 playerOldPos, Vector2 playerNewPos, float playerWidth, float playerHeight, Collider* lineCollider)
{
	CollisionInfo result = {};
	result.collisionLayer = lineCollider->GetCollisionLayer();
	Vector2 lineStart = lineCollider->GetStartPoint();
	Vector2 lineEnd = lineCollider->GetEndPoint();
	Vector2 lineDir = lineEnd - lineStart;

	// ������ ���� ���� ���
	Vector2 lineNormal = Vector2(-lineDir.y, lineDir.x).GetNormalize();

	// ������ ������ ���ϵ��� ���� (Y�� �����ϴ� ������ ����)
	if (lineNormal.y > 0) lineNormal *= -1;

	float halfWidth = playerWidth * 0.5f;
	float halfHeight = playerHeight * 0.5f;

	Vector2 playerOldBottom = Vector2(playerOldPos.x, playerOldPos.y + halfHeight);
	Vector2 playerNewBottom = Vector2(playerNewPos.x, playerNewPos.y + halfHeight);

	// �÷��̾� �ٴ��� �¿� ������
	Vector2 oldBottomLeft = Vector2(playerOldPos.x - halfWidth, playerOldPos.y + halfHeight);
	Vector2 oldBottomRight = Vector2(playerOldPos.x + halfWidth, playerOldPos.y + halfHeight);
	Vector2 newBottomLeft = Vector2(playerNewPos.x - halfWidth, playerNewPos.y + halfHeight);
	Vector2 newBottomRight = Vector2(playerNewPos.x + halfWidth, playerNewPos.y + halfHeight);

	// �÷��̾��� �̵� ��ο� ������ ������ �˻�
	std::vector<Vector2> intersectionPoints;
	float closestT = FLT_MAX;
	Vector2 closestIntersection;

	// �÷��̾� �ٴ� �߾�, ����, ������ �̵� ��� �˻�
	std::vector<std::pair<Vector2, Vector2>> trajectories = {
		{playerOldBottom, playerNewBottom},
		{oldBottomLeft, newBottomLeft},
		{oldBottomRight, newBottomRight}
	};

	for (const auto& trajectory : trajectories)
	{
		Vector2 intersection;
		float t;
		float cross;
		if (LineIntersectsLineSegment(trajectory.first, trajectory.second,
			lineStart, lineEnd, intersection, t, cross))
		{
			if (t >= 0.0f && t <= 1.0f && t < closestT)
			{
				closestT = t;
				closestIntersection = intersection;
			}
		}
	}

	if (closestT < FLT_MAX)
	{
		// �÷��̾ ������ �Ʒ��� �����ߴ��� Ȯ��
		Vector2 moveDir = playerNewPos - playerOldPos;
		if (moveDir.y > 0 && lineNormal.Dot(moveDir) < 0) // �Ʒ��� �̵��ϸ鼭 ���ο� ����
		{
			result.isColliding = true;
			result.collisionPoint = closestIntersection;
			result.vHitNormal = lineNormal;
			result.groundActor = lineCollider->GetOwner();

			// ħ�� ���� ���
			Vector2 currentBottom = Vector2(playerNewPos.x, playerNewPos.y + halfHeight);
			result.penetrationDepth = max(0.0f,
				currentBottom.y - (closestIntersection.y + lineNormal.y * 0.1f));
		}
	}

	return result;
}

vector<CollisionInfo> CollisionManager::CheckPlayerCollision(Player* player, Vector2 playerOldPos, Vector2 playerNewPos)
{
	vector<CollisionInfo> results;

	float playerWidth = player->GetCollider()->GetWidth();
	float playerHeight = player->GetCollider()->GetHeight();
	float halfWidth = playerWidth * 0.5f;
	float halfHeight = playerHeight * 0.5f;

	// �÷��̾� AABB ���
	RECT playerOldRect = {
		playerOldPos.x - halfWidth,
		playerOldPos.y - halfHeight,
		playerOldPos.x + halfWidth,
		playerOldPos.y + halfHeight
	};

	RECT playerNewRect = {
		playerNewPos.x - halfWidth,
		playerNewPos.y - halfHeight,
		playerNewPos.x + halfWidth,
		playerNewPos.y + halfHeight
	};

	// GROUND, WALL, STAIR, PLATFORM ��� ���̾� �˻�
	for (int32 layer = ECollisionLayer::GROUND; layer <= ECollisionLayer::STAIR; ++layer)
	{
		for (auto collider : _colliderList[layer])
		{
			CollisionInfo result;

			if (collider->GetColliderType() == EColliderType::AABB)
			{
				// AABB �浹 �˻�
				if (layer == ECollisionLayer::GROUND)
				{
					result = CheckAABBGroundCollision(playerOldRect, playerNewRect, collider);
				}
				else if (layer == ECollisionLayer::WALL)
				{
					result = CheckAABBWallCollision(playerOldRect, playerNewRect, collider);
				}
			}
			else if (collider->GetColliderType() == EColliderType::LINE)
			{
				// Line �浹 �˻�
				if (layer == ECollisionLayer::PLATFORM)
				{
					result = CheckLinePlatformCollision(playerOldPos, playerNewPos, playerWidth, playerHeight, collider);
				}
				else if (layer == ECollisionLayer::WALL)
				{
					result = CheckLineWallCollision(playerOldPos, playerNewPos, playerWidth, playerHeight, collider);
				}
				else if (!player->GetOnStair() && layer == ECollisionLayer::STAIR)
				{
					result = CheckLineStiarCollision2(playerOldPos, playerNewPos, playerWidth, playerHeight, collider, player->GetOnStair());
				}
			}

			if (result.isColliding)
			{
				results.push_back(result);
			}
		}
	}

	return results;
}

CollisionInfo CollisionManager::CheckAABBWallCollision(const RECT& playerOldRect, const RECT& playerNewRect, Collider* wallCollider)
{
	CollisionInfo result = {};
	result.collisionLayer = wallCollider->GetCollisionLayer();

	float wallWidth = wallCollider->GetWidth();
	float wallHeight = wallCollider->GetHeight();
	Vector2 wallPos = wallCollider->GetPos();

	RECT wallRect = {
		wallPos.x - wallWidth * 0.5f,
		wallPos.y - wallHeight * 0.5f,
		wallPos.x + wallWidth * 0.5f,
		wallPos.y + wallHeight * 0.5f
	};

	// ���� ��ġ���� ��ġ���� Ȯ��
	if (!(playerNewRect.left < wallRect.right &&
		playerNewRect.right > wallRect.left &&
		playerNewRect.top < wallRect.bottom &&
		playerNewRect.bottom > wallRect.top))
	{
		return result; // ��ġ�� ����
	}

	// ���� ��ġ������ ��ġ�� �ʾҴ��� Ȯ�� (���ο� �浹����)
	bool wasOverlapping = (playerOldRect.left < wallRect.right &&
		playerOldRect.right > wallRect.left &&
		playerOldRect.top < wallRect.bottom &&
		playerOldRect.bottom > wallRect.top);

	if (wasOverlapping)
	{
		return result; // �̹� �����ִ� ���¶�� ���ο� �浹�� �ƴ�
	}

	// �¿� �浹 Ȯ�� (�÷��̾ �¿쿡�� ������ �̵�)
	if ((playerOldRect.right <= wallRect.left && playerNewRect.right > wallRect.left) ||
		(playerOldRect.left >= wallRect.right && playerNewRect.left < wallRect.right))
	{
		result.isColliding = true;
		result.groundActor = wallCollider->GetOwner();

		if (playerNewRect.right > wallRect.left && playerNewRect.left < wallRect.left)
		{
			// ���ʿ��� ���������� �̵��ϸ� �� �浹
			result.vHitNormal = Vector2(-1, 0);
			result.collisionPoint = Vector2(wallRect.left, (playerNewRect.top + playerNewRect.bottom) * 0.5f);
		}
		else
		{
			// �����ʿ��� �������� �̵��ϸ� �� �浹
			result.vHitNormal = Vector2(1, 0);
			result.collisionPoint = Vector2(wallRect.right, (playerNewRect.top + playerNewRect.bottom) * 0.5f);
		}
	}

	return result;
}

CollisionInfo CollisionManager::CheckLineWallCollision(Vector2 playerOldPos, Vector2 playerNewPos, float playerWidth, float playerHeight, Collider* wallCollider)
{
	CollisionInfo result = {};
	result.collisionLayer = wallCollider->GetCollisionLayer();

	Vector2 moveDir = playerNewPos - playerOldPos;
	if (moveDir.x == 0) return result;

	Vector2 lineStart = wallCollider->GetStartPoint();
	Vector2 lineEnd = wallCollider->GetEndPoint();
	Vector2 lineDir = lineEnd - lineStart;

	// ������ ���� ���� ��� (�¿� ����)
	//Vector2 lineNormal = Vector2(-lineDir.y, lineDir.x).GetNormalize();
	//if (lineNormal.x < 0) lineNormal *= -1; // �׻� �������� ���ϵ���

	float halfWidth = playerWidth * 0.5f;
	//float halfHeight = playerHeight * 0.5f;

	// �÷��̾��� �¿� �̵� ��ο� ���� ������ �˻�
	Vector2 moveStart;
	Vector2 moveEnd;

	if (moveDir.x > 0)
	{
		moveStart = Vector2(playerOldPos.x + halfWidth, playerOldPos.y);
		moveEnd = Vector2(playerNewPos.x + halfWidth, playerNewPos.y);
	}
	else if(moveDir.x < 0)
	{
		moveStart = Vector2(playerOldPos.x - halfWidth, playerOldPos.y);
		moveEnd = Vector2(playerNewPos.x - halfWidth, playerNewPos.y);
	}

	Vector2 intersection;
	float t;
	float cross;
	if (LineIntersectsLineSegment(moveStart, moveEnd, lineStart, lineEnd, intersection, t, cross))
	{
		result.isColliding = true;
		result.collisionPoint = intersection;
		result.vHitNormal = (moveDir.x > 0) ? Vector2(-1, 0) : Vector2(1, 0); // �̵� �ݴ� ����
		result.groundActor = wallCollider->GetOwner();
	}

	return result;
}

CollisionInfo CollisionManager::CheckLineStairCollision(Vector2 playerOldPos, Vector2 playerNewPos, float playerWidth, float playerHeight, Collider* stairCollider, bool wasStair)
{
	CollisionInfo result = {};
	result.collisionLayer = stairCollider->GetCollisionLayer();

	Vector2 lineStart = stairCollider->GetStartPoint();
	Vector2 lineEnd = stairCollider->GetEndPoint();
	Vector2 lineDir = lineEnd - lineStart;

	// ������ ���� ���� ���
	Vector2 lineNormal = Vector2(-lineDir.y, lineDir.x).GetNormalize();
	// ������ ������ ���ϵ��� ����
	if (lineNormal.y > 0) lineNormal *= -1;

	float halfWidth = playerWidth * 0.5f;
	float halfHeight = playerHeight * 0.5f;

	if (wasStair)
	{
		float yOnLine;
		bool inSeg;
		CollisionManager::GetInstance()->GetYOnLineAtX(lineStart, lineEnd, playerNewPos.x, yOnLine, inSeg);

		if (inSeg && fabs((playerNewPos.y + halfHeight) - yOnLine) <= 4.0f) // 4�ȼ� ���� ���
		{
			result.isColliding = true;
			result.collisionPoint = Vector2(playerNewPos.x, yOnLine);
			result.vHitNormal = Vector2(-lineDir.y, lineDir.x).GetNormalize();
			if (result.vHitNormal.y > 0) result.vHitNormal *= -1;
			result.groundActor = stairCollider->GetOwner();
			result.hitCorner = 2;
			return result;
		}

		//float halfWidth = playerWidth * 0.5f;
		//float minX = min(lineStart.x, lineEnd.x);
		//float maxX = max(lineStart.x, lineEnd.x);

		//// �÷��̾ �� ����� X ���� ���� �ִ��� Ȯ��
		//if (playerNewPos.x + halfWidth >= minX &&
		//	playerNewPos.x - halfWidth <= maxX)  // �ణ�� ���� ����
		//{
		//	// ��ܿ� ��� �پ�����
		//	result.isColliding = true;
		//	result.collisionPoint = Vector2(playerNewPos.x, 0);
		//	result.normal = Vector2(-lineDir.y, lineDir.x).GetNormalize();
		//	if (result.normal.y > 0) result.normal *= -1;
		//	result.groundActor = stairCollider->GetOwner();
		//	result.hitCorner = 2; // Ư���� ���� ǥ��
		//	return result;
		//}
	}

	// �÷��̾��� �ٴ� �𼭸���
	Vector2 oldBottomLeft = Vector2(playerOldPos.x - halfWidth, playerOldPos.y + halfHeight);
	Vector2 newBottomLeft = Vector2(playerNewPos.x - halfWidth, playerNewPos.y + halfHeight);
	Vector2 oldBottomRight = Vector2(playerOldPos.x + halfWidth, playerOldPos.y + halfHeight);
	Vector2 newBottomRight = Vector2(playerNewPos.x + halfWidth, playerNewPos.y + halfHeight);

	Vector2 closestIntersection;
	int32 hitCorner = 0;
	bool foundIntersection = false;

	Vector2 intersection;
	float t;
	float cross;
	// ����
	oldBottomLeft.y -= 1.f;
	if (LineIntersectsLineSegment(oldBottomLeft, newBottomLeft, lineStart, lineEnd, intersection, t, cross))
	{
		if (t >= 0.0f && t <= 1.0f)
		{
			closestIntersection = intersection;
			hitCorner = 1;
			foundIntersection = true;
		}
	}
	// ������
	oldBottomRight.y -= 1.f;
	if (LineIntersectsLineSegment(oldBottomRight, newBottomRight, lineStart, lineEnd, intersection, t, cross))
	{
		if (t >= 0.0f && t <= 1.0f)
		{
			closestIntersection = intersection;
			hitCorner = 2;
			foundIntersection = true;
		}
	}

	if (!foundIntersection) return result;

	result.isColliding = true;
	result.collisionPoint = closestIntersection;
	result.vHitNormal = lineNormal;
	result.groundActor = stairCollider->GetOwner();
	result.hitCorner = hitCorner;

	return result;
}

CollisionInfo CollisionManager::CheckLineStiarCollision2(Vector2 playerOldPos, Vector2 playerNewPos, float playerWidth, float playerHeight, Collider* stairCollider, bool wasStair)
{
	CollisionInfo result = {};
	result.collisionLayer = stairCollider->GetCollisionLayer();

	Vector2 stairStart = stairCollider->GetStartPoint();
	Vector2 stairEnd = stairCollider->GetEndPoint();
	Vector2 stairDir = stairEnd - stairStart;

	float halfW = playerWidth * 0.5f;
	float halfH = playerHeight * 0.5f;

	// ��� ���� �Ǵ� (45�� ����)
	bool isRightUpStair = (stairDir.x > 0 && stairDir.y < 0) || (stairDir.x < 0 && stairDir.y > 0);

	// �̹� ��ܿ� �ִ� ����� ó��
	if (wasStair)
	{
		Vector2 targetCorner;
		if (isRightUpStair)
		{
			targetCorner = Vector2(playerNewPos.x + halfW, playerNewPos.y + halfH);
		}
		else
		{
			targetCorner = Vector2(playerNewPos.x - halfW, playerNewPos.y + halfH);
		}

		float yOnLine;
		bool inSeg;
		GetYOnLineAtX(stairStart, stairEnd, targetCorner.x, yOnLine, inSeg);

		//if (inSeg && fabs(targetCorner.y - yOnLine) <= 5.0f) // 5�ȼ� ��� ����
		//{
		//	result.isColliding = true;
		//	result.collisionPoint = Vector2(targetCorner.x, yOnLine);
		//	result.vHitNormal = Vector2(-stairDir.y, stairDir.x).GetNormalize();
		//	if (result.vHitNormal.y > 0) result.vHitNormal *= -1;
		//	result.groundActor = stairCollider->GetOwner();
		//	result.hitCorner = isRightUpStair ? 2 : 1; // 2=������, 1=����
		//	return result;
		//}

		if (inSeg)
		{
			// ��� ���κ� üũ - ��� ������ 90%�� ������ �浹���� ����
			Vector2 stairDirection = stairDir.GetNormalize();
			Vector2 stairStartToCorner = targetCorner - stairStart;
			float distanceAlongStair = stairStartToCorner.Dot(stairDirection);
			float stairLength = stairDir.Length();

			if (distanceAlongStair > stairLength * 0.9f)
			{
				return result; // ��� ���κп����� �浹���� ����
			}

			if (fabs(targetCorner.y - yOnLine) <= 5.0f) // 5�ȼ� ��� ����
			{
				result.isColliding = true;
				result.collisionPoint = Vector2(targetCorner.x, yOnLine);
				result.vHitNormal = Vector2(-stairDir.y, stairDir.x).GetNormalize();
				if (result.vHitNormal.y > 0) result.vHitNormal *= -1;
				result.groundActor = stairCollider->GetOwner();
				result.hitCorner = isRightUpStair ? 2 : 1; // 2=������, 1=����
				return result;
			}
		}

		// ��� ������ �ణ ����� ��ó�� ������ ��� ����
		float stairMinX = min(stairStart.x, stairEnd.x);
		float stairMaxX = max(stairStart.x, stairEnd.x);
		float tolerance = 10.0f; // 10�ȼ� ����

		if (targetCorner.x >= stairMinX - tolerance && targetCorner.x <= stairMaxX + tolerance)
		{
			// ��� ���弱���� Y�� ���
			float extendedY;
			GetYOnLineAtX(stairStart, stairEnd, targetCorner.x, extendedY, inSeg);

			if (fabs(targetCorner.y - extendedY) <= 15.0f)
			{
				result.isColliding = true;
				result.collisionPoint = Vector2(targetCorner.x, extendedY);
				result.vHitNormal = Vector2(-stairDir.y, stairDir.x).GetNormalize();
				if (result.vHitNormal.y > 0) result.vHitNormal *= -1;
				result.groundActor = stairCollider->GetOwner();
				result.hitCorner = isRightUpStair ? 2 : 1;
				return result;
			}
		}
		// ��� ������ ������� �浹 ����
		return result;
	}

	// ���ο� ��� �浹 �˻�
	Vector2 moveDir = playerNewPos - playerOldPos;

	// �÷��̾ �Ʒ��� �̵��ϰų� ���� �̵��� ���� ��� �浹 �˻�
	if (moveDir.y < -1.f) // ���� ���� ���̸� ��� ����
		return result;

	// ���� �𼭸� ��� �˻��ϵ�, ��� ���⿡ ���� �켱���� ����
	Vector2 bottomLeft = Vector2(playerNewPos.x - halfW, playerNewPos.y + halfH);
	Vector2 bottomRight = Vector2(playerNewPos.x + halfW, playerNewPos.y + halfH);

	Vector2 oldBottomLeft = Vector2(playerOldPos.x - halfW, playerOldPos.y + halfH);
	Vector2 oldBottomRight = Vector2(playerOldPos.x + halfW, playerOldPos.y + halfH);

	// ��ܰ��� ������ �˻�
	Vector2 intersection;
	float t;
	float cross;
	bool foundCollision = false;
	int hitCorner = 0;
	float bestT = FLT_MAX;
	Vector2 bestIntersection;

	// ����� ���: ������ �𼭸� �켱
	oldBottomRight.y -= 2.f;
	bottomRight.y -= 2.f;
	for (int32 i = 0; i < 3; ++i)
	{
		oldBottomRight.y += 1.f;
		bottomRight.y += 1.f;
		if (LineIntersectsLineSegment(oldBottomRight, bottomRight, stairStart, stairEnd, intersection, t, cross))
		{
			if (t >= 0.0f && t <= 1.0f && t < bestT)
			{
				// ��� ���κ� üũ
				Vector2 stairDirection = stairDir.GetNormalize();
				Vector2 stairStartToIntersection = intersection - stairStart;
				float distanceAlongStair = stairStartToIntersection.Dot(stairDirection);
				float stairLength = stairDir.Length();

				// ��� ���κ�(90% ����)�� �ƴϸ� �浹 ó��
				if (distanceAlongStair <= stairLength * 0.9f)
				{
					bestT = t;
					bestIntersection = intersection;
					foundCollision = true;
					hitCorner = 2;
				}

				//bestT = t;
				//bestIntersection = intersection;
				//foundCollision = true;
				//hitCorner = 2;
			}
		}
	}
	
	// �����ʿ��� �浹�� ������ ���ʵ� �˻�
	oldBottomLeft.y -= 2.f;
	bottomLeft.y -= 2.f;
	for (int32 i = 0; i < 3; ++i)
	{
		oldBottomLeft.y += 1.f;
		bottomLeft.y += 1.f;
		if (LineIntersectsLineSegment(oldBottomLeft, bottomLeft, stairStart, stairEnd, intersection, t, cross))
		{
			if (t >= 0.0f && t <= 1.0f && t < bestT)
			{
				// ��� ���κ� üũ
				Vector2 stairDirection = stairDir.GetNormalize();
				Vector2 stairStartToIntersection = intersection - stairStart;
				float distanceAlongStair = stairStartToIntersection.Dot(stairDirection);
				float stairLength = stairDir.Length();

				// ��� ���κ�(90% ����)�� �ƴϸ� �浹 ó��
				if (distanceAlongStair <= stairLength * 0.9f)
				{
					bestT = t;
					bestIntersection = intersection;
					foundCollision = true;
					hitCorner = 1;
				}

				//bestT = t;
				//bestIntersection = intersection;
				//foundCollision = true;
				//hitCorner = 1;
			}
		}
	}

	if (!foundCollision)
		return result;

	// �÷��̾ ��� ������ �����ϴ��� Ȯ�� (�Ʒ����� ���� �ö󰡴� �� ����)
	Vector2 stairNormal = Vector2(-stairDir.y, stairDir.x).GetNormalize();
	if (stairNormal.y > 0) stairNormal *= -1;

	// �̵� ������ ��� ������ �ݴ� �������� Ȯ�� (������ �Ʒ���)
	if (moveDir.y < -5.f)
		return result;

	//// �̵� ������ ��� ������ �ݴ� �������� Ȯ�� (������ �Ʒ���)
	//if (moveDir.Dot(stairNormal) > 0.1f)
	//	return result;

	// ��� ����
	result.isColliding = true;
	result.collisionPoint = bestIntersection;
	result.vHitNormal = stairNormal;
	result.groundActor = stairCollider->GetOwner();
	result.hitCorner = hitCorner;

	return result;
}

bool CollisionManager::LineIntersectsLineSegment(Vector2 p1, Vector2 p2, Vector2 q1, Vector2 q2, Vector2& intersection, float& t, float& crossOut)
{
	Vector2 dir1 = p2 - p1;
	Vector2 dir2 = q2 - q1;

	float cross = dir1.x * dir2.y - dir1.y * dir2.x;
	crossOut = cross;
	if (abs(cross) < 1e-6f) return false; // ����

	Vector2 diff = q1 - p1;
	float t1 = (diff.x * dir2.y - diff.y * dir2.x) / cross;
	float t2 = (diff.x * dir1.y - diff.y * dir1.x) / cross;

	if (t1 >= 0.0f && t1 <= 1.0f && t2 >= 0.0f && t2 <= 1.0f)
	{
		intersection = p1 + dir1 * t1;
		t = t1;
		return true;
	}

	return false;
}

bool CollisionManager::LineIntersectsWallSegment(Vector2 p1, Vector2 p2, Vector2 q1, Vector2 q2, Vector2& intersection, float& t)
{
	Vector2 dir1 = p2 - p1;
	Vector2 dir2 = q2 - q1;

	// ���� B�� ������ q1�� ���� A�� ������ p1�� ���ϴ��� �˻�
	float dot1 = dir1.x * (q1.x - p1.x) + dir1.y * (q1.y - p1.y);

	// ���� A�� ������ p1�� ���� B�� ������ q1�� ���ϴ��� �˻�
	float dot2 = dir2.x * (p1.x - q1.x) + dir2.y * (p1.y - q1.y);

	// �� �� �ϳ��� �����̶� �ٸ� �������� ���ϰ� ���� ���� ������ ����
	if (dot1 < 0.0f || dot2 < 0.0f)
	{
		float cross = dir1.x * dir2.y - dir1.y * dir2.x;
		//if (abs(cross) < 1e-6f) return false; // ����

		Vector2 diff = q1 - p1;
		float t1 = (diff.x * dir2.y - diff.y * dir2.x) / cross;
		float t2 = (diff.x * dir1.y - diff.y * dir1.x) / cross;

		if (t1 >= 0.0f && t1 <= 1.0f && t2 >= 0.0f && t2 <= 1.0f)
		{
			intersection = p1 + dir1 * t1;
			t = t1;
			return true;
		}
	}

	return false;
}

Vector2 CollisionManager::GetClosestPointOnLineSegment(Vector2 point, Vector2 lineStart, Vector2 lineEnd)
{
	Vector2 lineVec = lineEnd - lineStart;
	Vector2 pointVec = point - lineStart;

	float lineLength = lineVec.Length();
	if (lineLength < 1e-6f) return lineStart;

	float t = pointVec.Dot(lineVec) / (lineLength * lineLength);
	t = max(0.0f, min(1.0f, t)); // 0�� 1 ���̷� Ŭ����

	return lineStart + lineVec * t;
}

bool CollisionManager::GetYOnLineAtX(const Vector2& a, const Vector2& b, float x, float& outY, bool& outInSegment)
{
	const float EPS = 1e-6f;
	float dx = b.x - a.x;
	if (fabs(dx) < EPS)
	{
		// ������(����� 45���� �����ϹǷ� ���� �߻����� ����) -> x�� ���� ������ y ������ �߰��� ��ȯ
		outInSegment = (fabs(x - a.x) < 1.0f); // ������ �����ϰ� ó��
		outY = (a.y + b.y) * 0.5f;
		return outInSegment;
	}
	float t = (x - a.x) / dx;
	outInSegment = (t >= 0.0f && t <= 1.0f);
	outY = a.y + (b.y - a.y) * t;
	return outInSegment;
}
