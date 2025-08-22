#include "pch.h"
#include "PlayerState.h"
#include "../Objects/Actors/Player.h"
#include "../Managers/InputManager.h"
#include "../Components/Animator.h"
#include "../Components/InputComponent.h"
#include "../Components/PlayerMovementComponent.h"
#include "../Components/EffectorComponent.h"

PlayerState::PlayerState(Player* player)
	: _player(player) 
{
	_movementComponent = player->GetComponent<PlayerMovementComponent>();
	_animator = player->GetComponent<Animator>();
}

void PlayerState::UpdateState(float deltaTime)
{
	if (_player == nullptr) return;
	_player->UpdateAttachedComponents(deltaTime);

	if (!_movementComponent->GetOnGround() && !_movementComponent->GetIsPlatform() && !_movementComponent->GetOnStair())
	{
		_movementComponent->SetIsJumped(true);

		if (_movementComponent->GetVelocity().y > 0.f && 
			(	_player->GetCurrentState() != EPlayerState::PLAYER_ATTACK && 
				_player->GetCurrentState() != EPlayerState::PLAYER_HURT_BEGIN &&
				_player->GetCurrentState() != EPlayerState::PLAYER_HURT_LOOP
			)
		)
		{
			_player->ChangeState(EPlayerState::PLAYER_FALL);
			_player->SetIsCrouch(false);
		}
	}
}

///////* Idle *///////////////////////////////
void PlayerState_Idle::EnterState()
{
	_animator->SetAnimation(EPlayerState::PLAYER_IDLE);
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
	_animator->SetAnimation(EPlayerState::PLAYER_IDLE_TO_RUN);
}

void PlayerState_Idle_to_Run::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	_player->GetComponent<InputComponent>()->Input_In_IdleToRun(deltaTime);

	if (_animator->IsAnimationFinished())
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
	_animator->SetAnimation(EPlayerState::PLAYER_RUN);
}

void PlayerState_Run::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	_player->GetComponent<InputComponent>()->Input_In_Run(deltaTime);

	Vector2 acceleration = _movementComponent->GetAcceleration();
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
	_animator->SetAnimation(EPlayerState::PLAYER_RUN_TO_IDLE);
}

void PlayerState_Run_to_Idle::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	_player->GetComponent<InputComponent>()->Input_In_RunToIdle(deltaTime);

	if (_movementComponent->GetOnGround() || _movementComponent->GetIsPlatform() || _movementComponent->GetOnStair())
	{
		if (InputManager::GetInstance()->GetButtonPressed(KeyType::A) || InputManager::GetInstance()->GetButtonPressed(KeyType::D))
		{
			_player->ChangeState(EPlayerState::PLAYER_IDLE_TO_RUN);
		}
	}

	if (_animator->IsAnimationFinished())
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
	_animator->SetAnimation(EPlayerState::PLAYER_PRECROUCH);
}

void PlayerState_PreCrouch::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	_player->GetComponent<InputComponent>()->Input_In_PreCrouch(deltaTime);

	if (_animator->IsAnimationFinished())
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
	_animator->SetAnimation(EPlayerState::PLAYER_CROUCH);
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
	_animator->SetAnimation(EPlayerState::PLAYER_POSTCROUCH);
}

void PlayerState_PostCrouch::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	_player->GetComponent<InputComponent>()->Input_In_PostCrouch(deltaTime);

	if (_animator->IsAnimationFinished())
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
	_animator->SetAnimation(EPlayerState::PLAYER_JUMP);
	_player->SetIsCrouch(false);
}

void PlayerState_Jump::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	_player->GetComponent<InputComponent>()->Input_In_Jump(deltaTime);

	if (_movementComponent->GetOnGround() || _movementComponent->GetIsPlatform() || _movementComponent->GetOnStair())
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
	_animator->SetAnimation(EPlayerState::PLAYER_FALL);
	_player->SetIsCrouch(false);
}

