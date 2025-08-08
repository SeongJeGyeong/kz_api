#pragma once
#include "../Utils/Singleton.h"

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

	void ExecuteCollisionFunc(Collider* receive, Collider* send);

public:
	void Init();
	void Update();
	void PostUpdate();
	void Render(HDC hdc);

	void AddCollider(Collider* collider);

	vector<Collider*> GetPlacedColliders(ECollisionLayer layer) { return _colliderList[layer]; }
	bool CheckBlockedCollision(Player* player, Vector2 start, Vector2 end, CollisionInfo& info);
	bool CheckGroundCollision(Player* player, Vector2 start, Vector2 end, CollisionInfo& info);


	PlayerGroundCollisionResult CheckPlayerGroundCollision(Player* player, Vector2 playerOldPos, Vector2 playerNewPos);
	PlayerGroundCollisionResult CheckAABBGroundCollision(const RECT& playerOldRect, const RECT& playerNewRect, Collider* groundCollider);
	PlayerGroundCollisionResult CheckLineGroundCollision(Vector2 playerOldPos, Vector2 playerNewPos, float playerWidth, float playerHeight, Collider* lineCollider);
	bool LineIntersectsLineSegment(Vector2 p1, Vector2 p2, Vector2 q1, Vector2 q2, Vector2& intersection, float& t);
};

