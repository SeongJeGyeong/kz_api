#include "pch.h"
#include "CollisionManager.h"
#include "../Components/Collider.h"
#include "../Components/OBBCollider.h"
#include "../Components/AABBCollider.h"
#include "../Components/LineCollider.h"
#include "../Objects/Actors/Actor.h"
#include "../Objects/Actors/Player.h"
#include "../Game/Game.h"
#include "../Components/PlayerMovementComponent.h"
#include "../Objects/Actors/Enemy.h"
#include "../Components/EnemyMovementComponent.h"
#include "../Components/MovingLineCollider.h"
#include "../Objects/Actors/Axe.h"

void CollisionManager::Init()
{
	// 충돌 처리될 객체들만 체크
	SetBitFlag(ECollisionLayer::PLAYER, ECollisionLayer::GROUND, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::PLAYER, ECollisionLayer::WALL, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::PLAYER, ECollisionLayer::CEILING, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::PLAYER, ECollisionLayer::PLATFORM, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::PLAYER, ECollisionLayer::STAIR, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::PLAYER, ECollisionLayer::GROUND, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::PLAYER, ECollisionLayer::ENEMY_HITBOX, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::PLAYER, ECollisionLayer::PORTAL, ECollisionResponse::C_BLOCK, true);

	SetBitFlag(ECollisionLayer::ENEMY, ECollisionLayer::GROUND, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::ENEMY, ECollisionLayer::WALL, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::ENEMY, ECollisionLayer::CEILING, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::ENEMY, ECollisionLayer::PLATFORM, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::ENEMY, ECollisionLayer::STAIR, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::ENEMY, ECollisionLayer::PLAYER_HITBOX, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::ENEMY, ECollisionLayer::ENEMY_HITBOX, ECollisionResponse::C_BLOCK, true);

	// 총알, 도끼 반사용
	SetBitFlag(ECollisionLayer::ENEMY_HITBOX, ECollisionLayer::ENEMY, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::ENEMY_HITBOX, ECollisionLayer::PLAYER_HITBOX, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::ENEMY_HITBOX, ECollisionLayer::GROUND, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::ENEMY_HITBOX, ECollisionLayer::WALL, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::ENEMY_HITBOX, ECollisionLayer::CEILING, ECollisionResponse::C_BLOCK, true);
	SetBitFlag(ECollisionLayer::ENEMY_HITBOX, ECollisionLayer::STAIR, ECollisionResponse::C_BLOCK, true);
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
	//PLAYER_HITBOX
	//ENEMY_HITBOX
	//PORTAL
	//END

	for (int32 receiveLayer = 0; receiveLayer < ECollisionLayer::END; ++receiveLayer)
	{
		for (auto receiveCollider : _colliderList[(ECollisionLayer)receiveLayer])
		{
			for (int32 sendLayer = 0; sendLayer < ECollisionLayer::END; ++sendLayer)
			{
				if (!(COLLISION_BIT_MASK_BLOCK[receiveLayer] & (uint16)(1 << sendLayer))) continue;

				for (auto sendCollider : _colliderList[(ECollisionLayer)sendLayer])
				{
					if (receiveCollider->GetColliderId() == 6)
					{
						int i = 0;
					}
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
							info.collisionLayer = receiveCollider->GetCollisionLayer();
							info.collisionActor = receiveCollider->GetOwner();
							sendCollider->GetOwner()->OnCollisionStayOverlap(info);

							info.collisionLayer = sendCollider->GetCollisionLayer();
							info.collisionActor = sendCollider->GetOwner();
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
	COLLISION_BIT_MASK_IGNORE[(int16)type] |= ((uint16)1 << (int16)ignore);
	COLLISION_BIT_MASK_IGNORE[(int16)ignore] |= ((uint16)1 << (int16)type);
}

void CollisionManager::SetBitFlag(ECollisionLayer layer1, ECollisionLayer layer2, ECollisionResponse responseType, bool on)
{
	switch (responseType)
	{
	case C_IGNORE:
		if (on)
		{
			// 설정
			COLLISION_BIT_MASK_IGNORE[(int16)layer1] |= ((uint16)1 << (int16)layer2);
			COLLISION_BIT_MASK_IGNORE[(int16)layer2] |= ((uint8)1 << (int16)layer1);
		}
		else
		{
			// 해제
			COLLISION_BIT_MASK_IGNORE[(int16)layer1] &= ~((uint16)1 << (int16)layer2);
			COLLISION_BIT_MASK_IGNORE[(int16)layer2] &= ~((uint16)1 << (int16)layer1);
		}
		break;
	case C_BLOCK:
		if (on)
		{
			// 설정
			COLLISION_BIT_MASK_BLOCK[(int16)layer1] |= ((uint16)1 << (int16)layer2);
			COLLISION_BIT_MASK_BLOCK[(int16)layer2] |= ((uint16)1 << (int16)layer1);
		}
		else
		{
			// 해제
			COLLISION_BIT_MASK_BLOCK[(int16)layer1] &= ~((uint16)1 << (int16)layer2);
			COLLISION_BIT_MASK_BLOCK[(int16)layer2] &= ~((uint16)1 << (int16)layer1);
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
		return PlayerCollisionCheck(receive, send, info);
	}
	else if (receive->GetCollisionLayer() == ECollisionLayer::ENEMY)
	{
		return EnemyCollisionCheck(receive, send, info);
	}
	else if (receive->GetCollisionLayer() == ECollisionLayer::ENEMY_HITBOX)
	{
		if (receive->GetColliderType() == EColliderType::LINE)
		{
			return BulletCollisionCheck(receive, send, info);
		}
		else
		{
			return AxeCollisionCheck(receive, send, info);
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
		pair<float, float> proj1 = receive->ProjectionToAxis(axes[i]);
		pair<float, float> proj2 = send->ProjectionToAxis(axes[i]);

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

	info.vHitNormal = minAxis;
	Vector2 hitPos = CalculateHitPos(receive, send, minAxis);
	info.collisionPoint = hitPos;
	
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
		pair<float, float> proj1 = receive->ProjectionToAxis(axes[i]);
		pair<float, float> proj2 = send->ProjectionToAxis(axes[i]);

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


	info.vHitNormal = minAxis;
	Vector2 hitPos = CalculateHitPos(receive, send, minAxis);
	info.collisionPoint = hitPos;

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

	info.collisionActor = send->GetOwner();
	info.vHitNormal = normal;
	info.collisionPoint = hitPos;
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

// Liang-Barsky 알고리즘
bool CollisionManager::CheckLinetoAABB(Vector2 start, Vector2 end, Vector2 AABBmin, Vector2 AABBmax, CollisionInfo& info)
{
	// 방향 벡터
	float dx = end.x - start.x;
	float dy = end.y - start.y;

	// 4개 경계에 대한 p와 q 값
	// 음수 p: 밖->안 (진입), 양수 p: 안->밖 (탈출)
	float p[4] = { -dx, dx, -dy, dy };
	float q[4] = {
		start.x - AABBmin.x,   // 왼쪽 경계까지 거리
		AABBmax.x - start.x,   // 오른쪽 경계까지 거리  
		start.y - AABBmin.y,   // 아래 경계까지 거리
		AABBmax.y - start.y    // 위 경계까지 거리
	};

	float u1 = 0.0f;			// 최대 진입 매개변수
	float u2 = 1.0f;			// 최소 탈출 매개변수

	for (int i = 0; i < 4; i++) {
		if (p[i] == 0) {
			// 선분이 경계와 평행
			if (q[i] < 0) {
				// 평행하면서 경계 밖 -> 충돌 없음
				return false;
			}
			// 평행하면서 경계 내부 -> 계속 검사
		}
		else {
			float t = q[i] / p[i];

			if (p[i] < 0) {
				// 진입점: u1 = max(u1, t)
				u1 = max(u1, t);
			}
			else {
				// 탈출점: u2 = min(u2, t)
				u2 = min(u2, t);
			}

			// 조기 종료: 진입이 탈출보다 늦으면 충돌 없음
			if (u1 > u2) {
				return false;
			}
		}
	}

	// u1 <= u2이면 충돌
	if (u1 <= u2)
	{
		info.vHitNormal = (end - start).GetNormalize();
		return true;
	}

	return false;
}

bool CollisionManager::CheckLinetoLine(Vector2 start1, Vector2 end1, Vector2 start2, Vector2 end2, CollisionInfo& info)
{
	int ccw1 = CCW(start1, end1, start2);
	int ccw2 = CCW(start1, end1, end2);
	int ccw3 = CCW(start2, end2, start1);
	int ccw4 = CCW(start2, end2, end1);

	// 일반적인 교차: 각 선분이 다른 선분을 가로지름
	if (ccw1 * ccw2 < 0 && ccw3 * ccw4 < 0) {
		return true;
	}

	// 특수 케이스: 한 점이 다른 선분 위에 있는 경우
	if (ccw1 == 0 && IsPointOnSegment(start1, end1, start2)) return true;
	if (ccw2 == 0 && IsPointOnSegment(start1, end1, end2)) return true;
	if (ccw3 == 0 && IsPointOnSegment(start2, end2, start1)) return true;
	if (ccw4 == 0 && IsPointOnSegment(start2, end2, end1)) return true;

	return false;
}

// 세 점의 방향성을 계산 (시계반대방향, 시계방향, 일직선)
int32 CollisionManager::CCW(const Vector2& a, const Vector2& b, const Vector2& c) {
	float cross = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);

	if (cross > 0) return 1;   // 반시계방향
	if (cross < 0) return -1;  // 시계방향
	return 0;                   // 일직선
}

bool CollisionManager::IsPointOnSegment(const Vector2& a, const Vector2& b, const Vector2& c)
{
	// c가 a와 b 사이에 있는지 확인 (일직선상일 때만 호출됨)
	return min(a.x, b.x) <= c.x && c.x <= max(a.x, b.x) && min(a.y, b.y) <= c.y && c.y <= max(a.y, b.y);
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

bool CollisionManager::OverlapOnAxis(const std::vector<Vector2>& send, const std::vector<Vector2>& receive, Vector2 axis)
{
	// 축 정규화
	float len = sqrtf(axis.x * axis.x + axis.y * axis.y);
	if (len == 0.0f) return true; // 잘못된 축 → 무시
	axis.x /= len;
	axis.y /= len;

	auto range1 = ProjectionAxis(send, axis);
	auto range2 = ProjectionAxis(receive, axis);

	// 구간 겹침 여부 확인
	return !(range1.y < range2.x || range2.y < range1.x);
}

bool CollisionManager::PlayerCollisionCheck(Collider* receive, Collider* send, CollisionInfo& info)
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

	PlayerMovementComponent* movementComponent = player->GetComponent<PlayerMovementComponent>();

	RECT playerNewRect = {
		movementComponent->GetNewPos().x - halfWidth,
		movementComponent->GetNewPos().y - halfHeight,
		movementComponent->GetNewPos().x + halfWidth,
		movementComponent->GetNewPos().y + halfHeight
	};

	switch (send->GetCollisionLayer())
	{
	case ECollisionLayer::GROUND:
		info = CheckAABBGroundCollision(playerOldRect, playerNewRect, send);
		break;
	case ECollisionLayer::WALL:
		info = CheckAABBtoLineWallCollision(player->GetPos(), movementComponent->GetNewPos(), halfWidth, halfHeight, send);
		break;
	case ECollisionLayer::CEILING:
		info = CheckAABBtoLineCeilingCollision(player->GetPos(), movementComponent->GetNewPos(), halfWidth, halfHeight, send);
		break;
	case ECollisionLayer::STAIR:
		if (movementComponent->GetIsPlatform()) break;
		info = CheckAABBtoLineStairCollision(player->GetPos(), movementComponent->GetNewPos(), halfWidth, halfHeight, send, movementComponent->GetOnStair());
		break;
	case ECollisionLayer::PLATFORM:
		info = CheckAABBtoLinePlatformCollision(player->GetPos(), movementComponent->GetNewPos(), halfWidth, halfHeight, send);
		break;
	case ECollisionLayer::ENEMY_HITBOX:
		info.isColliding = CheckBetweenAABB(static_cast<AABBCollider*>(receive), static_cast<AABBCollider*>(send), info);
		break;
	default:
		break;
	}

	info.collisionLayer = send->GetCollisionLayer();
	return info.isColliding;
}

bool CollisionManager::EnemyCollisionCheck(Collider* receive, Collider* send, CollisionInfo& info)
{
	float colliderWidth = receive->GetWidth();
	float colliderHeight = receive->GetHeight();
	float halfWidth = colliderWidth * 0.5f;
	float halfHeight = colliderHeight * 0.5f;

	Actor* receiveOwner = receive->GetOwner();

	RECT oldRect = {
		receive->GetPos().x - halfWidth,
		receive->GetPos().y - halfHeight,
		receive->GetPos().x + halfWidth,
		receive->GetPos().y + halfHeight
	};

	RECT newRect = {
		receiveOwner->GetNewPos().x - halfWidth,
		receiveOwner->GetNewPos().y - halfHeight,
		receiveOwner->GetNewPos().x + halfWidth,
		receiveOwner->GetNewPos().y + halfHeight
	};

	switch (send->GetCollisionLayer())
	{
	case ECollisionLayer::GROUND:
		info = CheckAABBGroundCollision(oldRect, newRect, send);
		break;
	case ECollisionLayer::WALL:
		info = CheckAABBtoLineWallCollision(receive->GetPos(), receiveOwner->GetNewPos(), halfWidth, halfHeight, send);
		break;
	case ECollisionLayer::CEILING:
		info = CheckAABBtoLineCeilingCollision(receive->GetPos(), receiveOwner->GetNewPos(), halfWidth, halfHeight, send);
		break;
	case ECollisionLayer::STAIR:
		{
			Enemy* enemy = static_cast<Enemy*>(receiveOwner);
			bool onStair = enemy->GetComponent<EnemyMovementComponent>()->GetOnStair();
			info = CheckAABBtoLineStairCollision(receive->GetPos(), receiveOwner->GetNewPos(), halfWidth, halfHeight, send, onStair);
		}
		break;
	case ECollisionLayer::PLATFORM:
		info = CheckAABBtoLinePlatformCollision(receive->GetPos(), receiveOwner->GetNewPos(), halfWidth, halfHeight, send);
		break;
	case ECollisionLayer::ENEMY_HITBOX:
		if (send->GetColliderType() == EColliderType::LINE)
		{
			Vector2 aabbMin = { (float)newRect.left, (float)newRect.top };
			Vector2 aabbMax = { (float)newRect.right, (float)newRect.bottom };
			// 반사됐기 때문에 시작, 끝 지점 반대로 넣어줌
			info.isColliding = CheckLinetoAABB(send->GetEndPoint(), send->GetStartPoint(), aabbMin, aabbMax, info);
		}
		else
		{
			info.isColliding = CheckBetweenAABB(static_cast<AABBCollider*>(receive), static_cast<AABBCollider*>(send), info);
		}
		break;
	default:
		break;
	}

	info.collisionLayer = send->GetCollisionLayer();
	return info.isColliding;
}

bool CollisionManager::BulletCollisionCheck(Collider* receive, Collider* send, CollisionInfo& info)
{
	Vector2 pos = receive->GetPos();
	float radian = receive->GetRadian();

	float halfLength = receive->GetLength() * 0.5f;
	float dx = cos(radian) * halfLength;
	float dy = sin(radian) * halfLength;
	Vector2 start = { pos.x - dx, pos.y - dy };
	Vector2 end = { pos.x + dx, pos.y + dy };

	bool collide = false;
	switch (send->GetCollisionLayer())
	{
	case ECollisionLayer::GROUND:
		{
			RECT AABB = static_cast<AABBCollider*>(send)->GetRect();
			Vector2 AABBmin = { (float)AABB.left, (float)AABB.top };
			Vector2 AABBmax = { (float)AABB.right, (float)AABB.bottom };
			info.isColliding = CheckLinetoAABB(start, end, AABBmin, AABBmax, info);
		}
		break;
	case ECollisionLayer::WALL:
		info.isColliding = CheckLinetoLine(start, end, send->GetStartPoint(), send->GetEndPoint(), info);
		break;
	case ECollisionLayer::CEILING:
		info.isColliding = CheckLinetoLine(start, end, send->GetStartPoint(), send->GetEndPoint(), info);
		break;
	case ECollisionLayer::STAIR:
		info.isColliding = CheckLinetoLine(start, end, send->GetStartPoint(), send->GetEndPoint(), info);
		break;
	default:
		break;
	}

	info.collisionLayer = send->GetCollisionLayer();
	return info.isColliding;
}

bool CollisionManager::AxeCollisionCheck(Collider* receive, Collider* send, CollisionInfo& info)
{
	float axeWidth = receive->GetWidth();
	float axeHeight = receive->GetHeight();
	float halfWidth = axeWidth * 0.5f;
	float halfHeight = axeHeight * 0.5f;

	RECT oldRect = {
		receive->GetPos().x - halfWidth,
		receive->GetPos().y - halfHeight,
		receive->GetPos().x + halfWidth,
		receive->GetPos().y + halfHeight
	};

	Axe* axe = static_cast<Axe*>(receive->GetOwner());
	Vector2 newPos = axe->GetNewPos();

	RECT newRect = {
		newPos.x - halfWidth,
		newPos.y - halfHeight,
		newPos.x + halfWidth,
		newPos.y + halfHeight
	};

	switch (send->GetCollisionLayer())
	{
	case ECollisionLayer::GROUND:
		info = CheckAABBGroundCollision(oldRect, newRect, send);
		break;
	case ECollisionLayer::WALL:
		info = CheckAABBtoLineWallCollision(axe->GetPos(), axe->GetNewPos(), halfWidth, halfHeight, send);
		break;
	case ECollisionLayer::CEILING:
		info = CheckAABBtoLineCeilingCollision(axe->GetPos(), axe->GetNewPos(), halfWidth, halfHeight, send);
		break;
	case ECollisionLayer::ENEMY:
		info.isColliding = CheckBetweenAABB(static_cast<AABBCollider*>(receive), static_cast<AABBCollider*>(send), info);
		break;
	}

	info.collisionLayer = send->GetCollisionLayer();
	return info.isColliding;
}

bool CollisionManager::CheckOBBHitBox(Actor* attackActor, AttackInfo& hitInfo)
{
	Vector2 center = attackActor->GetPos();
	// obb vs aabb 용
	vector<Vector2> corners = GetRotatedCorners(center.x, center.y, hitInfo.fAttackRadian, hitInfo.fWidth, hitInfo.fHeight);

	// obb vs line 용
	float cosRotation = cosf(hitInfo.fAttackRadian);
	float sinRotation = sinf(hitInfo.fAttackRadian);
	Vector2 localAxisX = Vector2(cosRotation, sinRotation);
	Vector2 localAxisY = Vector2(-sinRotation, cosRotation);
	Vector2 AABBmin = Vector2(-hitInfo.fWidth * 0.5f, -hitInfo.fHeight * 0.5f);
	Vector2 AABBmax = Vector2(hitInfo.fWidth * 0.5f, hitInfo.fHeight * 0.5f);

	bool flag = false;

	for (int32 receiveLayer = 0; receiveLayer < ECollisionLayer::END; ++receiveLayer)
	{
		if (!(COLLISION_BIT_MASK_BLOCK[receiveLayer] & (uint16)(1 << hitInfo._attackLayer))) continue;

		for (auto receiveCollider : _colliderList[(ECollisionLayer)receiveLayer])
		{
			if (receiveCollider->GetOwner()->GetWasHit()) continue;
			if (MatchColliderId(receiveCollider, hitInfo._hitActors)) continue;

			if (receiveLayer == ECollisionLayer::ENEMY)
			{
				float halfWidth = receiveCollider->GetWidth() * 0.5f;
				float halfHeight = receiveCollider->GetHeight() * 0.5f;
				Vector2 pos = receiveCollider->GetPos();

				// 왼쪽 위, 오른쪽 위, 오른쪽 아래, 왼쪽 아래 순
				vector<Vector2> colliderCorners = {
					Vector2(pos.x - halfWidth, pos.y - halfHeight),
					Vector2(pos.x + halfWidth, pos.y - halfHeight),
					Vector2(pos.x + halfWidth, pos.y + halfHeight),
					Vector2(pos.x - halfWidth, pos.y + halfHeight)
				};

				if (CheckOBBtoAABB(corners, colliderCorners))
				{
					flag = true;
					hitInfo._hitActors.push_back(receiveCollider);
					receiveCollider->GetOwner()->TakeDamage(attackActor, hitInfo.vAttackDir);
				}
			}
			else if (receiveLayer == ECollisionLayer::ENEMY_HITBOX)
			{
				if (receiveCollider->GetColliderType() == EColliderType::LINE)
				{
					Vector2 start = receiveCollider->GetStartPoint();
					Vector2 end = receiveCollider->GetEndPoint();

					Vector2 relative1 = start - center;
					relative1 = { relative1.Dot(localAxisX), relative1.Dot(localAxisY) };
					Vector2 relative2 = end - center;
					relative2 = { relative2.Dot(localAxisX), relative2.Dot(localAxisY) };

					CollisionInfo info;
					if (CheckLinetoAABB(relative1, relative2, AABBmin, AABBmax, info))
					{
						flag = true;
						hitInfo._hitActors.push_back(receiveCollider);
						receiveCollider->GetOwner()->TakeDamage(attackActor, hitInfo.vAttackDir);
					}
				}
			}
		}
	}

	return flag;
}

bool CollisionManager::CheckAABBHitBox(Actor* attackActor, Vector2 center, float width, float height)
{
	Vector2 hitBoxMin = { center.x - width * 0.5f, center.y - height * 0.5f };
	Vector2 hitBoxMax = { center.x + width * 0.5f, center.y + height * 0.5f };

	for (auto& collider : _colliderList[ECollisionLayer::PLAYER])
	{
		Vector2 colPos = collider->GetPos();
		float colWidth = collider->GetWidth();
		float colHeight = collider->GetHeight();

		Vector2 colMin = { colPos.x - colWidth * 0.5f, colPos.y - colHeight * 0.5f };
		Vector2 colMax = { colPos.x + colWidth * 0.5f, colPos.y + colHeight * 0.5f };

		// 충돌체끼리 중심을 잇는 벡터
		double nx = center.x - colPos.x;
		double ny = center.y - colPos.y;

		// 충돌체의 길이, 높이의 절반
		Vector2 receiveExtent = (colMax - colMin) * 0.5f;
		Vector2 sendExtent = (hitBoxMax - hitBoxMin) * 0.5f;

		// n.x가 1이거나 -1이면 x축이 충돌
		double overlapX = receiveExtent.x + sendExtent.x - abs(nx);
		// n.y가 1이거나 -1이면 y축이 충돌
		double overlapY = receiveExtent.y + sendExtent.y - abs(ny);

		// 충돌하지 않으면 true를 반환해서 히트박스를 계속 유지
		if (overlapX < 0 || overlapY < 0) return true;

		Vector2 dir = colPos - center;
		dir.Normalize();
		collider->GetOwner()->TakeDamage(attackActor, dir);

		//Vector2 normal;
		//Vector2 hitPos;

		//// 위, 아래 충돌
		//if (overlapX > overlapY)
		//{
		//	if (ny < 0)
		//	{
		//		normal = Vector2(0, -1);
		//	}
		//	else
		//	{
		//		normal = Vector2(0, 1);
		//	}

		//	double penetration = overlapY;
		//	// 사각형끼리 겹치는 위치는 2군데
		//	int32 contactCount = 2;
		//	Vector2 contactPoint1 = Vector2(max(colMin.x, hitBoxMin.x), (ny < 0 ? colMin.y : colMax.y));
		//	Vector2 contactPoint2 = Vector2(max(colMax.x, hitBoxMax.x), contactPoint1.y);
		//	hitPos = { (contactPoint1.x + contactPoint2.x) * 0.5f, (contactPoint1.y + contactPoint2.y) * 0.5f };
		//}
		//// 왼쪽, 오른쪽 충돌
		//else
		//{
		//	if (nx < 0)
		//	{
		//		normal = Vector2(-1, 0);
		//	}
		//	else
		//	{
		//		normal = Vector2(1, 0);
		//	}

		//	double penetration = overlapX;
		//	int32 contactCount = 2;
		//	Vector2 contactPoint1 = Vector2((nx < 0 ? colMin.x : colMax.x), max(colMin.y, hitBoxMin.y));
		//	Vector2 contactPoint2 = Vector2(contactPoint1.x, min(colMax.y, hitBoxMax.y));
		//	hitPos = { (contactPoint1.x + contactPoint2.x) * 0.5f, (contactPoint1.y + contactPoint2.y) * 0.5f };
		//}
	}

	return false;
}

bool CollisionManager::CheckOBBtoAABB(const vector<Vector2>& OBB, const vector<Vector2>& AABB)
{
	// 검사할 축: OBB의 두 변의 법선벡터, AABB의 x축, y축의 법선벡터
	Vector2 edge1 = { OBB[1].x - OBB[0].x, OBB[1].y - OBB[0].y };
	Vector2 edge2 = { OBB[3].x - OBB[0].x, OBB[3].y - OBB[0].y };
	Vector2 axes[4] = {
		{ -edge1.y, edge1.x },
		{ -edge2.y, edge2.x },
		{ 1, 0 }, 
		{ 0, 1 }
	};

	for (auto& axis : axes)
	{
		if (!OverlapOnAxis(OBB, AABB, axis))
		{
			return false; // 분리축 발견 → 충돌 없음
		}
	}

	return true; // 모든 축에서 겹침 → 충돌 있음
}

CollisionInfo CollisionManager::CheckAABBGroundCollision(const RECT& playerOldRect, const RECT& playerNewRect, Collider* groundCollider)
{
	CollisionInfo result = {};
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
	result.collisionActor = groundCollider->GetOwner();

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

CollisionInfo CollisionManager::CheckAABBtoLinePlatformCollision(Vector2 oldPos, Vector2 newPos, float halfWidth, float halfHeight, Collider* lineCollider)
{
	CollisionInfo result = {};

	Vector2 moveDir = newPos - oldPos;
	if (moveDir.y < 0) return result; // 아래로 이동할 때만

	Vector2 lineStart = lineCollider->GetStartPoint();
	Vector2 lineEnd = lineCollider->GetEndPoint();

	// 현재 플레이어 바닥 Y좌표
	float playerBottomY = newPos.y + halfHeight;
	float playerOldBottomY = oldPos.y + halfHeight;

	// 플레이어 X 범위에서 플랫폼의 Y값 계산
	float playerLeftX = newPos.x - halfWidth;
	float playerRightX = newPos.x + halfWidth;

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
		result.collisionPoint = Vector2(newPos.x, platformTopY);

		Vector2 lineDir = lineEnd - lineStart;
		result.vHitNormal = Vector2(-lineDir.y, lineDir.x).GetNormalize();
		if (result.vHitNormal.y > 0) result.vHitNormal *= -1;

		result.collisionActor = lineCollider->GetOwner();
		result.penetrationDepth = max(0.0f, playerBottomY - platformTopY);
	}

	return result;
}

CollisionInfo CollisionManager::CheckAABBtoLineWallCollision(Vector2 oldPos, Vector2 newPos, float halfWidth, float halfHeight, Collider* wallCollider)
{
	CollisionInfo result = {};

	Vector2 moveDir = newPos - oldPos;

	Vector2 lineStart = wallCollider->GetStartPoint();
	Vector2 lineEnd = wallCollider->GetEndPoint();

	// 플레이어의 좌우 이동 경로와 벽의 교차점 검사
	Vector2 moveStart;
	Vector2 moveEnd;

	Vector2 intersection;
	float t;
	float cross;

	if (moveDir.x > 0)
	{
		moveStart = Vector2(oldPos.x + halfWidth, oldPos.y);
		moveEnd = Vector2(newPos.x + halfWidth, newPos.y);
		if (LineIntersectsLineSegment(moveStart, moveEnd, lineStart, lineEnd, intersection, t, cross))
		{
			result.isColliding = true;
			result.collisionPoint = intersection;
			result.vHitNormal = Vector2(-1, 0); // 이동 반대 방향
			result.collisionActor = wallCollider->GetOwner();
		}
	}
	else if(moveDir.x < 0)
	{
		moveStart = Vector2(oldPos.x - halfWidth, oldPos.y);
		moveEnd = Vector2(newPos.x - halfWidth, newPos.y);
		if (LineIntersectsLineSegment(moveStart, moveEnd, lineStart, lineEnd, intersection, t, cross))
		{
			result.isColliding = true;
			result.collisionPoint = intersection;
			result.vHitNormal = Vector2(1, 0);
			result.collisionActor = wallCollider->GetOwner();
		}
	}
	// 플레이어가 이동 중이 아니어도 벽에 닿았는지는 확인
	else
	{
		// 오른쪽
		moveStart = Vector2(oldPos.x + halfWidth, oldPos.y);
		moveEnd = Vector2(newPos.x + halfWidth + 1.f, newPos.y);
		if (LineIntersectsLineSegment(moveStart, moveEnd, lineStart, lineEnd, intersection, t, cross))
		{
			result.isColliding = true;
			result.collisionPoint = intersection;
			result.vHitNormal = Vector2(-1, 0);
			result.collisionActor = wallCollider->GetOwner();
		}
		// 왼쪽
		moveStart = Vector2(oldPos.x - halfWidth, oldPos.y);
		moveEnd = Vector2(newPos.x - halfWidth - 1.f, newPos.y);
		if (LineIntersectsLineSegment(moveStart, moveEnd, lineStart, lineEnd, intersection, t, cross))
		{
			result.isColliding = true;
			result.collisionPoint = intersection;
			result.vHitNormal = Vector2(1, 0);
			result.collisionActor = wallCollider->GetOwner();
		}
	}

	return result;
}

CollisionInfo CollisionManager::CheckAABBtoLineCeilingCollision(Vector2 oldPos, Vector2 newPos, float halfWidth, float halfHeight, Collider* wallCollider)
{
	CollisionInfo result = {};

	Vector2 moveDir = newPos - oldPos;

	Vector2 lineStart = wallCollider->GetStartPoint();
	Vector2 lineEnd = wallCollider->GetEndPoint();

	Vector2 moveStart;
	Vector2 moveEnd;

	Vector2 intersection;
	float t;
	float cross;

	if (moveDir.y < 0)
	{
		moveStart = Vector2(oldPos.x, oldPos.y - halfHeight);
		moveEnd = Vector2(newPos.x, newPos.y - halfHeight);
		if (LineIntersectsLineSegment(moveStart, moveEnd, lineStart, lineEnd, intersection, t, cross))
		{
			result.isColliding = true;
			result.collisionPoint = intersection;
			result.vHitNormal = Vector2(0, 1);
			result.collisionActor = wallCollider->GetOwner();
		}
	}

	return result;
}

CollisionInfo CollisionManager::CheckAABBtoLineStairCollision(Vector2 oldPos, Vector2 newPos, float halfWidth, float halfHeight, Collider* stairCollider, bool wasStair)
{
	CollisionInfo result = {};

	Vector2 stairStart = stairCollider->GetStartPoint();
	Vector2 stairEnd = stairCollider->GetEndPoint();
	Vector2 stairDir = stairEnd - stairStart;

	// 계단 방향 판단 (45도 가정)
	bool isRightUpStair = (stairDir.x > 0 && stairDir.y < 0) || (stairDir.x < 0 && stairDir.y > 0);

	// 이미 계단에 있는 경우의 처리
	if (wasStair)
	{
		Vector2 targetCorner;
		if (isRightUpStair)
		{
			targetCorner = Vector2(newPos.x + halfWidth, newPos.y + halfHeight);
		}
		else
		{
			targetCorner = Vector2(newPos.x - halfWidth, newPos.y + halfHeight);
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
				result.collisionActor = stairCollider->GetOwner();
				result.hitCorner = isRightUpStair ? 2 : 1;
				return result;
			}
		}
		// 계단 범위를 벗어났으면 충돌 없음
		return result;
	}

	// 새로운 계단 충돌 검사
	Vector2 moveDir = newPos - oldPos;

	// 플레이어가 아래로 이동하거나 수평 이동할 때만 계단 충돌 검사
	if (moveDir.y < -1.f) // 위로 점프 중이면 계단 무시
		return result;

	// 양쪽 모서리 모두 검사하되, 계단 방향에 따라 우선순위 결정
	Vector2 bottomLeft = Vector2(newPos.x - halfWidth, newPos.y + halfHeight);
	Vector2 bottomRight = Vector2(newPos.x + halfWidth, newPos.y + halfHeight);

	Vector2 oldBottomLeft = Vector2(oldPos.x - halfWidth, oldPos.y + halfHeight);
	Vector2 oldBottomRight = Vector2(oldPos.x + halfWidth, oldPos.y + halfHeight);

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
	result.collisionActor = stairCollider->GetOwner();
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

bool CollisionManager::GetYOnLineAtX(const Vector2& a, const Vector2& b, float x, float& outY)
{
	const float EPS = 1e-6f;
	float dx = b.x - a.x;

	float t = (x - a.x) / dx;
	outY = a.y + (b.y - a.y) * t;
	return (t >= 0.0f && t <= 1.0f);;
}

void CollisionManager::DeleteCollider(Actor* actor)
{
	Collider* collider = actor->GetCollider();
	ECollisionLayer layer = collider->GetCollisionLayer();

	auto iter = find(_colliderList[layer].begin(), _colliderList[layer].end(), collider);
	if (iter != _colliderList[layer].end())
	{
		iter = _colliderList[layer].erase(iter);
	}
}

bool CollisionManager::MatchColliderId(Collider* collider, vector<Collider*> list)
{
	for (Collider* col : list)
	{
		if (collider->GetColliderId() == col->GetColliderId()) return true;
	}
	return false;
}
