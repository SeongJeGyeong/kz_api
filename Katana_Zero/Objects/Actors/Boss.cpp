#include "pch.h"
#include "Boss.h"
#include "../../Components/Animator.h"
#include "../../Components/EffectorComponent.h"
#include "../../Components/EnemyMovementComponent.h"
#include "../../Managers/ResourceManager.h"
#include "../../States/StateMachine.h"
#include "../../States/BossState.h"
#include "../../Components/Collider.h"
#include "Player.h"
#include "../../Resources/Texture.h"
#include "../../Managers/InputManager.h"
#include "../../Managers/CollisionManager.h"

void Boss::Init(Vector2 pos)
{
	Super::Init(pos);

	CreateAABBCollider(50, 100, ECollisionLayer::ENEMY);
	_components.AddComponent<Animator>();
	_components.AddComponent<EffectorComponent>();
	_components.AddComponent<EnemyMovementComponent>();

	Animator* animator = _components.GetComponent<Animator>();
	if (animator != nullptr)
	{
		animator->SetOwner(this);
		animator->InitComponent(EBossState::BOSS_END);
		animator->AddAnimation(EBossState::BOSS_READY, ResourceManager::GetInstance()->GetSprite("spr_kissyface_casual_idle"), { 0, 0 });
		animator->AddAnimation(EBossState::BOSS_TOBATTLE, ResourceManager::GetInstance()->GetSprite("spr_kissyface_tobattle"), {-12, -14});
		animator->AddAnimation(EBossState::BOSS_IDLE, ResourceManager::GetInstance()->GetSprite("spr_kissyface_idle"), {0, -1});
		animator->AddAnimation(EBossState::BOSS_BLOCK, ResourceManager::GetInstance()->GetSprite("spr_kissyface_block"), { 0, -1 });
		animator->AddAnimation(EBossState::BOSS_HURT, ResourceManager::GetInstance()->GetSprite("spr_kissyface_hurt"), { 10, 3 });
		animator->AddAnimation(EBossState::BOSS_RECOVER, ResourceManager::GetInstance()->GetSprite("spr_kissyface_recover"), { 14, 3 });
		animator->AddAnimation(EBossState::BOSS_STRUGGLE, ResourceManager::GetInstance()->GetSprite("spr_kissyface_struggle"), { 0, 13 });
		animator->AddAnimation(EBossState::BOSS_FINISHED, ResourceManager::GetInstance()->GetSprite("spr_kissyface_die"), { 21, -7 });
		animator->AddAnimation(EBossState::BOSS_DEFEAT, ResourceManager::GetInstance()->GetSprite("spr_kissyface_dead"), { 16, 23 });
		animator->AddAnimation(EBossState::BOSS_DIE, ResourceManager::GetInstance()->GetSprite("spr_kissyface_nohead"), { 16, 29 });
		animator->AddAnimation(EBossState::BOSS_PRELUNGE, ResourceManager::GetInstance()->GetSprite("spr_kissyface_prelunge"), { 16, 5 });
		animator->AddAnimation(EBossState::BOSS_LUNGE, ResourceManager::GetInstance()->GetSprite("spr_kissyface_lunge"), { 50, 0 });
		animator->AddAnimation(EBossState::BOSS_LUNGEATTACK, ResourceManager::GetInstance()->GetSprite("spr_kissyface_lungeattack"), { 50, -5 });
		animator->AddAnimation(EBossState::BOSS_PREJUMP, ResourceManager::GetInstance()->GetSprite("spr_kissyface_prejump"), { 0, 0 });
		animator->AddAnimation(EBossState::BOSS_JUMP, ResourceManager::GetInstance()->GetSprite("spr_kissyface_jump"), { 0, 0 });
		animator->AddAnimation(EBossState::BOSS_JUMPATTACK, ResourceManager::GetInstance()->GetSprite("spr_kissyface_jump_swing"), { 0, 0 });
		animator->AddAnimation(EBossState::BOSS_LAND, ResourceManager::GetInstance()->GetSprite("spr_kissyface_landattack"), { 0, 0 });

		animator->AddAnimation(EBossState::BOSS_THROWAXE, ResourceManager::GetInstance()->GetSprite("spr_kissyface_throw"), { 2, -5 });
		animator->AddAnimation(EBossState::BOSS_TUGAXE, ResourceManager::GetInstance()->GetSprite("spr_kissyface_tug"), { 47, 9 });
		animator->AddAnimation(EBossState::BOSS_RETURNAXE, ResourceManager::GetInstance()->GetSprite("spr_kissyface_returnaxe"), { 0, 0 });
	}

	EffectorComponent* effector = _components.GetComponent<EffectorComponent>();
	if (effector != nullptr)
	{
		effector->InitComponent();
		effector->SetOwner(this);
	}

	_components.GetComponent<EnemyMovementComponent>()->InitComponent(this);

	_stateMachine = new StateMachine<EBossState>(this);
	_stateMachine->AddState(new BossState_Ready(this));
	_stateMachine->AddState(new BossState_ToBattle(this));
	_stateMachine->AddState(new BossState_Idle(this));
	_stateMachine->AddState(new BossState_PreLunge(this));
	_stateMachine->AddState(new BossState_Lunge(this));
	_stateMachine->AddState(new BossState_LungeAttack(this));
	_stateMachine->AddState(new BossState_PreJump(this));
	_stateMachine->AddState(new BossState_Jump(this));
	_stateMachine->AddState(new BossState_JumpAttack(this));
	_stateMachine->AddState(new BossState_Land(this));
	_stateMachine->AddState(new BossState_ThrowAxe(this));
	_stateMachine->AddState(new BossState_TugAxe(this));
	_stateMachine->AddState(new BossState_ReturnAxe(this));
	_stateMachine->AddState(new BossState_Block(this));
	_stateMachine->AddState(new BossState_Hurt(this));
	_stateMachine->AddState(new BossState_Recorver(this));
	_stateMachine->AddState(new BossState_Struggle(this));
	_stateMachine->AddState(new BossState_Finished(this));
	_stateMachine->AddState(new BossState_Defeat(this));
	_stateMachine->AddState(new BossState_Die(this));
	_stateMachine->ChangeState(EBossState::BOSS_READY);

	_components.GetComponent<EnemyMovementComponent>()->InitComponent(this);

	animator->SetFlipped(true);
	vFrontDir = { -1, 0 };

	_progressBackground = ResourceManager::GetInstance()->GetTexture("progress_background");
	_progressCurrent = ResourceManager::GetInstance()->GetTexture("progress_gauge");
	_progressPrev = ResourceManager::GetInstance()->GetTexture("progress_prevgauge");
}