void PlayerState_Fall::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	_player->GetComponent<InputComponent>()->Input_In_Fall(deltaTime);

	if (!_movementComponent->GetIsJumped())
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
	_animator->SetAnimation(EPlayerState::PLAYER_ATTACK);
}

void PlayerState_Attack::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	_player->GetComponent<InputComponent>()->Input_In_Attack(deltaTime);

	if (_animator->IsAnimationFinished())
	{
		if (!_movementComponent->GetIsJumped())
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
	_player->SetIsAttack(false);
	_player->ClearHitActors();
}
//////////////////////////////////////////////

///////* Roll *////////////////////////
void PlayerState_Roll::EnterState()
{
	_animator->SetAnimation(EPlayerState::PLAYER_ROLL);
	_player->SetInvincible(true);
}

void PlayerState_Roll::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	_player->GetComponent<InputComponent>()->Input_In_Roll(deltaTime);

	if (_animator->IsAnimationFinished())
	{
		_player->ChangeState(EPlayerState::PLAYER_IDLE);
	}
}

void PlayerState_Roll::ExitState()
{
	_player->SetInvincible(false);
}
//////////////////////////////////////////////

///////* Hurt Begin *////////////////////////
void PlayerState_HurtBegin::EnterState()
{
	_animator->SetAnimation(EPlayerState::PLAYER_HURT_BEGIN);
}

void PlayerState_HurtBegin::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	if (_animator->IsAnimationFinished())
	{
		_player->ChangeState(EPlayerState::PLAYER_HURT_LOOP);
	}
}

void PlayerState_HurtBegin::ExitState()
{
}

///////* Hurt Fly *////////////////////////
void PlayerState_HurtFly::EnterState()
{
	_animator->SetAnimation(EPlayerState::PLAYER_HURT_LOOP);
}

void PlayerState_HurtFly::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	if (_movementComponent->GetOnGround())
	{
		_player->ChangeState(EPlayerState::PLAYER_HURT_GROUND);
	}

}

void PlayerState_HurtFly::ExitState()
{
}

///////* Hurt Ground *////////////////////////
void PlayerState_HurtGround::EnterState()
{
	_animator->SetAnimation(EPlayerState::PLAYER_HURT_GROUND);
}

void PlayerState_HurtGround::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	if (_player->GetIsDead()) return;

	fRecoverTime += deltaTime;
	if (fRecoverTime >= 0.5f)
	{
		fRecoverTime = 0.f;
		_player->ChangeState(EPlayerState::PLAYER_HURT_RECOVER);
	}
}

void PlayerState_HurtGround::ExitState()
{
}

///////* Hurt Recover *////////////////////////
void PlayerState_Recover::EnterState()
{
	_animator->SetAnimation(EPlayerState::PLAYER_HURT_RECOVER);
}

void PlayerState_Recover::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	if (_animator->IsAnimationFinished())
	{
		_player->ChangeState(EPlayerState::PLAYER_IDLE);
	}
}

void PlayerState_Recover::ExitState()
{
}

///////* Struggle *////////////////////////
void PlayerState_Struggle::EnterState()
{
	_movementComponent->SetVelocity({ 0, 0 });
	_movementComponent->SetNewPos(_player->GetPos());
	_animator->SetAnimation(EPlayerState::PLAYER_HURT_BEGIN);
	_player->GetComponent<EffectorComponent>()->ClearSpawnList();
}

void PlayerState_Struggle::UpdateState(float deltaTime)
{
}

void PlayerState_Struggle::ExitState()
{
}

///////* Finish *////////////////////////
void PlayerState_Finish::EnterState()
{
	_movementComponent->SetNewPos(_player->GetPos());
	_animator->SetAnimation(EPlayerState::PLAYER_IDLE);
}

void PlayerState_Finish::UpdateState(float deltaTime)
{
}

void PlayerState_Finish::ExitState()
{
}