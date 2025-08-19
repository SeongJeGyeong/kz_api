#include "pch.h"
#include "EnemyState.h"
#include "../Objects/Actors/Enemy.h"
#include "../Components/Animator.h"
#include "../Components/EnemyMovementComponent.h"

void EnemyState::UpdateState(float deltaTime)
{
	if (_enemy == nullptr) return;
	_enemy->UpdateAttachedComponents(deltaTime);
}

///////////////// Idle /////////////////
void EnemyState_Idle::EnterState()
{
	_enemy->GetComponent<Animator>()->SetAnimation(EEnemyState::ENEMY_IDLE);
}

void EnemyState_Idle::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	fWaitTime += deltaTime;
	if (fWaitTime > 3.f)
	{
		fWaitTime = 0.f;
		_enemy->ChangeState(EEnemyState::ENEMY_TURN);
	}
}

void EnemyState_Idle::ExitState()
{
}

///////////////// Walk /////////////////
void EnemyState_Walk::EnterState()
{
	_enemy->GetComponent<Animator>()->SetAnimation(EEnemyState::ENEMY_WALK);
}

void EnemyState_Walk::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	fPatrolTime += deltaTime;
	if (fPatrolTime > 3.f)
	{
		fPatrolTime = 0.f;
		_enemy->ChangeState(EEnemyState::ENEMY_IDLE);
	}
	else
	{
		Vector2 dir = _enemy->GetFrontDir();
		_enemy->GetComponent<EnemyMovementComponent>()->SetVelocityX(dir.x * 100.f);
	}
}

void EnemyState_Walk::ExitState()
{
}

///////////////// Turn /////////////////
void EnemyState_Turn::EnterState()
{
	_enemy->GetComponent<Animator>()->SetAnimation(EEnemyState::ENEMY_TURN);
	Vector2 dir = _enemy->GetFrontDir();
	if (dir.x > 0.f)
	{
		_enemy->GetComponent<Animator>()->SetFlipped(true);
	}
	else
	{
		_enemy->GetComponent<Animator>()->SetFlipped(false);
	}
}

void EnemyState_Turn::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	Animator* animator = _enemy->GetComponent<Animator>();
	if (animator->IsAnimationFinished())
	{
		_enemy->ChangeState(EEnemyState::ENEMY_WALK);
		_enemy->TurnFrontDir();
	}
}

void EnemyState_Turn::ExitState()
{
}

///////////////// Run /////////////////
void EnemyState_Run::EnterState()
{
}

void EnemyState_Run::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
}

void EnemyState_Run::ExitState()
{
}

///////////////// Aim /////////////////
void EnemyState_Aim::EnterState()
{
}

void EnemyState_Aim::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
}

void EnemyState_Aim::ExitState()
{
}

///////////////// Hurt Fly /////////////////
void EnemyState_Hurt_Fly::EnterState()
{
}

void EnemyState_Hurt_Fly::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
}

void EnemyState_Hurt_Fly::ExitState()
{
}

///////////////// Hurt Ground /////////////////
void EnemyState_Hurt_Ground::EnterState()
{
}

void EnemyState_Hurt_Ground::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
}

void EnemyState_Hurt_Ground::ExitState()
{
}