void Boss::Update(float deltaTime)
{
	_stateMachine->Update(deltaTime);
	if (GetCurrentState() == EBossState::BOSS_STRUGGLE && InputManager::GetInstance()->GetButtonPressed(KeyType::LeftMouse))
	{
		if (fStruggleGauge < fPrevGauge)
		{
			fStruggleGauge += 60 * deltaTime;
		}
		else
		{
			fStruggleGauge += 10 * deltaTime;
		}
	}
	if (fStruggleGauge >= 100.f)
	{
		Defeat();
	}
}

void Boss::PostUpdate(float deltaTime)
{
	SetPos(_components.GetComponent<EnemyMovementComponent>()->GetNewPos());
	_components.GetComponent<EnemyMovementComponent>()->SetAcceleration({ 0, 0 });
}

void Boss::Render(HDC hdc)
{
	_components.RenderComponents(hdc);
	Super::Render(hdc);
	if (GetCurrentState() == EBossState::BOSS_STRUGGLE)
	{
		Vector2 pos = GetPos();
		pos.y -= 80.f;
		pos.x -= 50.f;
		_progressBackground->RenderProgress(hdc, pos, 100, 10);
		_progressPrev->RenderProgress(hdc, pos, (int32)fPrevGauge, 10);
		_progressCurrent->RenderProgress(hdc, pos, (int32)fStruggleGauge, 10);
	}
}

int32 Boss::GetCurrentState()
{
	return _stateMachine->GetCurrentStateType();
}

void Boss::ChangeState(int32 stateType)
{
	_stateMachine->ChangeState(static_cast<EBossState>(stateType));
}

bool Boss::ToBattle()
{
	float length = (_targetPlayer->GetPos() - GetPos()).LengthSquared();
	// 500 이내일 때
	return length <= 250000;
}

void Boss::StartStruggle()
{
	ChangeState(EBossState::BOSS_STRUGGLE);
	Vector2 pos = GetPos();
	_targetPlayer->SetPos({ pos.x + (vFrontDir.x * 50.f), pos.y });
	_targetPlayer->ChangeState(EPlayerState::PLAYER_STRUGGLE);
}

void Boss::EndStruggle()
{
	_targetPlayer->ChangeState(EPlayerState::PLAYER_HURT_BEGIN);
	_targetPlayer->AddForce({ vFrontDir.x * 2000.f, -150.f });
	fPrevGauge = fStruggleGauge;
	fStruggleGauge = 0.f;
}

void Boss::Defeat()
{
	fPrevGauge = 0.f;
	fStruggleGauge = 0.f;
	ChangeState(EBossState::BOSS_FINISHED);
	Vector2 pos = GetPos();
	_targetPlayer->SetPos({ pos.x + (vFrontDir.x * 60.f), pos.y + 25.f });
	_targetPlayer->ChangeState(EPlayerState::PLAYER_FINISH);
}

