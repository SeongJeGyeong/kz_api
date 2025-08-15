#pragma once

class Actor;

class State
{
public:
	virtual ~State() {}

	virtual int32 GetStateType() = 0;
	virtual void EnterState() = 0;
	virtual void UpdateState(float deltaTime) = 0;
	virtual void ExitState() = 0;
};

