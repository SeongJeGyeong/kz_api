#include "pch.h"
#include "PlayerState.h"
#include "../Objects/Actors/Player.h"
#include "../Components/Animator.h"

void PlayerState::UpdateState(Actor* owner, float deltaTime)
{
	static_cast<Player*>(owner)->UpdateAttachedComponents(deltaTime);
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