void Boss::EndFinishMotion()
{
	_targetPlayer->ChangeState(EPlayerState::PLAYER_IDLE);
}

void Boss::Die()
{
	CollisionManager::GetInstance()->DeleteCollider(this);
	ChangeState(EBossState::BOSS_DIE);
}

void Boss::UpdateDirection()
{
	Vector2 dir = _targetPlayer->GetPos() - GetPos();
	if (dir.x >= 0)
	{
		vFrontDir.x = 1;
		_components.GetComponent<Animator>()->SetFlipped(false);
	}
	else
	{
		vFrontDir.x = -1;
		_components.GetComponent<Animator>()->SetFlipped(true);
	}
}

void Boss::ThrowAxe()
{
	Vector2 pos = GetPos();
	//Vector2 targetPos = _targetPlayer->GetPos();
	//targetPos.y -= 20.f;
	//Vector2 dir = (targetPos - GetPos()).GetNormalize();
	pos.x += vFrontDir.x * 100.f;
	pos.y -= 15.f;
	OnSpawnAxe(pos, GetPos(), vFrontDir, true);
}

void Boss::TakeDamage(Actor* damageCauser, const Vector2& attackDirection)
{
	if (GetCurrentState() == EBossState::BOSS_HURT)
	{
		StartStruggle();
		return;
	}
	if (GetCurrentState() == EBossState::BOSS_DEFEAT)
	{
		Die();
		return;
	}

	if ((attackDirection.x > 0 && vFrontDir.x > 0) || (attackDirection.x < 0 && vFrontDir.x < 0))
	{
		ChangeState(EBossState::BOSS_HURT);
		Vector2 knockbackDir = { vFrontDir.x * 1000.f, -500.f };
		vFrontDir.x = -vFrontDir.x;
		_components.GetComponent<Animator>()->FlipAnimation();
		_components.GetComponent<EnemyMovementComponent>()->SetVelocity(knockbackDir);
	}
	else
	{
		Vector2 reflectVector = { -attackDirection.x, -attackDirection.y };
		_targetPlayer->AddForce(reflectVector * 500.f);
		_targetPlayer->AttackBlocked();
		ChangeState(EBossState::BOSS_BLOCK);
	}
}

void Boss::OnCollisionBeginOverlap(const CollisionInfo& info)
{
	switch (info.collisionLayer)
	{
	case ECollisionLayer::GROUND:
		ProcessGroundCollision(info);
		break;
	case ECollisionLayer::WALL:
		ProcessWallCollision(info);
		break;
	case ECollisionLayer::ENEMY_HITBOX:
		TakeDamage(info.collisionActor, info.vHitNormal);
		break;
	default:
		break;
	}
}

void Boss::OnCollisionStayOverlap(const CollisionInfo& info)
{
	switch (info.collisionLayer)
	{
	case ECollisionLayer::GROUND:
		ProcessGroundCollision(info);
		break;
	case ECollisionLayer::WALL:
		ProcessWallCollision(info);
		break;
	default:
		break;
	}
}

void Boss::OnCollisionEndOverlap(const CollisionInfo& info)
{
	switch (info.collisionLayer)
	{
	case ECollisionLayer::GROUND:
		_components.GetComponent<EnemyMovementComponent>()->SetOnGround(false);
		break;
	default:
		break;
	}
}

void Boss::ProcessGroundCollision(const CollisionInfo& collisionInfo)
{
	EnemyMovementComponent* movementComp = _components.GetComponent<EnemyMovementComponent>();

	switch (collisionInfo.hitCorner)
	{
	case 0:
		if (movementComp->GetVelocity().y < 0.f) break;
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

void Boss::ProcessWallCollision(const CollisionInfo& collisionInfo)
{
	EnemyMovementComponent* movementComp = _components.GetComponent<EnemyMovementComponent>();

	float halfWidth = GetCollider()->GetWidth() * 0.5f;
	// 벽 방향에 따른 위치 조정
	if (collisionInfo.vHitNormal.x < 0) // 오른쪽 벽
	{
		movementComp->SetNewPosX(collisionInfo.collisionPoint.x - halfWidth);
	}
	else if (collisionInfo.vHitNormal.x > 0)// 왼쪽 벽
	{
		movementComp->SetNewPosX(collisionInfo.collisionPoint.x + halfWidth);
	}

	movementComp->SetVelocityX(0.f);
}

float Boss::CheckDistance()
{
	Vector2 dir = _targetPlayer->GetPos() - GetPos();
	return dir.Length();
}
