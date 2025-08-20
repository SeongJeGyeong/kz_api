#include "pch.h"
#include "PlayerState.h"
#include "../Objects/Actors/Player.h"
#include "../Components/Animator.h"
#include "../Managers/InputManager.h"
#include "../Components/InputComponent.h"
#include "../Components/PlayerMovementComponent.h"

PlayerState::PlayerState(Player* player)
	: _player(player) 
{
	movementComponent = player->GetComponent<PlayerMovementComponent>();
}

void PlayerState::UpdateState(float deltaTime)
{
	if (_player == nullptr) return;
	_player->UpdateAttachedComponents(deltaTime);

	if (!movementComponent->GetIsGround() && !movementComponent->GetIsPlatform() && !movementComponent->GetOnStair())
	{
		movementComponent->SetIsJumped(true);

		if (movementComponent->GetVelocity().y > 0.f && _player->GetCurrentState() != EPlayerState::PLAYER_ATTACK)
		{
			_player->ChangeState(EPlayerState::PLAYER_FALL);
			_player->SetIsCrouch(false);
		}
	}
}

///////* Idle *///////////////////////////////
void PlayerState_Idle::EnterState()
{
	Animator* animator = _player->GetComponent<Animator>();
	animator->SetAnimation(EPlayerState::PLAYER_IDLE);
}

void PlayerState_Idle::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	_player->GetComponent<InputComponent>()->Input_In_Idle(deltaTime);
}

void PlayerState_Idle::ExitState()
{

}
//////////////////////////////////////////////

///////* Idle to Run *////////////////////////
void PlayerState_Idle_to_Run::EnterState()
{
	Animator* animator = _player->GetComponent<Animator>();
	animator->SetAnimation(EPlayerState::PLAYER_IDLE_TO_RUN);
}

void PlayerState_Idle_to_Run::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	_player->GetComponent<InputComponent>()->Input_In_IdleToRun(deltaTime);

	Animator* animator = _player->GetComponent<Animator>();
	if (animator->IsAnimationFinished())
	{
		_player->ChangeState(EPlayerState::PLAYER_RUN);
	}
}

void PlayerState_Idle_to_Run::ExitState()
{

}
//////////////////////////////////////////////

///////* Run *////////////////////////////////
void PlayerState_Run::EnterState()
{
	Animator* animator = _player->GetComponent<Animator>();
	animator->SetAnimation(EPlayerState::PLAYER_RUN);
}

void PlayerState_Run::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	_player->GetComponent<InputComponent>()->Input_In_Run(deltaTime);

	Vector2 acceleration = movementComponent->GetAcceleration();
	if (acceleration.x == 0)
	{
		_player->ChangeState(EPlayerState::PLAYER_RUN_TO_IDLE);
	}
}

void PlayerState_Run::ExitState()
{

}
//////////////////////////////////////////////

///////* Run to Idle *////////////////////////
void PlayerState_Run_to_Idle::EnterState()
{
	Animator* animator = _player->GetComponent<Animator>();
	animator->SetAnimation(EPlayerState::PLAYER_RUN_TO_IDLE);
}

void PlayerState_Run_to_Idle::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	_player->GetComponent<InputComponent>()->Input_In_RunToIdle(deltaTime);

	if (movementComponent->GetIsGround() || movementComponent->GetIsPlatform() || movementComponent->GetOnStair())
	{
		if (InputManager::GetInstance()->GetButtonPressed(KeyType::A) || InputManager::GetInstance()->GetButtonPressed(KeyType::D))
		{
			_player->ChangeState(EPlayerState::PLAYER_IDLE_TO_RUN);
		}
	}
	Animator* animator = _player->GetComponent<Animator>();
	if (animator->IsAnimationFinished())
	{
		_player->ChangeState(EPlayerState::PLAYER_IDLE);
	}
}

void PlayerState_Run_to_Idle::ExitState()
{

}
//////////////////////////////////////////////

///////* PreCrouch *////////////////////////
void PlayerState_PreCrouch::EnterState()
{
	Animator* animator = _player->GetComponent<Animator>();
	animator->SetAnimation(EPlayerState::PLAYER_PRECROUCH);
}

