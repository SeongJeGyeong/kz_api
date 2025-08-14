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
	// 충돌을 무시할 객체들 세팅
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
	//PLAYER	0
	//ENEMY		1
	//GROUND	2
	//PLATFORM	3
	//WALL		4
	//CEILING	5
	//STAIR		6
	for (int32 receiveLayer = 0; receiveLayer < ECollisionLayer::END; ++receiveLayer)
	{
		for (auto receiveCollider : _colliderList[(ECollisionLayer)receiveLayer])
		{
			for (int32 sendLayer = 0; sendLayer < ECollisionLayer::END; ++sendLayer)
			{
				if (COLLISION_BIT_MASK_IGNORE[receiveLayer] & (uint8)(1 << sendLayer)) continue;

				for (auto sendCollider : _colliderList[(ECollisionLayer)sendLayer])
				{
					// 처음 충돌에 들어옴(콜리전 맵에도 없음)
					bool first = true;

					CollisionMapID mapId = CollisionMapID(receiveCollider->GetColliderId(), sendCollider->GetColliderId());

					auto iter = _collisionMap.find(mapId.ToUInt64());

					if (_collisionMap.end() != iter)
					{
						// 이미 충돌 검사 완료했으면 넘긴다.
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
					info.isColliding = false;
					if (CheckCollision(receiveCollider, sendCollider, info))
					{
						if (first)
						{
							//sendCollider->GetOwner()->OnCollisionBeginOverlap(receiveCollider);
							receiveCollider->GetOwner()->OnCollisionBeginOverlap(info);
							_collisionMap.emplace(mapId.ToUInt64(), true);
							sendCollider->SetOverlapped(true);
						}
						else
						{
							//sendCollider->GetOwner()->OnCollisionStayOverlap(receiveCollider);
							receiveCollider->GetOwner()->OnCollisionStayOverlap(info);
							_collisionMap[iter->first] = true;
						}
					}
					else
					{
						if (_collisionMap.end() != iter)
						{
							//sendCollider->GetOwner()->OnCollisionEndOverlap(receiveCollider);
							receiveCollider->GetOwner()->OnCollisionEndOverlap(info);
							_collisionMap.erase(iter);
							sendCollider->SetOverlapped(false);
						}
					}
				}
			}
		}
	}

}

