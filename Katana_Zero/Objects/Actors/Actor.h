#pragma once
#include "../Object.h"

class Texture;
class Collider;

class Actor : public Object
{
	using Super = Object;
public:
	virtual ~Actor() {}

protected:
	Collider* _collider = nullptr;

public:
	virtual void Init(Vector2 pos);
	virtual void Update(float deltaTime) override;
	virtual void Render(HDC hdc) override;

	virtual void OnCollisionHit(Collider* other) {}
	virtual void OnCollisionBeginOverlap(Collider* other) {}
	virtual void OnCollisionStayOverlap(Collider* other) {}
	virtual void OnCollisionEndOverlap(Collider* other) {}
	virtual int32 GetCurrentState() { return 0; };

	Collider* GetCollider() { return _collider; }

	void CreateOBBCollider(float width, float height, float rotation, ECollisionLayer layer);
	void CreateAABBCollider(float width, float height, ECollisionLayer layer);
	void CreateLineCollider(Vector2 start, Vector2 end, ECollisionLayer layer);
};

