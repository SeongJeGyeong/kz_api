#pragma once
#include "State.h"

class PlayerState : public State
{
public:
	virtual ~PlayerState() {}

	virtual int32 GetStateType() override { return -1; };

	virtual void EnterState(Actor* owner) override {}
	virtual void UpdateState(Actor* owner, float deltaTime) override;
	virtual void ExitState(Actor* owner) override {}
};

class PlayerState_Idle : public PlayerState
{
	using Super = PlayerState;
public:
	virtual int32 GetStateType() override { return EPlayerState::PLAYER_IDLE; }
	virtual void EnterState(Actor* owner) override;
	virtual void UpdateState(Actor* owner, float deltaTime) override;
	virtual void ExitState(Actor* owner) override;
};

class PlayerState_Idle_to_Run : public PlayerState
{
	using Super = PlayerState;
public:
	virtual int32 GetStateType() override { return EPlayerState::PLAYER_IDLE_TO_RUN; }
	virtual void EnterState(Actor* owner) override;
	virtual void UpdateState(Actor* owner, float deltaTime) override;
	virtual void ExitState(Actor* owner) override;
};

class PlayerState_Run : public PlayerState
{
	using Super = PlayerState;
public:
	virtual int32 GetStateType() override { return EPlayerState::PLAYER_RUN; }
	virtual void EnterState(Actor* owner) override;
	virtual void UpdateState(Actor* owner, float deltaTime) override;
	virtual void ExitState(Actor* owner) override;
};

class PlayerState_Run_to_Idle : public PlayerState
{
	using Super = PlayerState;
public:
	virtual int32 GetStateType() override { return EPlayerState::PLAYER_RUN_TO_IDLE; }
	virtual void EnterState(Actor* owner) override;
	virtual void UpdateState(Actor* owner, float deltaTime) override;
	virtual void ExitState(Actor* owner) override;
};

class PlayerState_PreCrouch : public PlayerState
{
	using Super = PlayerState;
public:
	virtual int32 GetStateType() override { return EPlayerState::PLAYER_PRECROUCH; }
	virtual void EnterState(Actor* owner) override;
	virtual void UpdateState(Actor* owner, float deltaTime) override;
	virtual void ExitState(Actor* owner) override;
};

class PlayerState_Crouch : public PlayerState
{
	using Super = PlayerState;
public:
	virtual int32 GetStateType() override { return EPlayerState::PLAYER_CROUCH; }
	virtual void EnterState(Actor* owner) override;
	virtual void UpdateState(Actor* owner, float deltaTime) override;
	virtual void ExitState(Actor* owner) override;
};

class PlayerState_PostCrouch : public PlayerState
{
	using Super = PlayerState;
public:
	virtual int32 GetStateType() override { return EPlayerState::PLAYER_POSTCROUCH; }
	virtual void EnterState(Actor* owner) override;
	virtual void UpdateState(Actor* owner, float deltaTime) override;
	virtual void ExitState(Actor* owner) override;
};

class PlayerState_Jump: public PlayerState
{
	using Super = PlayerState;
public:
	virtual int32 GetStateType() override { return EPlayerState::PLAYER_JUMP; }
	virtual void EnterState(Actor* owner) override;
	virtual void UpdateState(Actor* owner, float deltaTime) override;
	virtual void ExitState(Actor* owner) override;
};

class PlayerState_Fall : public PlayerState
{
	using Super = PlayerState;
public:
	virtual int32 GetStateType() override { return EPlayerState::PLAYER_FALL; }
	virtual void EnterState(Actor* owner) override;
	virtual void UpdateState(Actor* owner, float deltaTime) override;
	virtual void ExitState(Actor* owner) override;
};