void CollisionManager::Render(HDC hdc)
{
	wstring str = std::format(L"collisionMap Size ({0})", _collisionMap.size());
	::TextOut(hdc, 200, 200, str.c_str(), static_cast<int32>(str.size()));
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
			// 설정
			COLLISION_BIT_MASK_IGNORE[(int8)layer1] |= ((uint8)1 << (int8)layer2);
			COLLISION_BIT_MASK_IGNORE[(int8)layer2] |= ((uint8)1 << (int8)layer1);
		}
		else
		{
			// 해제
			COLLISION_BIT_MASK_IGNORE[(int8)layer1] &= ~((uint8)1 << (int8)layer2);
			COLLISION_BIT_MASK_IGNORE[(int8)layer2] &= ~((uint8)1 << (int8)layer1);
		}
		break;
	case C_BLOCK:
		if (on)
		{
			// 설정
			COLLISION_BIT_MASK_BLOCK[(int8)layer1] |= ((uint8)1 << (int8)layer2);
			COLLISION_BIT_MASK_BLOCK[(int8)layer2] |= ((uint8)1 << (int8)layer1);
		}
		else
		{
			// 해제
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

		// 플레이어 AABB 계산
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
			info = CheckLineStairCollision(player->GetPos(), player->GetNewPos(), playerWidth, playerHeight, send, player->GetOnStair());
			break;
		case ECollisionLayer::PLATFORM :
			//if (player->GetIsPlatform()) break;
			info = CheckLinePlatformCollision(player->GetPos(), player->GetNewPos(), playerWidth, playerHeight, send);
			break;
		default:
			break;
		}

		if (info.isColliding) return true;
	}
	//else
	//{
	//	EColliderType receiveType = receive->GetColliderType();
	//	EColliderType sendType = send->GetColliderType();

	//	if (send->GetCollisionLayer() == ECollisionLayer::STAIR)
	//	{
	//		int i = 1;
	//	}

	//	if (receiveType == EColliderType::OBB && receiveType == sendType)
	//	{
	//		//return CheckBetweenOBB(static_cast<OBBCollider*>(receive), static_cast<OBBCollider*>(send), info);
	//	}
	//	else if (receiveType == EColliderType::AABB && receiveType == sendType)
	//	{
	//		OutputDebugString(L"CheckBetweenAABB\n");
	//		//return CheckBetweenAABB(static_cast<AABBCollider*>(receive), static_cast<AABBCollider*>(send), info);
	//	}
	//	else if (receiveType == EColliderType::AABB && sendType == EColliderType::LINE)
	//	{
	//		//return CheckAABBtoLine(static_cast<AABBCollider*>(receive), static_cast<LineCollider*>(send), info);
	//	}
	//}

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
	
	// MTV(최소이동벡터) : 두 도형이 겹쳤을 때, 두 도형이 서로 겹치지 않도록 한 도형을 이동시키는 벡터 중 가장 작은 벡터
	// SAT에서 MTV 찾는 법 : 각각의 투영영역을 비교했을 때, 투영영역끼리 겹치는 부분이 제일 작은 것이 MTV
	// MTV의 방향은 +-로 결정한다. 왼쪽 도형을 왼쪽으로 밀어내고 싶을 때는 -, 오른쪽 도형을 오른쪽으로 밀어내고 싶을 때는 +
	receive->SetHitMTV(minAxis * minOverlap);

	// 모든 축에서 겹치므로 충돌
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

	// MTV(최소이동벡터) : 두 도형이 겹쳤을 때, 두 도형이 서로 겹치지 않도록 한 도형을 이동시키는 벡터 중 가장 작은 벡터
	// SAT에서 MTV 찾는 법 : 각각의 투영영역을 비교했을 때, 투영영역끼리 겹치는 부분이 제일 작은 것이 MTV
	// MTV의 방향은 +-로 결정한다. 왼쪽 도형을 왼쪽으로 밀어내고 싶을 때는 -, 오른쪽 도형을 오른쪽으로 밀어내고 싶을 때는 +
	receive->SetHitMTV(minAxis * minOverlap);

	// 모든 축에서 겹치므로 충돌
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

	// 충돌체끼리 중심을 잇는 벡터
	double nx = sendCenter.x - receiveCenter.x;
	double ny = sendCenter.y - receiveCenter.y;

	// 충돌체의 길이, 높이의 절반
	Vector2 receiveExtent = (receiveMax - receiveMin) * 0.5f;
	Vector2 sendExtent = (sendMax - sendMin) * 0.5f;

	// n.x가 1이거나 -1이면 x축이 충돌
	double overlapX = receiveExtent.x + sendExtent.x - abs(nx);
	// n.y가 1이거나 -1이면 y축이 충돌
	double overlapY = receiveExtent.y + sendExtent.y - abs(ny);

	if (overlapX < 0 || overlapY < 0) return false;

	Vector2 normal;
	Vector2 hitPos;

	// 위, 아래 충돌
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
		// 사각형끼리 겹치는 위치는 2군데
		int32 contactCount = 2;
		Vector2 contactPoint1 = Vector2(max(receiveMin.x, sendMin.x), (ny < 0 ? receiveMin.y : receiveMax.y));
		Vector2 contactPoint2 = Vector2(max(receiveMax.x, sendMax.x), contactPoint1.y);
		hitPos = { (contactPoint1.x + contactPoint2.x) * 0.5f, (contactPoint1.y + contactPoint2.y) * 0.5f };
	}
	// 왼쪽, 오른쪽 충돌
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

	// start 지점에서 충돌
	float tMin = 0.0f;
	// end 지점에서 충돌
	float tMax = lineLength;
	//float tMax = 1.0f;

	// x축 검사
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
			return false; // 충돌 안 함
	}
	else if (start.x < rect.left || start.x > rect.right)
	{
		return false; // Y축으로 평행하고 밖에 있음
	}

	// y축 검사
	if (dir.y != 0)
	{
		float ty1 = (rect.top - start.y) / dir.y;
		float ty2 = (rect.bottom - start.y) / dir.y;

		// 시작 지점으로부터 떨어져있는 거리 비율
		// ty1 가 ty2보다 작다면, 위에서 아래 방향이다.
		Vector2 ny = (ty1 < ty2) ? Vector2(0, -1) : Vector2(0, 1);

		if (ty1 > ty2) std::swap(ty1, ty2);

		if (ty1 > tMin)
		{
			info.vHitNormal = ny;
		}

		tMin = max(tMin, ty1);
		tMax = min(tMax, ty2);

		if (tMin > tMax)
			return false; // 충돌 안 함
	}
	else if (start.y < rect.top || start.y > rect.bottom)
	{
		return false;
	}

	// 선분이 사각형 내부에 있는 경우 예외처리
	// 선분의 진행방향에 따라서 축 기준으로 조금더 기울어진 방향으로 normal 을 보정해준다.
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

	// 충돌 결과 저장
	info.collisionPoint = start + dir * tMin;

	if (tMax <= lineLength) {
		Vector2 exitPoint = start + dir * tMax;
		info.collisionPoint = (info.collisionPoint + exitPoint) * 0.5f;
	}

	// 예외처리 하나더.
	info.collisionPoint += info.vHitNormal * 0.01f; // 약간의 오프셋을 주어 정확한 충돌 위치를 보정

	// 충돌 됨.
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
		return false; // 분리축 발견
	}

	return true;
}

