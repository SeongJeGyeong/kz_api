#pragma once
#include "Actor.h"

class Sprite;

class Axe : public Actor
{
	using Super = Actor;
private:
	float fLifeTime = 0.f;
	Vector2 vDirection;
	float fRadian;
	Sprite* _sprite = nullptr;
	Vector2 vNewPos;
	Vector2 vOwnerPos;

	bool bActive = false;
	bool bThrowOrSwing = true;

public:
	virtual void Init(Vector2 pos, Vector2 ownerPos, Vector2 dir, bool throwOrSwing);
	virtual void Update(float deltaTime) override;
	virtual void PostUpdate(float deltaTime) override;
	virtual void Render(HDC hdc) override;

	virtual void TakeDamage(Actor* damageCauser, const Vector2& attackDirection) override;
	void SetActive(bool active) { bActive = active; }
};

