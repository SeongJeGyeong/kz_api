#pragma once
#include "../Object.h"
#include "../../Utils/ComponentContainer.h"

class Texture;
class Collider;

class Actor : public Object
{
	using Super = Object;
public:
	virtual ~Actor() {}

protected:
	Collider* _collider = nullptr;
	ComponentContainer _components;

public:
	virtual void Init(Vector2 pos);
	virtual void Update(float deltaTime) override;
	virtual void PostUpdate(float deltaTime) {}
	virtual void Render(HDC hdc) override;

	virtual void OnCollisionBeginOverlap(const CollisionInfo& info) {}
	virtual void OnCollisionStayOverlap(const CollisionInfo& info) {}
	virtual void OnCollisionEndOverlap(const CollisionInfo& info) {}
	virtual int32 GetCurrentState() { return 0; }

	Collider* GetCollider() { return _collider; }

	void CreateOBBCollider(float width, float height, float rotation, ECollisionLayer layer);
	void CreateAABBCollider(float width, float height, ECollisionLayer layer);
	void CreateLineCollider(Vector2 start, Vector2 end, ECollisionLayer layer);

	template <typename T>
	T* GetComponent()
	{
		return _components.GetComponent<T>();
	}

	void UpdateAttachedComponents(float deltaTime)
	{
		_components.UpdateComponents(deltaTime);
	}
};