Vector2 CollisionManager::CalculateHitPos(OBBCollider* receive, OBBCollider* send, Vector2 normal)
{
	std::vector<Vector2> vertices1 = receive->GetVertices();
	std::vector<Vector2> vertices2 = send->GetVertices();

	// 법선 방향으로 가장 가까운 꼭짓점들 찾기
	float maxDist1 = -INFINITY;
	float maxDist2 = -INFINITY;
	Vector2 closestVertex1, closestVertex2;

	// obb1에서 법선 방향으로 가장 먼 점
	for (auto& vertex : vertices1) {
		float dist = vertex.Dot(normal);
		if (dist > maxDist1) {
			maxDist1 = dist;
			closestVertex1 = vertex;
		}
	}

	// obb2에서 법선 반대 방향으로 가장 먼 점
	Vector2 invNormal = { -normal.x,-normal.y };
	for (auto& vertex : vertices2) {
		float dist = vertex.Dot(invNormal);
		if (dist > maxDist2) {
			maxDist2 = dist;
			closestVertex2 = vertex;
		}
	}

	// 두 점의 중점을 접촉점으로 사용
	return (closestVertex1 + closestVertex2) * 0.5f;
}

void CollisionManager::ExecuteCollisionFunc(Collider* receive, Collider* send)
{
	//send->GetCollider()->
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

	// 현재 위치에서 겹치는지 확인
	// 위에서 아래는 같은 높이기만 해도 겹치는 걸로 판단.
	if (!(playerNewRect.left < groundRect.right &&
		playerNewRect.right > groundRect.left &&
		playerNewRect.top < groundRect.bottom &&
		playerNewRect.bottom >= groundRect.top))
	{
		return result;
	}

	// 어느 방향에서 충돌했는지 확인
	bool fromTop = (playerOldRect.bottom <= groundRect.top && playerNewRect.bottom >= groundRect.top);		// 위에서 아래
	bool fromBottom = (playerOldRect.top >= groundRect.bottom && playerNewRect.top < groundRect.bottom);	// 아래에서 위로
	bool fromLeft = (playerOldRect.right <= groundRect.left && playerNewRect.right > groundRect.left);	// 왼쪽에서 오른쪽으로
	bool fromRight = (playerOldRect.left >= groundRect.right && playerNewRect.left < groundRect.right);	// 오른쪽에서 왼쪽으로

	// 여러 방향에서 동시에 충돌한 경우, 침투 깊이가 가장 작은 방향을 선택
	float topPenetration = fromTop ? (playerNewRect.bottom - groundRect.top) : FLT_MAX;
	float bottomPenetration = fromBottom ? (groundRect.bottom - playerNewRect.top) : FLT_MAX;
	float leftPenetration = fromLeft ? (playerNewRect.right - groundRect.left) : FLT_MAX;
	float rightPenetration = fromRight ? (groundRect.right - playerNewRect.left) : FLT_MAX;

	float minPenetration = min(topPenetration, bottomPenetration);
	minPenetration = min(minPenetration, leftPenetration);
	minPenetration = min(minPenetration, rightPenetration);

	if (minPenetration == FLT_MAX)
		return result; // 충돌하지 않음

	result.isColliding = true;
	result.penetrationDepth = minPenetration;
	result.groundActor = groundCollider->GetOwner();

	// 가장 작은 침투 깊이에 따라 충돌 방향 결정
	if (minPenetration == topPenetration) // 위에서 아래로 (바닥)
	{
		result.collisionPoint = Vector2(
			(max(playerNewRect.left, groundRect.left) + min(playerNewRect.right, groundRect.right)) * 0.5f,
			groundRect.top
		);
		result.vHitNormal = Vector2(0, -1); // 위쪽 방향
		result.hitCorner = 0; // 바닥 충돌
	}
	else if (minPenetration == bottomPenetration) // 아래에서 위로 (천장)
	{
		result.collisionPoint = Vector2(
			(max(playerNewRect.left, groundRect.left) + min(playerNewRect.right, groundRect.right)) * 0.5f,
			groundRect.bottom
		);
		result.vHitNormal = Vector2(0, 1); // 아래쪽 방향
		result.hitCorner = 3; // 천장 충돌
	}
	else if (minPenetration == leftPenetration) // 왼쪽에서 오른쪽으로 (오른쪽 벽)
	{
		result.collisionPoint = Vector2(
			groundRect.left,
			(max(playerNewRect.top, groundRect.top) + min(playerNewRect.bottom, groundRect.bottom)) * 0.5f
		);
		result.vHitNormal = Vector2(-1, 0); // 왼쪽 방향
		result.hitCorner = 1; // 왼쪽 벽 충돌
	}
	else if (minPenetration == rightPenetration) // 오른쪽에서 왼쪽으로 (왼쪽 벽)
	{
		result.collisionPoint = Vector2(
			groundRect.right,
			(max(playerNewRect.top, groundRect.top) + min(playerNewRect.bottom, groundRect.bottom)) * 0.5f
		);
		result.vHitNormal = Vector2(1, 0); // 오른쪽 방향
		result.hitCorner = 2; // 오른쪽 벽 충돌
	}

	return result;
}

