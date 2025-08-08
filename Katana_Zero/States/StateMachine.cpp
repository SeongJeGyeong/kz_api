#include "pch.h"
#include "StateMachine.h"
#include "../Objects/Actors/Actor.h"

void StateMachine::Update(float deltaTime)
{
	if (_currentState) _currentState->UpdateState(_owner, deltaTime);
}

void StateMachine::AddState(State* state)
{
	if (state == nullptr) return;
	// 이미 맵에 존재함
	if (_stateMap.find(state->GetStateType()) != _stateMap.end()) return;

	_stateMap.emplace(state->GetStateType(), state);
}

void StateMachine::ChangeState(int32 stateType)
{
	if (_currentState && _currentState->GetStateType() == stateType) return;

	if (_currentState) _currentState->ExitState(_owner);

	auto it = _stateMap.find(stateType);
	if (it == _stateMap.end()) return;

	_currentState = it->second;
	_currentState->EnterState(_owner);
}
