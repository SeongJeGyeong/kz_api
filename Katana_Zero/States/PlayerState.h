#pragma once
#include "State.h"

class Player;
class PlayerMovementComponent;

class PlayerState : public State<EPlayerState>
{
public:
	PlayerState(Player* player);
	virtual ~PlayerState() {}

protected:
	Player* _player = nullptr;
	PlayerMovementComponent* movementComponent = nullptr;

public:
	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_IDLE; };

	virtual void EnterState() override {}
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override {}
};

class PlayerState_Idle : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_Idle(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_IDLE; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_Idle_to_Run : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_Idle_to_Run(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_IDLE_TO_RUN; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_Run : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_Run(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_RUN; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_Run_to_Idle : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_Run_to_Idle(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_RUN_TO_IDLE; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_PreCrouch : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_PreCrouch(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_PRECROUCH; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_Crouch : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_Crouch(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_CROUCH; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_PostCrouch : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_PostCrouch(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_POSTCROUCH; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_Jump: public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_Jump(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_JUMP; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_Fall : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_Fall(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_FALL; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_Attack : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_Attack(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_ATTACK; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};

class PlayerState_Roll : public PlayerState
{
	using Super = PlayerState;
public:
	PlayerState_Roll(Player* player) : Super(player) {}

	virtual EPlayerState GetStateType() override { return EPlayerState::PLAYER_ROLL; }
	virtual void EnterState() override;
	virtual void UpdateState(float deltaTime) override;
	virtual void ExitState() override;
};