CollisionInfo CollisionManager::CheckLinePlatformCollision(Vector2 playerOldPos, Vector2 playerNewPos, float playerWidth, float playerHeight, Collider* lineCollider)
{
	CollisionInfo result = {};
	result.collisionLayer = lineCollider->GetCollisionLayer();

	Vector2 moveDir = playerNewPos - playerOldPos;
	if (moveDir.y < 0) return result; // 아래로 이동할 때만

	Vector2 lineStart = lineCollider->GetStartPoint();
	Vector2 lineEnd = lineCollider->GetEndPoint();

	float halfWidth = playerWidth * 0.5f;
	float halfHeight = playerHeight * 0.5f;

	// 현재 플레이어 바닥 Y좌표
	float playerBottomY = playerNewPos.y + halfHeight;
	float playerOldBottomY = playerOldPos.y + halfHeight;

	// 플레이어 X 범위에서 플랫폼의 Y값 계산
	float playerLeftX = playerNewPos.x - halfWidth;
	float playerRightX = playerNewPos.x + halfWidth;

	float outY1, outY2;

	bool inSegment1 = GetYOnLineAtX(lineStart, lineEnd, playerLeftX, outY1);
	bool inSegment2 = GetYOnLineAtX(lineStart, lineEnd, playerRightX, outY2);

	if (!inSegment1 && !inSegment2) return result;

	// 플랫폼의 최고점 찾기
	float platformTopY = (inSegment1 && inSegment2) ? min(outY1, outY2) :
		(inSegment1 ? outY1 : outY2);

	// 이전에는 위에 있었고, 현재는 플랫폼에 닿았거나 관통했는지 확인
	bool wasAbove = playerOldBottomY <= platformTopY;
	bool isOnOrBelow = playerBottomY >= platformTopY - 2.0f; // 2픽셀 허용

	if (wasAbove && isOnOrBelow)
	{
		result.isColliding = true;
		result.collisionPoint = Vector2(playerNewPos.x, platformTopY);

		Vector2 lineDir = lineEnd - lineStart;
		result.vHitNormal = Vector2(-lineDir.y, lineDir.x).GetNormalize();
		if (result.vHitNormal.y > 0) result.vHitNormal *= -1;

		result.groundActor = lineCollider->GetOwner();
		result.penetrationDepth = max(0.0f, playerBottomY - platformTopY);
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

	// 플레이어 AABB 계산
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

	// GROUND, WALL, STAIR, PLATFORM 모든 레이어 검사
	for (int32 layer = ECollisionLayer::GROUND; layer <= ECollisionLayer::STAIR; ++layer)
	{
		for (auto collider : _colliderList[layer])
		{
			CollisionInfo result;

			if (collider->GetColliderType() == EColliderType::AABB)
			{
				// AABB 충돌 검사
				if (layer == ECollisionLayer::GROUND)
				{
					result = CheckAABBGroundCollision(playerOldRect, playerNewRect, collider);
				}
			}
			else if (collider->GetColliderType() == EColliderType::LINE)
			{
				// Line 충돌 검사
				if (layer == ECollisionLayer::PLATFORM)
				{
					result = CheckLinePlatformCollision(playerOldPos, playerNewPos, playerWidth, playerHeight, collider);
				}
				else if (layer == ECollisionLayer::WALL)
				{
					result = CheckLineWallCollision(playerOldPos, playerNewPos, playerWidth, playerHeight, collider);
				}
				else if (layer == ECollisionLayer::STAIR)
				{
					result = CheckLineStairCollision(playerOldPos, playerNewPos, playerWidth, playerHeight, collider, player->GetOnStair());
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

CollisionInfo CollisionManager::CheckLineWallCollision(Vector2 playerOldPos, Vector2 playerNewPos, float playerWidth, float playerHeight, Collider* wallCollider)
{
	CollisionInfo result = {};
	result.collisionLayer = wallCollider->GetCollisionLayer();

	Vector2 moveDir = playerNewPos - playerOldPos;
	//if (moveDir.x == 0) return result;

	Vector2 lineStart = wallCollider->GetStartPoint();
	Vector2 lineEnd = wallCollider->GetEndPoint();
	Vector2 lineDir = lineEnd - lineStart;

	float halfWidth = playerWidth * 0.5f;

	// 플레이어의 좌우 이동 경로와 벽의 교차점 검사
	Vector2 moveStart;
	Vector2 moveEnd;

	Vector2 intersection;
	float t;
	float cross;

	if (moveDir.x > 0)
	{
		moveStart = Vector2(playerOldPos.x + halfWidth, playerOldPos.y);
		moveEnd = Vector2(playerNewPos.x + halfWidth, playerNewPos.y);
		if (LineIntersectsLineSegment(moveStart, moveEnd, lineStart, lineEnd, intersection, t, cross))
		{
			result.isColliding = true;
			result.collisionPoint = intersection;
			result.vHitNormal = Vector2(-1, 0); // 이동 반대 방향
			result.groundActor = wallCollider->GetOwner();
		}
	}
	else if(moveDir.x < 0)
	{
		moveStart = Vector2(playerOldPos.x - halfWidth, playerOldPos.y);
		moveEnd = Vector2(playerNewPos.x - halfWidth, playerNewPos.y);
		if (LineIntersectsLineSegment(moveStart, moveEnd, lineStart, lineEnd, intersection, t, cross))
		{
			result.isColliding = true;
			result.collisionPoint = intersection;
			result.vHitNormal = Vector2(1, 0);
			result.groundActor = wallCollider->GetOwner();
		}
	}
	else
	{
		// 오른쪽
		moveStart = Vector2(playerOldPos.x + halfWidth, playerOldPos.y);
		moveEnd = Vector2(playerNewPos.x + halfWidth + 1.f, playerNewPos.y);
		if (LineIntersectsLineSegment(moveStart, moveEnd, lineStart, lineEnd, intersection, t, cross))
		{
			result.isColliding = true;
			result.collisionPoint = intersection;
			result.vHitNormal = Vector2(-1, 0);
			result.groundActor = wallCollider->GetOwner();
		}
		// 왼쪽
		moveStart = Vector2(playerOldPos.x - halfWidth, playerOldPos.y);
		moveEnd = Vector2(playerNewPos.x - halfWidth - 1.f, playerNewPos.y);
		if (LineIntersectsLineSegment(moveStart, moveEnd, lineStart, lineEnd, intersection, t, cross))
		{
			result.isColliding = true;
			result.collisionPoint = intersection;
			result.vHitNormal = Vector2(1, 0);
			result.groundActor = wallCollider->GetOwner();
		}
	}

	return result;
}

CollisionInfo CollisionManager::CheckLineWallCollision2(Vector2 playerOldPos, Vector2 playerNewPos, float playerWidth, float playerHeight, Collider* wallCollider)
{
	CollisionInfo result = {};
	result.collisionLayer = wallCollider->GetCollisionLayer();

	Vector2 lineStart = wallCollider->GetStartPoint();
	Vector2 lineEnd = wallCollider->GetEndPoint();

	float halfWidth = playerWidth * 0.5f;
	float halfHeight = playerHeight * 0.5f;

	// 플레이어 Y 범위에서 벽의 X값 계산
	float playerTopY = playerNewPos.y - halfHeight;
	float playerBottomY = playerNewPos.y + halfHeight;
	float playerOldTopY = playerOldPos.y - halfHeight;
	float playerOldBottomY = playerOldPos.y + halfHeight;

	float wallX1, wallX2;
	bool inSegment1 = GetXOnLineAtY(lineStart, lineEnd, playerTopY, wallX1);
	bool inSegment2 = GetXOnLineAtY(lineStart, lineEnd, playerBottomY, wallX2);

	if (!inSegment1 && !inSegment2) return result;

	// 벽의 X 위치 결정
	float wallX = (inSegment1 && inSegment2) ?
		((wallX1 + wallX2) * 0.5f) : (inSegment1 ? wallX1 : wallX2);

	// 현재 플레이어 좌우 X좌표
	float playerLeftX = playerNewPos.x - halfWidth;
	float playerRightX = playerNewPos.x + halfWidth;
	float playerOldLeftX = playerOldPos.x - halfWidth;
	float playerOldRightX = playerOldPos.x + halfWidth;

	const float tolerance = 2.0f; // 허용 오차

	// 왼쪽 벽 체크 (플레이어가 벽의 오른쪽에서 접근)
	bool wasRightOfWall = playerOldLeftX >= wallX;
	bool isNearOrLeftOfWall = playerLeftX <= wallX + tolerance;

	if (wasRightOfWall && isNearOrLeftOfWall)
	{
		result.isColliding = true;
		result.collisionPoint = Vector2(wallX, playerNewPos.y);
		result.vHitNormal = Vector2(1, 0); // 오른쪽을 향함
		result.groundActor = wallCollider->GetOwner();
		return result;
	}

	// 오른쪽 벽 체크 (플레이어가 벽의 왼쪽에서 접근)
	bool wasLeftOfWall = playerOldRightX <= wallX;
	bool isNearOrRightOfWall = playerRightX >= wallX - tolerance;

	if (wasLeftOfWall && isNearOrRightOfWall)
	{
		result.isColliding = true;
		result.collisionPoint = Vector2(wallX, playerNewPos.y);
		result.vHitNormal = Vector2(-1, 0); // 왼쪽을 향함
		result.groundActor = wallCollider->GetOwner();
		return result;
	}

	return result;




	//// 현재 플레이어 왼쪽, 오른쪽 x좌표
	//float playerLeftX = playerNewPos.x - halfWidth;
	//float playerOldLeftX = playerOldPos.x - halfWidth;
	//float playerRightX = playerNewPos.x + halfWidth;
	//float playerOldRightX = playerOldPos.x + halfWidth;

	//// 플레이어 Y 범위에서 벽의 X값 계산 (GetXOnLineAtY 함수 필요)
	//float playerTopY = playerNewPos.y - halfHeight;
	//float playerBottomY = playerNewPos.y + halfHeight;

	//float wallX1, wallX2;

	//bool inSegment1 = GetXOnLineAtY(lineStart, lineEnd, playerTopY, wallX1);
	//bool inSegment2 = GetXOnLineAtY(lineStart, lineEnd, playerBottomY, wallX2);

	//if (!inSegment1 && !inSegment2) return result;

	//// 벽의 가장 왼쪽/오른쪽 찾기 (벽의 방향에 따라)
	//float wallLeftmostX, wallRightmostX;
	//if (inSegment1 && inSegment2) {
	//	wallLeftmostX = min(wallX1, wallX2);
	//	wallRightmostX = max(wallX1, wallX2);
	//}
	//else if (inSegment1) {
	//	wallLeftmostX = wallRightmostX = wallX1;
	//}
	//else {
	//	wallLeftmostX = wallRightmostX = wallX2;
	//}

	//// 왼쪽 벽 충돌 체크
	//Vector2 moveDir = playerNewPos - playerOldPos;
	//bool wasRightOfWall = playerOldRightX <= wallLeftmostX;
	//bool isOnOrLeftOfWall = playerRightX >= wallLeftmostX - 2.0f; // 2픽셀 허용

	//if (wasRightOfWall && isOnOrLeftOfWall && moveDir.x <= 0) // 왼쪽으로 이동하거나 정지
	//{
	//	result.isColliding = true;
	//	result.collisionPoint = Vector2(wallLeftmostX, playerNewPos.y);

	//	Vector2 lineDir = lineEnd - lineStart;
	//	result.vHitNormal = Vector2(-lineDir.y, lineDir.x).GetNormalize();
	//	if (result.vHitNormal.x > 0) result.vHitNormal *= -1; // 왼쪽을 향하도록

	//	result.groundActor = wallCollider->GetOwner();
	//	result.penetrationDepth = max(0.0f, wallLeftmostX - playerRightX);
	//	return result;
	//}

	//// 오른쪽 벽 충돌 체크
	//bool wasLeftOfWall = playerOldLeftX >= wallRightmostX;
	//bool isOnOrRightOfWall = playerLeftX <= wallRightmostX + 2.0f; // 2픽셀 허용

	//if (wasLeftOfWall && isOnOrRightOfWall && moveDir.x >= 0) // 오른쪽으로 이동하거나 정지
	//{
	//	result.isColliding = true;
	//	result.collisionPoint = Vector2(wallRightmostX, playerNewPos.y);

	//	Vector2 lineDir = lineEnd - lineStart;
	//	result.vHitNormal = Vector2(-lineDir.y, lineDir.x).GetNormalize();
	//	if (result.vHitNormal.x < 0) result.vHitNormal *= -1; // 오른쪽을 향하도록

	//	result.groundActor = wallCollider->GetOwner();
	//	result.penetrationDepth = max(0.0f, playerLeftX - wallRightmostX);
	//	return result;
	//}

	//return result;
}

CollisionInfo CollisionManager::CheckLineStairCollision(Vector2 playerOldPos, Vector2 playerNewPos, float playerWidth, float playerHeight, Collider* stairCollider, bool wasStair)
{
	CollisionInfo result = {};
	result.collisionLayer = stairCollider->GetCollisionLayer();

	Vector2 stairStart = stairCollider->GetStartPoint();
	Vector2 stairEnd = stairCollider->GetEndPoint();
	Vector2 stairDir = stairEnd - stairStart;

	float halfW = playerWidth * 0.5f;
	float halfH = playerHeight * 0.5f;

	// 계단 방향 판단 (45도 가정)
	bool isRightUpStair = (stairDir.x > 0 && stairDir.y < 0) || (stairDir.x < 0 && stairDir.y > 0);

	// 이미 계단에 있는 경우의 처리
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
		GetYOnLineAtX(stairStart, stairEnd, targetCorner.x, yOnLine);

		// 계단 범위를 약간 벗어나도 근처에 있으면 계속 유지
		float stairMinX = min(stairStart.x, stairEnd.x);
		float stairMaxX = max(stairStart.x, stairEnd.x);
		float tolerance = 10.0f; // 10픽셀 여유

		if (targetCorner.x >= stairMinX - tolerance && targetCorner.x <= stairMaxX + tolerance)
		{
			// 계단 연장선상의 Y값 계산
			float extendedY;
			GetYOnLineAtX(stairStart, stairEnd, targetCorner.x, extendedY);

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
		// 계단 범위를 벗어났으면 충돌 없음
		return result;
	}

	// 새로운 계단 충돌 검사
	Vector2 moveDir = playerNewPos - playerOldPos;

	// 플레이어가 아래로 이동하거나 수평 이동할 때만 계단 충돌 검사
	if (moveDir.y < -1.f) // 위로 점프 중이면 계단 무시
		return result;

	// 양쪽 모서리 모두 검사하되, 계단 방향에 따라 우선순위 결정
	Vector2 bottomLeft = Vector2(playerNewPos.x - halfW, playerNewPos.y + halfH);
	Vector2 bottomRight = Vector2(playerNewPos.x + halfW, playerNewPos.y + halfH);

	Vector2 oldBottomLeft = Vector2(playerOldPos.x - halfW, playerOldPos.y + halfH);
	Vector2 oldBottomRight = Vector2(playerOldPos.x + halfW, playerOldPos.y + halfH);

	// 계단과의 교차점 검사
	Vector2 intersection;
	float t;
	float cross;
	bool foundCollision = false;
	int hitCorner = 0;
	float bestT = FLT_MAX;
	Vector2 bestIntersection;

	// 우상향 계단: 오른쪽 모서리 우선
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
				//// 계단 끝부분 체크
				//Vector2 stairDirection = stairDir.GetNormalize();
				//Vector2 stairStartToIntersection = intersection - stairStart;
				//float distanceAlongStair = stairStartToIntersection.Dot(stairDirection);
				//float stairLength = stairDir.Length();

				//// 계단 끝부분(90% 지점)이 아니면 충돌 처리
				//if (distanceAlongStair <= stairLength * 0.9f)
				//{
				//	bestT = t;
				//	bestIntersection = intersection;
				//	foundCollision = true;
				//	hitCorner = 2;
				//}

				bestT = t;
				bestIntersection = intersection;
				foundCollision = true;
				hitCorner = 2;
			}
		}
	}

	// 오른쪽에서 충돌이 없으면 왼쪽도 검사
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
				//// 계단 끝부분 체크
				//Vector2 stairDirection = stairDir.GetNormalize();
				//Vector2 stairStartToIntersection = intersection - stairStart;
				//float distanceAlongStair = stairStartToIntersection.Dot(stairDirection);
				//float stairLength = stairDir.Length();

				//// 계단 끝부분(90% 지점)이 아니면 충돌 처리
				//if (distanceAlongStair <= stairLength * 0.9f)
				//{
				//	bestT = t;
				//	bestIntersection = intersection;
				//	foundCollision = true;
				//	hitCorner = 1;
				//}

				bestT = t;
				bestIntersection = intersection;
				foundCollision = true;
				hitCorner = 1;
			}
		}
	}

	if (!foundCollision)
		return result;

	// 플레이어가 계단 위에서 접근하는지 확인 (아래에서 위로 올라가는 것 방지)
	Vector2 stairNormal = Vector2(-stairDir.y, stairDir.x).GetNormalize();
	if (stairNormal.y > 0) stairNormal *= -1;

	// 이동 방향이 계단 법선과 반대 방향인지 확인 (위에서 아래로)
	if (moveDir.y < -5.f)
		return result;

	//// 이동 방향이 계단 법선과 반대 방향인지 확인 (위에서 아래로)
	//if (moveDir.Dot(stairNormal) > 0.1f)
	//	return result;

	// 결과 설정
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
	if (abs(cross) < 1e-6f) return false; // 평행

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

	// 선분 B의 시작점 q1이 선분 A의 시작점 p1을 향하는지 검사
	float dot1 = dir1.x * (q1.x - p1.x) + dir1.y * (q1.y - p1.y);

	// 선분 A의 시작점 p1이 선분 B의 시작점 q1을 향하는지 검사
	float dot2 = dir2.x * (p1.x - q1.x) + dir2.y * (p1.y - q1.y);

	// 둘 중 하나의 선분이라도 다른 선분으로 향하고 있을 때만 교차로 간주
	if (dot1 < 0.0f || dot2 < 0.0f)
	{
		float cross = dir1.x * dir2.y - dir1.y * dir2.x;
		//if (abs(cross) < 1e-6f) return false; // 평행

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
	t = max(0.0f, min(1.0f, t)); // 0과 1 사이로 클램프

	return lineStart + lineVec * t;
}

bool CollisionManager::GetYOnLineAtX(const Vector2& a, const Vector2& b, float x, float& outY)
{
	const float EPS = 1e-6f;
	float dx = b.x - a.x;

	//if (fabs(dx) < EPS)
	//{
	//	// 수직선(계단은 45도로 가정하므로 거의 발생하지 않음) -> x가 거의 같으면 y 범위의 중간값 반환
	//	outInSegment = (fabs(x - a.x) < 1.0f); // 작지만 관대하게 처리
	//	outY = (a.y + b.y) * 0.5f;
	//	return outInSegment;
	//}

	float t = (x - a.x) / dx;
	outY = a.y + (b.y - a.y) * t;
	return (t >= 0.0f && t <= 1.0f);;
}

bool CollisionManager::GetXOnLineAtY(const Vector2& a, const Vector2& b, float y, float& outX)
{
	const float EPS = 1e-6f;
	float dy = b.y - a.y;

	// 수평선 처리 (기울기가 0인 경우)
	//if (fabs(dy) < EPS)
	//{
	//	outX = (a.x + b.x) * 0.5f; // x 범위의 중간값 반환
	//	return (fabs(y - a.y) < 1.0f); // y가 거의 같으면 선분 내부로 간주
	//}

	// 직선의 방정식 사용
	float t = (y - a.y) / dy; // 매개변수 t 계산
	outX = a.x + (b.x - a.x) * t; // 직선의 방정식으로 X값 계산
	return (t >= 0.0f && t <= 1.0f); // 선분 범위 내인지 확인
}
