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

void CollisionManager::Init()
{
	// �浹 ó���� ��ü�鸸 üũ
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

	// �Ѿ�, ���� �ݻ��
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
				if (receiveLayer == ECollisionLayer::ENEMY && sendLayer == ECollisionLayer::ENEMY_HITBOX)
				{
					wstring strs = std::format(L"{0}\n", 1 << 8);
					OutputDebugString(strs.c_str());
					wstring str2 = std::format(L"{0}\n", (COLLISION_BIT_MASK_BLOCK[receiveLayer] & (uint16)(1 << sendLayer)));
					OutputDebugString(str2.c_str());
				}

				if (!(COLLISION_BIT_MASK_BLOCK[receiveLayer] & (uint16)(1 << sendLayer))) continue;

				for (auto sendCollider : _colliderList[(ECollisionLayer)sendLayer])
				{
					// ó�� �浹�� ����(�ݸ��� �ʿ��� ����)
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
			// ����
			COLLISION_BIT_MASK_IGNORE[(int16)layer1] |= ((uint16)1 << (int16)layer2);
			COLLISION_BIT_MASK_IGNORE[(int16)layer2] |= ((uint8)1 << (int16)layer1);
		}
		else
		{
			// ����
			COLLISION_BIT_MASK_IGNORE[(int16)layer1] &= ~((uint16)1 << (int16)layer2);
			COLLISION_BIT_MASK_IGNORE[(int16)layer2] &= ~((uint16)1 << (int16)layer1);
		}
		break;
	case C_BLOCK:
		if (on)
		{
			// ����
			COLLISION_BIT_MASK_BLOCK[(int16)layer1] |= ((uint16)1 << (int16)layer2);
			COLLISION_BIT_MASK_BLOCK[(int16)layer2] |= ((uint16)1 << (int16)layer1);
		}
		else
		{
			// ����
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

// Liang-Barsky �˰���
bool CollisionManager::CheckLinetoAABB(Vector2 start, Vector2 end, Vector2 AABBmin, Vector2 AABBmax, CollisionInfo& info)
{
	// ���� ����
	float dx = end.x - start.x;
	float dy = end.y - start.y;

	// 4�� ��迡 ���� p�� q ��
	// ���� p: ��->�� (����), ��� p: ��->�� (Ż��)
	float p[4] = { -dx, dx, -dy, dy };
	float q[4] = {
		start.x - AABBmin.x,   // ���� ������ �Ÿ�
		AABBmax.x - start.x,   // ������ ������ �Ÿ�  
		start.y - AABBmin.y,   // �Ʒ� ������ �Ÿ�
		AABBmax.y - start.y    // �� ������ �Ÿ�
	};

	float u1 = 0.0f;			// �ִ� ���� �Ű�����
	float u2 = 1.0f;			// �ּ� Ż�� �Ű�����

	for (int i = 0; i < 4; i++) {
		if (p[i] == 0) {
			// ������ ���� ����
			if (q[i] < 0) {
				// �����ϸ鼭 ��� �� -> �浹 ����
				return false;
			}
			// �����ϸ鼭 ��� ���� -> ��� �˻�
		}
		else {
			float t = q[i] / p[i];

			if (p[i] < 0) {
				// ������: u1 = max(u1, t)
				u1 = max(u1, t);
			}
			else {
				// Ż����: u2 = min(u2, t)
				u2 = min(u2, t);
			}

			// ���� ����: ������ Ż�⺸�� ������ �浹 ����
			if (u1 > u2) {
				return false;
			}
		}
	}

	// u1 <= u2�̸� �浹
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

	// �Ϲ����� ����: �� ������ �ٸ� ������ ��������
	if (ccw1 * ccw2 < 0 && ccw3 * ccw4 < 0) {
		return true;
	}

	// Ư�� ���̽�: �� ���� �ٸ� ���� ���� �ִ� ���
	if (ccw1 == 0 && IsPointOnSegment(start1, end1, start2)) return true;
	if (ccw2 == 0 && IsPointOnSegment(start1, end1, end2)) return true;
	if (ccw3 == 0 && IsPointOnSegment(start2, end2, start1)) return true;
	if (ccw4 == 0 && IsPointOnSegment(start2, end2, end1)) return true;

	return false;
}

// �� ���� ���⼺�� ��� (�ð�ݴ����, �ð����, ������)
int32 CollisionManager::CCW(const Vector2& a, const Vector2& b, const Vector2& c) {
	float cross = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);

	if (cross > 0) return 1;   // �ݽð����
	if (cross < 0) return -1;  // �ð����
	return 0;                   // ������
}

bool CollisionManager::IsPointOnSegment(const Vector2& a, const Vector2& b, const Vector2& c)
{
	// c�� a�� b ���̿� �ִ��� Ȯ�� (���������� ���� ȣ���)
	return min(a.x, b.x) <= c.x && c.x <= max(a.x, b.x) && min(a.y, b.y) <= c.y && c.y <= max(a.y, b.y);
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

bool CollisionManager::OverlapOnAxis(const std::vector<Vector2>& send, const std::vector<Vector2>& receive, Vector2 axis)
{
	// �� ����ȭ
	float len = sqrtf(axis.x * axis.x + axis.y * axis.y);
	if (len == 0.0f) return true; // �߸��� �� �� ����
	axis.x /= len;
	axis.y /= len;

	auto range1 = ProjectionAxis(send, axis);
	auto range2 = ProjectionAxis(receive, axis);

	// ���� ��ħ ���� Ȯ��
	return !(range1.y < range2.x || range2.y < range1.x);
}

bool CollisionManager::PlayerCollisionCheck(Collider* receive, Collider* send, CollisionInfo& info)
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
		info = CheckLineWallCollision(player->GetPos(), movementComponent->GetNewPos(), playerWidth, playerHeight, send);
		break;
	case ECollisionLayer::CEILING:
		info = CheckLineCeilingCollision(player->GetPos(), movementComponent->GetNewPos(), playerWidth, playerHeight, send);
		break;
	case ECollisionLayer::STAIR:
		if (movementComponent->GetIsPlatform()) break;
		info = CheckLineStairCollision(player->GetPos(), movementComponent->GetNewPos(), playerWidth, playerHeight, send, movementComponent->GetOnStair());
		break;
	case ECollisionLayer::PLATFORM:
		info = CheckLinePlatformCollision(player->GetPos(), movementComponent->GetNewPos(), playerWidth, playerHeight, send);
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

	Enemy* enemy = static_cast<Enemy*>(receive->GetOwner());

	RECT oldRect = {
		receive->GetPos().x - halfWidth,
		receive->GetPos().y - halfHeight,
		receive->GetPos().x + halfWidth,
		receive->GetPos().y + halfHeight
	};

	EnemyMovementComponent* movementComponent = enemy->GetComponent<EnemyMovementComponent>();

	RECT newRect = {
		movementComponent->GetNewPos().x - halfWidth,
		movementComponent->GetNewPos().y - halfHeight,
		movementComponent->GetNewPos().x + halfWidth,
		movementComponent->GetNewPos().y + halfHeight
	};

	switch (send->GetCollisionLayer())
	{
	case ECollisionLayer::GROUND:
		info = CheckAABBGroundCollision(oldRect, newRect, send);
		break;
	case ECollisionLayer::WALL:
		info = CheckLineWallCollision(enemy->GetPos(), movementComponent->GetNewPos(), colliderWidth, colliderHeight, send);
		break;
	case ECollisionLayer::CEILING:
		info = CheckLineCeilingCollision(enemy->GetPos(), movementComponent->GetNewPos(), colliderWidth, colliderHeight, send);
		break;
	case ECollisionLayer::STAIR:
		info = CheckLineStairCollision(enemy->GetPos(), movementComponent->GetNewPos(), colliderWidth, colliderHeight, send, movementComponent->GetOnStair());
		break;
	case ECollisionLayer::PLATFORM:
		info = CheckLinePlatformCollision(enemy->GetPos(), movementComponent->GetNewPos(), colliderWidth, colliderHeight, send);
		break;
	case ECollisionLayer::ENEMY_HITBOX:
		if (!send->GetOwner()->GetWasHit()) break;
		if (send->GetColliderType() == EColliderType::LINE)
		{
			Vector2 aabbMin = { (float)newRect.left, (float)newRect.top };
			Vector2 aabbMax = { (float)newRect.right, (float)newRect.bottom };
			// �ݻ�Ʊ� ������ ����, �� ���� �ݴ�� �־���
			info.isColliding = CheckLinetoAABB(send->GetEndPoint(), send->GetStartPoint(), aabbMin, aabbMax, info);
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

bool CollisionManager::CheckOBBHitBox(const vector<Vector2>& OBB, ECollisionLayer layer, vector<Collider*>& hitActors, Vector2 attackDir)
{
	bool flag = false;

	for (int32 receiveLayer = 0; receiveLayer < ECollisionLayer::END; ++receiveLayer)
	{
		if (!(COLLISION_BIT_MASK_BLOCK[receiveLayer] & (uint8)(1 << layer))) continue;

		for (auto receiveCollider : _colliderList[(ECollisionLayer)receiveLayer])
		{
			if (receiveCollider->GetOwner()->GetWasHit()) continue;

			if (receiveLayer == ECollisionLayer::ENEMY)
			{
				float halfWidth = receiveCollider->GetWidth() * 0.5f;
				float halfHeight = receiveCollider->GetHeight() * 0.5f;
				Vector2 pos = receiveCollider->GetPos();

				// ���� ��, ������ ��, ������ �Ʒ�, ���� �Ʒ� ��
				vector<Vector2> colliderCorners = {
					Vector2(pos.x - halfWidth, pos.y - halfHeight),
					Vector2(pos.x + halfWidth, pos.y - halfHeight),
					Vector2(pos.x + halfWidth, pos.y + halfHeight),
					Vector2(pos.x - halfWidth, pos.y + halfHeight)
				};

				if (CheckOBBtoAABB(OBB, colliderCorners))
				{
					flag = true;
					hitActors.push_back(receiveCollider);
					receiveCollider->GetOwner()->TakeDamage(attackDir);
				}
			}
			else if (receiveLayer == ECollisionLayer::ENEMY_HITBOX)
			{
				if (receiveCollider->GetColliderType() == EColliderType::LINE)
				{
				}
			}
		}
	}

	return flag;
}

bool CollisionManager::CheckOBBHitBox(Vector2 center, float radian, float width, float height, ECollisionLayer layer, vector<Collider*>& hitActors, Vector2 attackDir)
{
	// obb vs aabb ��
	vector<Vector2> corners = GetRotatedCorners(center.x, center.y, radian, width, height);

	// obb vs line ��
	float cosRotation = cosf(radian);
	float sinRotation = sinf(radian);
	Vector2 localAxisX = Vector2(cosRotation, sinRotation);
	Vector2 localAxisY = Vector2(-sinRotation, cosRotation);
	Vector2 AABBmin = Vector2(-width * 0.5f, -height * 0.5f);
	Vector2 AABBmax = Vector2(width * 0.5f, height * 0.5f);

	bool flag = false;

	for (int32 receiveLayer = 0; receiveLayer < ECollisionLayer::END; ++receiveLayer)
	{
		if (!(COLLISION_BIT_MASK_BLOCK[receiveLayer] & (uint8)(1 << layer))) continue;

		for (auto receiveCollider : _colliderList[(ECollisionLayer)receiveLayer])
		{
			if (receiveCollider->GetOwner()->GetWasHit()) continue;

			if (receiveLayer == ECollisionLayer::ENEMY)
			{
				float halfWidth = receiveCollider->GetWidth() * 0.5f;
				float halfHeight = receiveCollider->GetHeight() * 0.5f;
				Vector2 pos = receiveCollider->GetPos();

				// ���� ��, ������ ��, ������ �Ʒ�, ���� �Ʒ� ��
				vector<Vector2> colliderCorners = {
					Vector2(pos.x - halfWidth, pos.y - halfHeight),
					Vector2(pos.x + halfWidth, pos.y - halfHeight),
					Vector2(pos.x + halfWidth, pos.y + halfHeight),
					Vector2(pos.x - halfWidth, pos.y + halfHeight)
				};

				if (CheckOBBtoAABB(corners, colliderCorners))
				{
					flag = true;
					hitActors.push_back(receiveCollider);
					receiveCollider->GetOwner()->TakeDamage(attackDir);
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
						hitActors.push_back(receiveCollider);
						receiveCollider->GetOwner()->TakeDamage(attackDir);
					}
				}
			}
		}
	}

	return flag;
}

bool CollisionManager::CheckOBBtoAABB(const vector<Vector2>& OBB, const vector<Vector2>& AABB)
{
	// �˻��� ��: OBB�� �� ���� ��������, AABB�� x��, y���� ��������
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
			return false; // �и��� �߰� �� �浹 ����
		}
	}

	return true; // ��� �࿡�� ��ħ �� �浹 ����
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

	// ���� ��ġ���� ��ġ���� Ȯ��
	// ������ �Ʒ��� ���� ���̱⸸ �ص� ��ġ�� �ɷ� �Ǵ�.
	if (!(playerNewRect.left < groundRect.right &&
		playerNewRect.right > groundRect.left &&
		playerNewRect.top < groundRect.bottom &&
		playerNewRect.bottom >= groundRect.top))
	{
		return result;
	}

	// ��� ���⿡�� �浹�ߴ��� Ȯ��
	bool fromTop = (playerOldRect.bottom <= groundRect.top && playerNewRect.bottom >= groundRect.top);		// ������ �Ʒ�
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
	result.collisionActor = groundCollider->GetOwner();

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

	Vector2 moveDir = playerNewPos - playerOldPos;
	if (moveDir.y < 0) return result; // �Ʒ��� �̵��� ����

	Vector2 lineStart = lineCollider->GetStartPoint();
	Vector2 lineEnd = lineCollider->GetEndPoint();

	float halfWidth = playerWidth * 0.5f;
	float halfHeight = playerHeight * 0.5f;

	// ���� �÷��̾� �ٴ� Y��ǥ
	float playerBottomY = playerNewPos.y + halfHeight;
	float playerOldBottomY = playerOldPos.y + halfHeight;

	// �÷��̾� X �������� �÷����� Y�� ���
	float playerLeftX = playerNewPos.x - halfWidth;
	float playerRightX = playerNewPos.x + halfWidth;

	float outY1, outY2;

	bool inSegment1 = GetYOnLineAtX(lineStart, lineEnd, playerLeftX, outY1);
	bool inSegment2 = GetYOnLineAtX(lineStart, lineEnd, playerRightX, outY2);

	if (!inSegment1 && !inSegment2) return result;

	// �÷����� �ְ��� ã��
	float platformTopY = (inSegment1 && inSegment2) ? min(outY1, outY2) :
		(inSegment1 ? outY1 : outY2);

	// �������� ���� �־���, ����� �÷����� ��Ұų� �����ߴ��� Ȯ��
	bool wasAbove = playerOldBottomY <= platformTopY;
	bool isOnOrBelow = playerBottomY >= platformTopY - 2.0f; // 2�ȼ� ���

	if (wasAbove && isOnOrBelow)
	{
		result.isColliding = true;
		result.collisionPoint = Vector2(playerNewPos.x, platformTopY);

		Vector2 lineDir = lineEnd - lineStart;
		result.vHitNormal = Vector2(-lineDir.y, lineDir.x).GetNormalize();
		if (result.vHitNormal.y > 0) result.vHitNormal *= -1;

		result.collisionActor = lineCollider->GetOwner();
		result.penetrationDepth = max(0.0f, playerBottomY - platformTopY);
	}

	return result;
}

CollisionInfo CollisionManager::CheckLineWallCollision(Vector2 playerOldPos, Vector2 playerNewPos, float playerWidth, float playerHeight, Collider* wallCollider)
{
	CollisionInfo result = {};

	Vector2 moveDir = playerNewPos - playerOldPos;

	Vector2 lineStart = wallCollider->GetStartPoint();
	Vector2 lineEnd = wallCollider->GetEndPoint();

	float halfWidth = playerWidth * 0.5f;
	float halfHeight = playerHeight * 0.5f;

	// �÷��̾��� �¿� �̵� ��ο� ���� ������ �˻�
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
			result.vHitNormal = Vector2(-1, 0); // �̵� �ݴ� ����
			result.collisionActor = wallCollider->GetOwner();
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
			result.collisionActor = wallCollider->GetOwner();
		}
	}
	// �÷��̾ �̵� ���� �ƴϾ ���� ��Ҵ����� Ȯ��
	else
	{
		// ������
		moveStart = Vector2(playerOldPos.x + halfWidth, playerOldPos.y);
		moveEnd = Vector2(playerNewPos.x + halfWidth + 1.f, playerNewPos.y);
		if (LineIntersectsLineSegment(moveStart, moveEnd, lineStart, lineEnd, intersection, t, cross))
		{
			result.isColliding = true;
			result.collisionPoint = intersection;
			result.vHitNormal = Vector2(-1, 0);
			result.collisionActor = wallCollider->GetOwner();
		}
		// ����
		moveStart = Vector2(playerOldPos.x - halfWidth, playerOldPos.y);
		moveEnd = Vector2(playerNewPos.x - halfWidth - 1.f, playerNewPos.y);
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

CollisionInfo CollisionManager::CheckLineCeilingCollision(Vector2 playerOldPos, Vector2 playerNewPos, float playerWidth, float playerHeight, Collider* wallCollider)
{
	CollisionInfo result = {};

	Vector2 moveDir = playerNewPos - playerOldPos;

	Vector2 lineStart = wallCollider->GetStartPoint();
	Vector2 lineEnd = wallCollider->GetEndPoint();

	float halfHeight = playerHeight * 0.5f;

	Vector2 moveStart;
	Vector2 moveEnd;

	Vector2 intersection;
	float t;
	float cross;

	if (moveDir.y < 0)
	{
		moveStart = Vector2(playerOldPos.x, playerOldPos.y - halfHeight);
		moveEnd = Vector2(playerNewPos.x, playerNewPos.y - halfHeight);
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

CollisionInfo CollisionManager::CheckLineStairCollision(Vector2 playerOldPos, Vector2 playerNewPos, float playerWidth, float playerHeight, Collider* stairCollider, bool wasStair)
{
	CollisionInfo result = {};

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
		GetYOnLineAtX(stairStart, stairEnd, targetCorner.x, yOnLine);

		// ��� ������ �ణ ����� ��ó�� ������ ��� ����
		float stairMinX = min(stairStart.x, stairEnd.x);
		float stairMaxX = max(stairStart.x, stairEnd.x);
		float tolerance = 10.0f; // 10�ȼ� ����

		if (targetCorner.x >= stairMinX - tolerance && targetCorner.x <= stairMaxX + tolerance)
		{
			// ��� ���弱���� Y�� ���
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
				bestT = t;
				bestIntersection = intersection;
				foundCollision = true;
				hitCorner = 2;
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
				bestT = t;
				bestIntersection = intersection;
				foundCollision = true;
				hitCorner = 1;
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
