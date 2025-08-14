#include "pch.h"
#include "PlayerState.h"
#include "../Objects/Actors/Player.h"
#include "../Components/Animator.h"
#include "../Managers/InputManager.h"

void PlayerState::UpdateState(Actor* owner, float deltaTime)
{
	Player* player = static_cast<Player*>(owner);
	player->UpdateAttachedComponents(deltaTime);
	if (!player->GetIsGround() && !player->GetIsPlatform() && !player->GetOnStair() && 
		static_cast<Player*>(owner)->GetVelocity().y > 0)
	{
		static_cast<Player*>(owner)->ChangeState(EPlayerState::PLAYER_FALL);
	}
}

///////* Idle *///////////////////////////////
void PlayerState_Idle::EnterState(Actor* owner)
{
	Animator* animator = static_cast<Player*>(owner)->GetComponent<Animator>();
	animator->SetAnimation(EPlayerState::PLAYER_IDLE);
}

void PlayerState_Idle::UpdateState(Actor* owner, float deltaTime)
{
	Super::UpdateState(owner, deltaTime);
}

void PlayerState_Idle::ExitState(Actor* owner)
{

}
//////////////////////////////////////////////

///////* Idle to Run *////////////////////////
void PlayerState_Idle_to_Run::EnterState(Actor* owner)
{
	Animator* animator = static_cast<Player*>(owner)->GetComponent<Animator>();
	animator->SetAnimation(EPlayerState::PLAYER_IDLE_TO_RUN);
}

void PlayerState_Idle_to_Run::UpdateState(Actor* owner, float deltaTime)
{
	Super::UpdateState(owner, deltaTime);
	Animator* animator = static_cast<Player*>(owner)->GetComponent<Animator>();
	if (animator->IsAnimationFinished())
	{
		static_cast<Player*>(owner)->ChangeState(EPlayerState::PLAYER_RUN);
	}
}

void PlayerState_Idle_to_Run::ExitState(Actor* owner)
{

}
//////////////////////////////////////////////

///////* Run *////////////////////////////////
void PlayerState_Run::EnterState(Actor* owner)
{
	Animator* animator = static_cast<Player*>(owner)->GetComponent<Animator>();
	animator->SetAnimation(EPlayerState::PLAYER_RUN);
}

void PlayerState_Run::UpdateState(Actor* owner, float deltaTime)
{
	Super::UpdateState(owner, deltaTime);
	Vector2 acceleration = static_cast<Player*>(owner)->GetAcceleration();
	if (acceleration.x == 0)
	{
		static_cast<Player*>(owner)->ChangeState(EPlayerState::PLAYER_RUN_TO_IDLE);
	}
}

void PlayerState_Run::ExitState(Actor* owner)
{

}
//////////////////////////////////////////////

///////* Run to Idle *////////////////////////
void PlayerState_Run_to_Idle::EnterState(Actor* owner)
{
	Animator* animator = static_cast<Player*>(owner)->GetComponent<Animator>();
	animator->SetAnimation(EPlayerState::PLAYER_RUN_TO_IDLE);
}

void PlayerState_Run_to_Idle::UpdateState(Actor* owner, float deltaTime)
{
	Super::UpdateState(owner, deltaTime);
	Player* player = static_cast<Player*>(owner);
	if (player->GetIsGround() || player->GetIsPlatform() || player->GetOnStair())
	{
		if (InputManager::GetInstance()->GetButtonPressed(KeyType::A) || InputManager::GetInstance()->GetButtonPressed(KeyType::D))
		{
			player->ChangeState(EPlayerState::PLAYER_IDLE_TO_RUN);
		}
	}
	Animator* animator = static_cast<Player*>(owner)->GetComponent<Animator>();
	if (animator->IsAnimationFinished())
	{
		static_cast<Player*>(owner)->ChangeState(EPlayerState::PLAYER_IDLE);
	}
}

void PlayerState_Run_to_Idle::ExitState(Actor* owner)
{

}
//////////////////////////////////////////////

///////* PreCrouch *////////////////////////
void PlayerState_PreCrouch::EnterState(Actor* owner)
{
	Animator* animator = static_cast<Player*>(owner)->GetComponent<Animator>();
	animator->SetAnimation(EPlayerState::PLAYER_PRECROUCH);
}

void PlayerState_PreCrouch::UpdateState(Actor* owner, float deltaTime)
{
	Super::UpdateState(owner, deltaTime);
	Animator* animator = static_cast<Player*>(owner)->GetComponent<Animator>();
	if (animator->IsAnimationFinished())
	{
		static_cast<Player*>(owner)->ChangeState(EPlayerState::PLAYER_CROUCH);
	}
}

void PlayerState_PreCrouch::ExitState(Actor* owner)
{
}
//////////////////////////////////////////////

///////* Crouch *////////////////////////
void PlayerState_Crouch::EnterState(Actor* owner)
{
	Animator* animator = static_cast<Player*>(owner)->GetComponent<Animator>();
	animator->SetAnimation(EPlayerState::PLAYER_CROUCH);
}

void PlayerState_Crouch::UpdateState(Actor* owner, float deltaTime)
{
	Super::UpdateState(owner, deltaTime);
}

void PlayerState_Crouch::ExitState(Actor* owner)
{
}
//////////////////////////////////////////////

///////* PostCrouch *////////////////////////
void PlayerState_PostCrouch::EnterState(Actor* owner)
{
	Animator* animator = static_cast<Player*>(owner)->GetComponent<Animator>();
	animator->SetAnimation(EPlayerState::PLAYER_POSTCROUCH);
}

void PlayerState_PostCrouch::UpdateState(Actor* owner, float deltaTime)
{
	Super::UpdateState(owner, deltaTime);
	Animator* animator = static_cast<Player*>(owner)->GetComponent<Animator>();
	if (animator->IsAnimationFinished())
	{
		static_cast<Player*>(owner)->ChangeState(EPlayerState::PLAYER_IDLE);
	}
}

void PlayerState_PostCrouch::ExitState(Actor* owner)
{
}
//////////////////////////////////////////////

///////* Jump *////////////////////////
void PlayerState_Jump::EnterState(Actor* owner)
{
	Animator* animator = static_cast<Player*>(owner)->GetComponent<Animator>();
	animator->SetAnimation(EPlayerState::PLAYER_JUMP);
}

void PlayerState_Jump::UpdateState(Actor* owner, float deltaTime)
{
	Super::UpdateState(owner, deltaTime);
	Player* player = static_cast<Player*>(owner);
	if (player->GetIsGround() || player->GetIsPlatform() || player->GetOnStair())
	{
		static_cast<Player*>(owner)->ChangeState(EPlayerState::PLAYER_RUN_TO_IDLE);
	}
}

void PlayerState_Jump::ExitState(Actor* owner)
{
}
//////////////////////////////////////////////

///////* Fall *////////////////////////
void PlayerState_Fall::EnterState(Actor* owner)
{
	Animator* animator = static_cast<Player*>(owner)->GetComponent<Animator>();
	animator->SetAnimation(EPlayerState::PLAYER_FALL);
}

void PlayerState_Fall::UpdateState(Actor* owner, float deltaTime)
{
	Super::UpdateState(owner, deltaTime);
	if (!static_cast<Player*>(owner)->GetIsJumped())
	{
		static_cast<Player*>(owner)->ChangeState(EPlayerState::PLAYER_RUN_TO_IDLE);
	}
}

void PlayerState_Fall::ExitState(Actor* owner)
{
}
//////////////////////////////////////////////