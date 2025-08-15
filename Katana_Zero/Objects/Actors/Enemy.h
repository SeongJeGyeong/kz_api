#pragma once
#include "Actor.h"
class Enemy : public Actor
{
	using Super = Actor;
private:

public:
	virtual void Init(Vector2 pos) override;
	virtual void Update(float deltaTime) override;
	virtual void PostUpdate(float deltaTime) override;
	virtual void Render(HDC hdc) override;
};

