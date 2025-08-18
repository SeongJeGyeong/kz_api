#pragma once
#include "Actor.h"

template<typename EnumType>
class StateMachine;

class Enemy : public Actor
{
	using Super = Actor;
private:
	StateMachine<EEnemyState>* _stateMachine;

public:
	virtual void Init(Vector2 pos) override;
	virtual void Update(float deltaTime) override;
	virtual void PostUpdate(float deltaTime) override;
	virtual void Render(HDC hdc) override;
};

