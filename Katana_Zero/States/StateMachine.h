#pragma once
#include "State.h"

class Actor;

class StateMachine
{
public:
	StateMachine(Actor* owner) : _owner(owner) {}
	~StateMachine(){}

private:
	Actor* _owner = nullptr;

	State* _currentState = nullptr;

	int32 iCurrentStateType = -1;
	map<int32, State*> _stateMap;

public:
	void Update(float deltaTime);
	void AddState(State* state);
	void ChangeState(int32 stateType);
	EPlayerState GetCurrentStateType() 
	{
		if(_currentState) return (EPlayerState)_currentState->GetStateType();

		return EPlayerState::PLAYER_END;
	}
};

