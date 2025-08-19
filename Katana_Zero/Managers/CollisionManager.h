#pragma once
#include "../Utils/Singleton.h"

struct CollisionMapID
{
	uint32 left;
	uint32 right;

	CollisionMapID(uint32 a = 0, uint32 b = 0)
	{
		if (a < b)
		{
			left = a;
			right = b;
		}
		else
		{
			left = b;
			right = a;
		}
	}

	// 64비트 단일 값으로 변환
	uint64 ToUInt64() const noexcept
	{
		return (static_cast<uint64>(left) << 32) | static_cast<uint64>(right);
	}

	// 동등 비교
	bool operator==(const CollisionMapID& other) const noexcept
	{
		return left == other.left && right == other.right;
	}
};

class Collider;
class OBBCollider;
class AABBCollider;
class LineCollider;
class Player;

class CollisionManager : public Singleton<CollisionManager>
{
	friend Singleton<CollisionManager>;
private:
	CollisionManager(){}

	vector<Collider*> _colliderList[(int32)ECollisionLayer::END];

	unordered_map<uint64, bool> _collisionMap;

	uint8 COLLISION_BIT_MASK_IGNORE[(int32)ECollisionLayer::END] = {};
	uint8 COLLISION_BIT_MASK_BLOCK[(int32)ECollisionLayer::END] = {};

	void SetIgnoreFlag(ECollisionLayer type, ECollisionLayer ignore);
	void SetBitFlag(ECollisionLayer layer1, ECollisionLayer layer2, ECollisionResponse responseType, bool on);

	bool CheckCollision(Collider* receive, Collider* send, CollisionInfo& info);
	bool CheckBetweenOBB(OBBCollider* receive, OBBCollider* send, CollisionInfo& info);
	bool CheckBetweenSAT(OBBCollider* receive, OBBCollider* send, CollisionInfo& info);
	bool CheckBetweenAABB(AABBCollider* receive, AABBCollider* send, CollisionInfo& info);
	bool CheckAABBtoLine(AABBCollider* receive, LineCollider* send, CollisionInfo& info);

	bool CheckSeparatingAxis(pair<float, float> proj1, pair<float, float> proj2);

	Vector2 CalculateHitPos(OBBCollider* receive, OBBCollider* send, Vector2 normal);

	bool OverlapOnAxis(const std::vector<Vector2>& poly1, const std::vector<Vector2>& poly2, Vector2 axis);

public:
	void Init();
	void Update();
	void PostUpdate();
	void Render(HDC hdc);

	void AddCollider(Collider* collider);

	vector<Collider*> GetPlacedColliders(ECollisionLayer layer) { return _colliderList[layer]; }

	bool CheckOBBHitBox(POINT OBB[4], RECT AABB);

	CollisionInfo CheckAABBGroundCollision(const RECT& playerOldRect, const RECT& playerNewRect, Collider* groundCollider);
	CollisionInfo CheckLinePlatformCollision(Vector2 playerOldPos, Vector2 playerNewPos, float playerWidth, float playerHeight, Collider* lineCollider);
	CollisionInfo CheckLineWallCollision(Vector2 playerOldPos, Vector2 playerNewPos, float playerWidth, float playerHeight, Collider* wallCollider);
	CollisionInfo CheckLineCeilingCollision(Vector2 playerOldPos, Vector2 playerNewPos, float playerWidth, float playerHeight, Collider* wallCollider);
	CollisionInfo CheckLineStairCollision(Vector2 playerOldPos, Vector2 playerNewPos, float playerWidth, float playerHeight, Collider* stairCollider, bool wasStair);

	bool LineIntersectsLineSegment(Vector2 p1, Vector2 p2, Vector2 q1, Vector2 q2, Vector2& intersection, float& t, float& crossOut);
	bool LineIntersectsWallSegment(Vector2 p1, Vector2 p2, Vector2 q1, Vector2 q2, Vector2& intersection, float& t);
	Vector2 GetClosestPointOnLineSegment(Vector2 point, Vector2 lineStart, Vector2 lineEnd);

	bool GetYOnLineAtX(const Vector2& a, const Vector2& b, float x, float& outY);
	bool GetXOnLineAtY(const Vector2& a, const Vector2& b, float y, float& outX);
};

