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
	SetBitFlag(ECollisionLayer::WALL, ECollisionLayer::WALL, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::CEILING, ECollisionLayer::CEILING, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::PLATFORM, ECollisionLayer::PLATFORM, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::GROUND, ECollisionLayer::WALL, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::GROUND, ECollisionLayer::CEILING, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::GROUND, ECollisionLayer::PLATFORM, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::WALL, ECollisionLayer::GROUND, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::WALL, ECollisionLayer::CEILING, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::WALL, ECollisionLayer::PLATFORM, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::CEILING, ECollisionLayer::GROUND, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::CEILING, ECollisionLayer::WALL, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::CEILING, ECollisionLayer::PLATFORM, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::PLATFORM, ECollisionLayer::GROUND, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::PLATFORM, ECollisionLayer::WALL, ECollisionResponse::C_IGNORE, true);
	SetBitFlag(ECollisionLayer::PLATFORM, ECollisionLayer::CEILING, ECollisionResponse::C_IGNORE, true);

	// 타일은 따로 검사
	//SetBitFlag(CollisionLayer::PLAYER, CollisionLayer::TILE_FG, CollisionResponse::C_IGNORE, true);

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
					CollisionInfo info;
					if (CheckCollision(receiveCollider, sendCollider, info))
					{
						if (COLLISION_BIT_MASK_BLOCK[receiveLayer] & (uint8)(1 << sendLayer))
						{
							sendCollider->GetOwner()->OnCollisionHit(receiveCollider);
							receiveCollider->GetOwner()->OnCollisionHit(sendCollider);
						}
						else
						{
							sendCollider->GetOwner()->OnCollisionBeginOverlap(receiveCollider);
							receiveCollider->GetOwner()->OnCollisionBeginOverlap(sendCollider);
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
	info.vHitPos = start + dir * tMin;

	if (tMax <= lineLength) {
		Vector2 exitPoint = start + dir * tMax;
		info.vHitPos = (info.vHitPos + exitPoint) * 0.5f;
	}

	// 예외처리 하나더.
	info.vHitPos += info.vHitNormal * 0.01f; // 약간의 오프셋을 주어 정확한 충돌 위치를 보정

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

// 플레이어의 궤적 line과 다른 충돌체가 충돌하는지 검사
bool CollisionManager::CheckBlockedCollision(Player* player, Vector2 start, Vector2 end, CollisionInfo& info)
{
	// 전체 액터를 다 순회하면서, Line, AABB 충돌체크
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
					// 더 가까운 충돌체로 충돌 갱신
					if (t < tMin)
					{
						info.vHitNormal = normal;
						info.vHitPos = hitPos;
						tMin = t;
						closestActor = collider->GetOwner();
					}
				}
			}
			else
			{
				Vector2 playerMoveDir = end - start;
				Vector2 lineDir = collider->GetEndPoint() - collider->GetStartPoint();
				// 라인 법선벡터
				Vector2 lineNormal = Vector2(-lineDir.y, lineDir.x).GetNormalize();

				// 플레이어 AABB를 라인의 법선 방향으로 투영한 범위 계산
				float projectedExtent = fabs(lineNormal.x * (playerWidth * 0.5f)) + fabs(lineNormal.y * (playerHeight * 0.5f));

				// 라인을 양쪽으로 확장
				Vector2 expandedStart1 = collider->GetStartPoint() + lineNormal * projectedExtent;
				Vector2 expandedEnd1 = collider->GetEndPoint() + lineNormal * projectedExtent;
				Vector2 expandedStart2 = collider->GetStartPoint() - lineNormal * projectedExtent;
				Vector2 expandedEnd2 = collider->GetEndPoint() - lineNormal * projectedExtent;

				Vector2 normal, hitPos;
				float t;

				// 플레이어 중심점의 이동 경로와 확장된 라인들의 교점 검사
				if (LineIntersectsLine(start, end, expandedStart1, expandedEnd1, normal, hitPos, t))
				{
					if (t < tMin && t >= 0.0f && t <= 1.0f)
					{
						tMin = t;
						info.vHitNormal = { -normal.x, -normal.y};
						info.vHitPos = hitPos;
						closestActor = collider->GetOwner();
					}
				}
				else if (LineIntersectsLine(start, end, expandedStart2, expandedEnd2, normal, hitPos, t))
				{
					if (t < tMin && t >= 0.0f && t <= 1.0f)
					{
						tMin = t;
						info.vHitNormal = normal;
						info.vHitPos = hitPos;
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

vector<PlayerGroundCollisionResult> CollisionManager::CheckPlayerGroundCollision(Player* player, Vector2 playerOldPos, Vector2 playerNewPos)
{
	vector<PlayerGroundCollisionResult> results;

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

	float minPenetration = FLT_MAX;

	// GROUND와 PLATFORM 레이어 검사
	for (int32 type = ECollisionLayer::GROUND; type <= ECollisionLayer::PLATFORM; ++type)
	{
		for (auto collider : _colliderList[type])
		{
			PlayerGroundCollisionResult result;

			if (collider->GetColliderType() == EColliderType::AABB)
			{
				// AABB vs AABB 바닥 충돌 검사
				result = CheckAABBGroundCollision(playerOldRect, playerNewRect, collider);
			}
			else if (collider->GetColliderType() == EColliderType::LINE)
			{
				// AABB vs Line 바닥 충돌 검사
				result = CheckLinePlatformCollision(playerOldPos, playerNewPos, playerWidth, playerHeight, collider);
			}

			if (result.isColliding)
			{
				//minPenetration = result.penetrationDepth;
				results.push_back(result);
			}
		}
	}

	return results;
}

PlayerGroundCollisionResult CollisionManager::CheckAABBGroundCollision(const RECT& playerOldRect, const RECT& playerNewRect, Collider* groundCollider)
{
	PlayerGroundCollisionResult result = {};
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
	if (!(playerNewRect.left < groundRect.right &&
		playerNewRect.right > groundRect.left &&
		playerNewRect.top < groundRect.bottom &&
		playerNewRect.bottom > groundRect.top))
	{
		return result; // 겹치지 않음
	}

	// 이전 위치에서는 겹치지 않았는지 확인 (새로운 충돌인지)
	bool wasOverlapping = (playerOldRect.left < groundRect.right &&
		playerOldRect.right > groundRect.left &&
		playerOldRect.top < groundRect.bottom &&
		playerOldRect.bottom > groundRect.top);

	if (wasOverlapping)
	{
		return result; // 이미 겹쳐있던 상태라면 새로운 바닥 충돌이 아님
	}

	// 플레이어가 위에서 아래로 이동했는지 확인
	if (playerOldRect.bottom <= groundRect.top && playerNewRect.bottom > groundRect.top)
	{
		result.isColliding = true;
		result.penetrationDepth = playerNewRect.bottom - groundRect.top;
		result.collisionPoint = Vector2(
			(max(playerNewRect.left, groundRect.left) + min(playerNewRect.right, groundRect.right)) * 0.5f,
			groundRect.top
		);
		result.normal = Vector2(0, -1); // 위쪽 방향
		result.groundActor = groundCollider->GetOwner();
	}

	return result;
}

PlayerGroundCollisionResult CollisionManager::CheckLinePlatformCollision(Vector2 playerOldPos, Vector2 playerNewPos, float playerWidth, float playerHeight, Collider* lineCollider)
{
	PlayerGroundCollisionResult result = {};
	result.collisionLayer = lineCollider->GetCollisionLayer();
	Vector2 lineStart = lineCollider->GetStartPoint();
	Vector2 lineEnd = lineCollider->GetEndPoint();
	Vector2 lineDir = lineEnd - lineStart;

	// 수직선인지 확인 (수직선은 바닥이 될 수 없음)
	if (abs(lineDir.x) < 1e-6f) // 거의 수직
	{
		return result;
	}

	// 라인의 법선 벡터 계산
	Vector2 lineNormal = Vector2(-lineDir.y, lineDir.x).GetNormalize();

	// 법선이 위쪽을 향하도록 조정 (Y가 감소하는 방향이 위쪽)
	if (lineNormal.y > 0) lineNormal *= -1;

	// 플레이어의 바닥 변을 확장한 선분들
	float halfWidth = playerWidth * 0.5f;
	float halfHeight = playerHeight * 0.5f;

	Vector2 playerOldBottom = Vector2(playerOldPos.x, playerOldPos.y + halfHeight);
	Vector2 playerNewBottom = Vector2(playerNewPos.x, playerNewPos.y + halfHeight);

	// 플레이어 바닥의 좌우 끝점들
	Vector2 oldBottomLeft = Vector2(playerOldPos.x - halfWidth, playerOldPos.y + halfHeight);
	Vector2 oldBottomRight = Vector2(playerOldPos.x + halfWidth, playerOldPos.y + halfHeight);
	Vector2 newBottomLeft = Vector2(playerNewPos.x - halfWidth, playerNewPos.y + halfHeight);
	Vector2 newBottomRight = Vector2(playerNewPos.x + halfWidth, playerNewPos.y + halfHeight);

	// 플레이어의 이동 경로와 라인의 교차점 검사
	std::vector<Vector2> intersectionPoints;
	float closestT = FLT_MAX;
	Vector2 closestIntersection;

	// 플레이어 바닥 중앙, 좌측, 우측의 이동 경로 검사
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
		// 플레이어가 위에서 아래로 접근했는지 확인
		Vector2 moveDir = playerNewPos - playerOldPos;
		if (moveDir.y > 0 && lineNormal.Dot(moveDir) < 0) // 아래로 이동하면서 라인에 접근
		{
			result.isColliding = true;
			result.collisionPoint = closestIntersection;
			result.normal = lineNormal;
			result.groundActor = lineCollider->GetOwner();

			// 침투 깊이 계산
			Vector2 currentBottom = Vector2(playerNewPos.x, playerNewPos.y + halfHeight);
			result.penetrationDepth = max(0.0f,
				currentBottom.y - (closestIntersection.y + lineNormal.y * 0.1f));
		}
	}

	return result;
}

vector<PlayerGroundCollisionResult> CollisionManager::CheckPlayerCollision(Player* player, Vector2 playerOldPos, Vector2 playerNewPos)
{
	vector<PlayerGroundCollisionResult> results;

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
	for (int32 type = ECollisionLayer::GROUND; type <= ECollisionLayer::PLATFORM; ++type)
	{
		for (auto collider : _colliderList[type])
		{
			PlayerGroundCollisionResult result;

			if (collider->GetColliderType() == EColliderType::AABB)
			{
				// AABB 충돌 검사
				if (type == ECollisionLayer::GROUND)
				{
					result = CheckAABBGroundCollision(playerOldRect, playerNewRect, collider);
				}
				else if (type == ECollisionLayer::WALL)
				{
					result = CheckAABBWallCollision(playerOldRect, playerNewRect, collider);
				}
			}
			else if (collider->GetColliderType() == EColliderType::LINE)
			{
				// Line 충돌 검사
				if (type == ECollisionLayer::PLATFORM)
				{
					result = CheckLinePlatformCollision(playerOldPos, playerNewPos, playerWidth, playerHeight, collider);
				}
				else if (type == ECollisionLayer::WALL)
				{
					result = CheckLineWallCollision(playerOldPos, playerNewPos, playerWidth, playerHeight, collider);
				}
				else if (!player->GetIsStair() && type == ECollisionLayer::STAIR)
				{
					result = CheckLineStiarCollision2(playerOldPos, playerNewPos, playerWidth, playerHeight, collider, player->GetIsStair());
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

PlayerGroundCollisionResult CollisionManager::CheckAABBWallCollision(const RECT& playerOldRect, const RECT& playerNewRect, Collider* wallCollider)
{
	PlayerGroundCollisionResult result = {};
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

	// 현재 위치에서 겹치는지 확인
	if (!(playerNewRect.left < wallRect.right &&
		playerNewRect.right > wallRect.left &&
		playerNewRect.top < wallRect.bottom &&
		playerNewRect.bottom > wallRect.top))
	{
		return result; // 겹치지 않음
	}

	// 이전 위치에서는 겹치지 않았는지 확인 (새로운 충돌인지)
	bool wasOverlapping = (playerOldRect.left < wallRect.right &&
		playerOldRect.right > wallRect.left &&
		playerOldRect.top < wallRect.bottom &&
		playerOldRect.bottom > wallRect.top);

	if (wasOverlapping)
	{
		return result; // 이미 겹쳐있던 상태라면 새로운 충돌이 아님
	}

	// 좌우 충돌 확인 (플레이어가 좌우에서 벽으로 이동)
	if ((playerOldRect.right <= wallRect.left && playerNewRect.right > wallRect.left) ||
		(playerOldRect.left >= wallRect.right && playerNewRect.left < wallRect.right))
	{
		result.isColliding = true;
		result.groundActor = wallCollider->GetOwner();

		if (playerNewRect.right > wallRect.left && playerNewRect.left < wallRect.left)
		{
			// 왼쪽에서 오른쪽으로 이동하며 벽 충돌
			result.normal = Vector2(-1, 0);
			result.collisionPoint = Vector2(wallRect.left, (playerNewRect.top + playerNewRect.bottom) * 0.5f);
		}
		else
		{
			// 오른쪽에서 왼쪽으로 이동하며 벽 충돌
			result.normal = Vector2(1, 0);
			result.collisionPoint = Vector2(wallRect.right, (playerNewRect.top + playerNewRect.bottom) * 0.5f);
		}
	}

	return result;
}

PlayerGroundCollisionResult CollisionManager::CheckLineWallCollision(Vector2 playerOldPos, Vector2 playerNewPos, float playerWidth, float playerHeight, Collider* wallCollider)
{
	PlayerGroundCollisionResult result = {};
	result.collisionLayer = wallCollider->GetCollisionLayer();

	Vector2 moveDir = playerNewPos - playerOldPos;
	if (moveDir.x == 0) return result;

	Vector2 lineStart = wallCollider->GetStartPoint();
	Vector2 lineEnd = wallCollider->GetEndPoint();
	Vector2 lineDir = lineEnd - lineStart;

	// 라인의 법선 벡터 계산 (좌우 방향)
	//Vector2 lineNormal = Vector2(-lineDir.y, lineDir.x).GetNormalize();
	//if (lineNormal.x < 0) lineNormal *= -1; // 항상 오른쪽을 향하도록

	float halfWidth = playerWidth * 0.5f;
	//float halfHeight = playerHeight * 0.5f;

	// 플레이어의 좌우 이동 경로와 벽의 교차점 검사
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
		result.normal = (moveDir.x > 0) ? Vector2(-1, 0) : Vector2(1, 0); // 이동 반대 방향
		result.groundActor = wallCollider->GetOwner();
	}

	return result;
}

PlayerGroundCollisionResult CollisionManager::CheckLineStairCollision(Vector2 playerOldPos, Vector2 playerNewPos, float playerWidth, float playerHeight, Collider* stairCollider, bool wasStair)
{
	PlayerGroundCollisionResult result = {};
	result.collisionLayer = stairCollider->GetCollisionLayer();

	Vector2 lineStart = stairCollider->GetStartPoint();
	Vector2 lineEnd = stairCollider->GetEndPoint();
	Vector2 lineDir = lineEnd - lineStart;

	// 라인의 법선 벡터 계산
	Vector2 lineNormal = Vector2(-lineDir.y, lineDir.x).GetNormalize();
	// 법선이 위쪽을 향하도록 조정
	if (lineNormal.y > 0) lineNormal *= -1;

	float halfWidth = playerWidth * 0.5f;
	float halfHeight = playerHeight * 0.5f;

	if (wasStair)
	{
		float yOnLine;
		bool inSeg;
		CollisionManager::GetInstance()->GetYOnLineAtX(lineStart, lineEnd, playerNewPos.x, yOnLine, inSeg);

		if (inSeg && fabs((playerNewPos.y + halfHeight) - yOnLine) <= 4.0f) // 4픽셀 정도 허용
		{
			result.isColliding = true;
			result.collisionPoint = Vector2(playerNewPos.x, yOnLine);
			result.normal = Vector2(-lineDir.y, lineDir.x).GetNormalize();
			if (result.normal.y > 0) result.normal *= -1;
			result.groundActor = stairCollider->GetOwner();
			result.hitCorner = 2;
			return result;
		}

		//float halfWidth = playerWidth * 0.5f;
		//float minX = min(lineStart.x, lineEnd.x);
		//float maxX = max(lineStart.x, lineEnd.x);

		//// 플레이어가 이 계단의 X 범위 내에 있는지 확인
		//if (playerNewPos.x + halfWidth >= minX &&
		//	playerNewPos.x - halfWidth <= maxX)  // 약간의 여유 공간
		//{
		//	// 계단에 계속 붙어있음
		//	result.isColliding = true;
		//	result.collisionPoint = Vector2(playerNewPos.x, 0);
		//	result.normal = Vector2(-lineDir.y, lineDir.x).GetNormalize();
		//	if (result.normal.y > 0) result.normal *= -1;
		//	result.groundActor = stairCollider->GetOwner();
		//	result.hitCorner = 2; // 특별한 상태 표시
		//	return result;
		//}
	}

	// 플레이어의 바닥 모서리들
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
	// 왼쪽
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
	// 오른쪽
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
	result.normal = lineNormal;
	result.groundActor = stairCollider->GetOwner();
	result.hitCorner = hitCorner;

	return result;
}

PlayerGroundCollisionResult CollisionManager::CheckLineStiarCollision2(Vector2 playerOldPos, Vector2 playerNewPos, float playerWidth, float playerHeight, Collider* stairCollider, bool wasStair)
{
	PlayerGroundCollisionResult result = {};
	result.collisionLayer = stairCollider->GetCollisionLayer();

	Vector2 stairStart = stairCollider->GetStartPoint();
	Vector2 stairEnd = stairCollider->GetEndPoint();

	float halfW = playerWidth * 0.5f;
	float halfH = playerHeight * 0.5f;

	// 플레이어 하단 모서리
	Vector2 bottomLeft(playerNewPos.x - halfW, playerNewPos.y + halfH);
	Vector2 bottomRight(playerNewPos.x + halfW, playerNewPos.y + halfH);

	float stairYLeft, stairYRight;
	bool inSegLeft, inSegRight;
	GetYOnLineAtX(stairStart, stairEnd, bottomLeft.x, stairYLeft, inSegLeft);
	GetYOnLineAtX(stairStart, stairEnd, bottomRight.x, stairYRight, inSegRight);

	// 두 모서리 모두 계단 범위 밖이면 충돌 없음
	if (!inSegLeft && !inSegRight)
		return result;

	// 계단 위인지 확인 (모서리가 계단 y보다 살짝 아래 있으면 충돌로 간주)
	float yTol = 5.0f; // 허용 오차
	bool leftOnStair = inSegLeft && fabs(bottomLeft.y - stairYLeft) <= yTol;
	bool rightOnStair = inSegRight && fabs(bottomRight.y - stairYRight) <= yTol;

	if (!leftOnStair && !rightOnStair)
		return result;

	// 더 가까운 모서리 선택
	int hitCorner = 0;
	Vector2 collisionPoint;
	if (leftOnStair && (!rightOnStair || fabs(bottomLeft.y - stairYLeft) < fabs(bottomRight.y - stairYRight)))
	{
		collisionPoint = Vector2(bottomLeft.x, stairYLeft);
		hitCorner = 1;
	}
	else
	{
		collisionPoint = Vector2(bottomRight.x, stairYRight);
		hitCorner = 2;
	}

	// 결과 채우기
	Vector2 lineDir = stairEnd - stairStart;
	Vector2 normal = Vector2(-lineDir.y, lineDir.x).GetNormalize();
	if (normal.y > 0) normal *= -1;

	result.isColliding = true;
	result.collisionPoint = collisionPoint;
	result.normal = normal;
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

bool CollisionManager::GetYOnLineAtX(const Vector2& a, const Vector2& b, float x, float& outY, bool& outInSegment)
{
	const float EPS = 1e-6f;
	float dx = b.x - a.x;
	if (fabs(dx) < EPS)
	{
		// 수직선(계단은 45도로 가정하므로 거의 발생하지 않음) -> x가 거의 같으면 y 범위의 중간값 반환
		outInSegment = (fabs(x - a.x) < 1.0f); // 작지만 관대하게 처리
		outY = (a.y + b.y) * 0.5f;
		return outInSegment;
	}
	float t = (x - a.x) / dx;
	outInSegment = (t >= 0.0f && t <= 1.0f);
	outY = a.y + (b.y - a.y) * t;
	return outInSegment;
}
