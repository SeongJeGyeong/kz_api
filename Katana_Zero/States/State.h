#pragma once

class Actor;

class State
{
public:
	virtual ~State() {}

	virtual int32 GetStateType() = 0;
	virtual void EnterState(Actor* owner) = 0;
	virtual void UpdateState(Actor* owner, float deltaTime) = 0;
	virtual void ExitState(Actor* owner) = 0;
};