void PlayerState_PreCrouch::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	_player->GetComponent<InputComponent>()->Input_In_PreCrouch(deltaTime);

	Animator* animator = _player->GetComponent<Animator>();
	if (animator->IsAnimationFinished())
	{
		_player->ChangeState(EPlayerState::PLAYER_CROUCH);
	}
}

void PlayerState_PreCrouch::ExitState()
{
}
//////////////////////////////////////////////

///////* Crouch *////////////////////////
void PlayerState_Crouch::EnterState()
{
	Animator* animator = _player->GetComponent<Animator>();
	animator->SetAnimation(EPlayerState::PLAYER_CROUCH);
}

void PlayerState_Crouch::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	_player->GetComponent<InputComponent>()->Input_In_Crouch(deltaTime);
}

void PlayerState_Crouch::ExitState()
{
	_player->SetIsCrouch(false);
}
//////////////////////////////////////////////

///////* PostCrouch *////////////////////////
void PlayerState_PostCrouch::EnterState()
{
	Animator* animator = _player->GetComponent<Animator>();
	animator->SetAnimation(EPlayerState::PLAYER_POSTCROUCH);
}

void PlayerState_PostCrouch::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	_player->GetComponent<InputComponent>()->Input_In_PostCrouch(deltaTime);

	Animator* animator = _player->GetComponent<Animator>();
	if (animator->IsAnimationFinished())
	{
		_player->ChangeState(EPlayerState::PLAYER_IDLE);
	}
}

void PlayerState_PostCrouch::ExitState()
{
}
//////////////////////////////////////////////

///////* Jump *////////////////////////
void PlayerState_Jump::EnterState()
{
	Animator* animator = _player->GetComponent<Animator>();
	animator->SetAnimation(EPlayerState::PLAYER_JUMP);
	_player->SetIsCrouch(false);
}

void PlayerState_Jump::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	_player->GetComponent<InputComponent>()->Input_In_Jump(deltaTime);

	if (movementComponent->GetIsGround() || movementComponent->GetIsPlatform() || movementComponent->GetOnStair())
	{
		_player->ChangeState(EPlayerState::PLAYER_RUN_TO_IDLE);
	}
}

void PlayerState_Jump::ExitState()
{
}
//////////////////////////////////////////////

///////* Fall *////////////////////////
void PlayerState_Fall::EnterState()
{
	Animator* animator = _player->GetComponent<Animator>();
	animator->SetAnimation(EPlayerState::PLAYER_FALL);
	_player->SetIsCrouch(false);
}

void PlayerState_Fall::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	_player->GetComponent<InputComponent>()->Input_In_Fall(deltaTime);

	if (!movementComponent->GetIsJumped())
	{
		_player->ChangeState(EPlayerState::PLAYER_RUN_TO_IDLE);
	}
}

void PlayerState_Fall::ExitState()
{
}
//////////////////////////////////////////////

///////* Attack *////////////////////////
void PlayerState_Attack::EnterState()
{
	Animator* animator = _player->GetComponent<Animator>();
	animator->SetAnimation(EPlayerState::PLAYER_ATTACK);
}

void PlayerState_Attack::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	_player->GetComponent<InputComponent>()->Input_In_Attack(deltaTime);

	Animator* animator = _player->GetComponent<Animator>();
	if (animator->IsAnimationFinished())
	{
		_player->SetIsAttack(false);
		_player->ClearHitActors();
		if (!movementComponent->GetIsJumped())
		{
			_player->ChangeState(EPlayerState::PLAYER_IDLE);
		}
		else
		{
			_player->ChangeState(EPlayerState::PLAYER_FALL);
		}
	}
}

void PlayerState_Attack::ExitState()
{
}
//////////////////////////////////////////////

///////* Roll *////////////////////////
void PlayerState_Roll::EnterState()
{
	Animator* animator = _player->GetComponent<Animator>();
	animator->SetAnimation(EPlayerState::PLAYER_ROLL);
}

void PlayerState_Roll::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	_player->GetComponent<InputComponent>()->Input_In_Roll(deltaTime);

	Animator* animator = _player->GetComponent<Animator>();
	if (animator->IsAnimationFinished())
	{
		_player->ChangeState(EPlayerState::PLAYER_IDLE);
	}
}

void PlayerState_Roll::ExitState()
{
}
//////////////////////////////////////////////