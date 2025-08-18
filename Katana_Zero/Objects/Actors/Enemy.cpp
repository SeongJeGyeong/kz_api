#include "pch.h"
#include "Enemy.h"
#include "../../Components/Animator.h"
#include "../../Components/EffectorComponent.h"
#include "../../States/StateMachine.h"
#include "../../States/EnemyState.h"

void Enemy::Init(Vector2 pos)
{
	Super::Init(pos);

	CreateAABBCollider(30, 50, ECollisionLayer::ENEMY);
	_components.AddComponent<Animator>();
	_components.AddComponent<EffectorComponent>();

	Animator* animator = _components.GetComponent<Animator>();
	if (animator != nullptr)
	{
		animator->SetOwner(this);
		animator->InitComponent(EEnemyState::ENEMY_END);
	}

	_stateMachine = new StateMachine<EEnemyState>(this);
	_stateMachine->AddState(new EnemyState_Idle(this));
	_stateMachine->AddState(new EnemyState_Walk(this));
	_stateMachine->AddState(new EnemyState_Turn(this));
	_stateMachine->AddState(new EnemyState_Run(this));
	_stateMachine->AddState(new EnemyState_Aim(this));
	_stateMachine->AddState(new EnemyState_Hurt_Fly(this));
	_stateMachine->AddState(new EnemyState_Hurt_Ground(this));
	_stateMachine->ChangeState(EEnemyState::ENEMY_IDLE);
}

void Enemy::Update(float deltaTime)
{

}

void Enemy::PostUpdate(float deltaTime)
{
}

void Enemy::Render(HDC hdc)
{
	Super::Render(hdc);
}