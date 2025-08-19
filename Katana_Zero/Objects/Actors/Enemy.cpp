#include "pch.h"
#include "Enemy.h"
#include "../../Components/Animator.h"
#include "../../Components/EffectorComponent.h"
#include "../../Components/EnemyMovementComponent.h"
#include "../../States/StateMachine.h"
#include "../../States/EnemyState.h"
#include "../../Managers/ResourceManager.h"
#include "../Camera.h"
#include "../../Components/Collider.h"
#include "../../Game/Game.h"

void Enemy::Init(Vector2 pos)
{
	Super::Init(pos);

	CreateAABBCollider(30, 70, ECollisionLayer::ENEMY);
	_components.AddComponent<Animator>();
	_components.AddComponent<EffectorComponent>();
	_components.AddComponent<EnemyMovementComponent>();

	Animator* animator = _components.GetComponent<Animator>();
	if (animator != nullptr)
	{
		animator->SetOwner(this);
		animator->InitComponent(EEnemyState::ENEMY_END);
		animator->AddAnimation(EEnemyState::ENEMY_IDLE, ResourceManager::GetInstance()->GetSprite("spr_gangsteridle"), { 0, -15 });
		animator->AddAnimation(EEnemyState::ENEMY_WALK, ResourceManager::GetInstance()->GetSprite("spr_gangsterwalk"), { 0, -5 });
		animator->AddAnimation(EEnemyState::ENEMY_TURN, ResourceManager::GetInstance()->GetSprite("spr_gangsterturn"), { 0, -10 });
		animator->AddAnimation(EEnemyState::ENEMY_RUN, ResourceManager::GetInstance()->GetSprite("spr_gangsterrun"), { 0, 0 });
		animator->AddAnimation(EEnemyState::ENEMY_AIM, ResourceManager::GetInstance()->GetSprite("spr_gangsteridle"), { 0, 0 });
		animator->AddAnimation(EEnemyState::ENEMY_HURT_FLY, ResourceManager::GetInstance()->GetSprite("spr_gangsterhurtfly"), { 0, 0 });
		animator->AddAnimation(EEnemyState::ENEMY_HURT_GROUND, ResourceManager::GetInstance()->GetSprite("spr_gangsterhurtground"), { 0, 0 });
	}

	_components.GetComponent<EnemyMovementComponent>()->InitComponent(this);

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
	_stateMachine->Update(deltaTime);
}

void Enemy::PostUpdate(float deltaTime)
{
	SetPos(_components.GetComponent<EnemyMovementComponent>()->GetNewPos());
	_components.GetComponent<EnemyMovementComponent>()->SetAcceleration({ 0, 0 });
}

void Enemy::Render(HDC hdc)
{
	_components.RenderComponents(hdc);
	Super::Render(hdc);

	HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255)); // 파란색, 2픽셀 두께
	HBRUSH hBrush = (HBRUSH)GetStockObject(HOLLOW_BRUSH); // 투명한 브러쉬

	SelectObject(hdc, hPen);
	SelectObject(hdc, hBrush);

	Vector2 screenPos = Game::GetInstance()->GetCurrentSceneCamera()->ConvertScreenPos(GetPos());

	Ellipse(hdc, screenPos.x - 500.f, screenPos.y - 500.f, screenPos.x + 500.f, screenPos.y + 500.f);

	// 사용한 객체 삭제
	DeleteObject(hPen);
	DeleteObject(hBrush);
}

void Enemy::OnCollisionBeginOverlap(const CollisionInfo& info)
{
	switch (info.collisionLayer)
	{
	case ECollisionLayer::GROUND:
		OutputDebugString(L"beginoverlapground\n");
		ProcessGroundCollision(info);
		break;
	//case ECollisionLayer::PLATFORM:
	//	ProcessPlatformCollision(info);
	//	break;
	//case ECollisionLayer::WALL:
	//	ProcessWallCollision(info);
	//	break;
	//case ECollisionLayer::CEILING:
	//	ProcessCeilingCollision(info);
	//	break;
	//case ECollisionLayer::STAIR:
	//	if (_movementComp->GetIsPlatform()) break;
	//	ProcessStairCollision(info, GetPos());
	//	break;
	default:
		break;
	}
}

void Enemy::OnCollisionStayOverlap(const CollisionInfo& info)
{
}

void Enemy::OnCollisionEndOverlap(const CollisionInfo& info)
{
}

void Enemy::ProcessGroundCollision(const CollisionInfo& collisionInfo)
{
	EnemyMovementComponent* movementComp = _components.GetComponent<EnemyMovementComponent>();

	switch (collisionInfo.hitCorner)
	{
	case 0:
		movementComp->SetNewPosY(collisionInfo.collisionPoint.y - GetCollider()->GetHeight() * 0.5f);
		movementComp->SetVelocityY(0.f);
		movementComp->SetOnGround(true);
		break;
	case 3:
		movementComp->SetNewPosY(collisionInfo.collisionPoint.y + GetCollider()->GetHeight() * 0.5f);
		movementComp->SetVelocityY(0.f);
		break;
	case 1:
		{
			float halfWidth = GetCollider()->GetWidth() * 0.5f;
			movementComp->SetNewPosX(collisionInfo.collisionPoint.x + halfWidth);
			movementComp->SetVelocityX(0.f);
			break;
		}
	case 2: 
		{
			float halfWidth = GetCollider()->GetWidth() * 0.5f;
			movementComp->SetNewPosX(collisionInfo.collisionPoint.x - halfWidth);
			movementComp->SetVelocityX(0.f);
			break;
		}
	}
}

void Enemy::ChangeState(EEnemyState stateType)
{
	_stateMachine->ChangeState(stateType);
}