#include "pch.h"
#include "BossState.h"
#include "../Objects/Actors/Boss.h"
#include "../Components/Animator.h"
#include "../Components/EnemyMovementComponent.h"
#include "../Components/Collider.h"
#include "../Managers/TimeManager.h"

void BossState::UpdateState(float deltaTime)
{
	if (_boss == nullptr) return;
	_boss->UpdateAttachedComponents(deltaTime);
}

///////////////// Ready /////////////////
void BossState_Ready::EnterState()
{
	_boss->GetComponent<Animator>()->SetAnimation(EBossState::BOSS_READY);
}

void BossState_Ready::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	if (_boss->ToBattle())
	{
		_boss->ChangeState(EBossState::BOSS_TOBATTLE);
	}
}

void BossState_Ready::ExitState()
{
}

///////////////// ToBattle /////////////////
void BossState_ToBattle::EnterState()
{
	_boss->GetComponent<Animator>()->SetAnimation(EBossState::BOSS_TOBATTLE);
}

void BossState_ToBattle::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	if (_boss->GetComponent<Animator>()->IsAnimationFinished())
	{
		_boss->ChangeState(EBossState::BOSS_IDLE);
	}
}

void BossState_ToBattle::ExitState()
{
}

///////////////// Idle /////////////////
void BossState_Idle::EnterState()
{
	_boss->GetComponent<Animator>()->SetAnimation(EBossState::BOSS_IDLE);
	_boss->SetWasHit(false);
}

void BossState_Idle::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	fWaitTime += deltaTime;
	if (fWaitTime >= 0.5f)
	{
		fWaitTime = 0.f;
		if (_boss->CheckDistance() <= 200.f)
		{
			_boss->ChangeState(EBossState::BOSS_PREJUMP);
		}
		else
		{
			_boss->ChangeState(EBossState::BOSS_THROWAXE);

			//if (TimeManager::GetInstance()->GetRandom())
			//{
			//	_boss->ChangeState(EBossState::BOSS_PRELUNGE);
			//}
			//else
			//{
			//	_boss->ChangeState(EBossState::BOSS_THROWAXE);
			//}
		}
	}
	_boss->UpdateDirection();
}

void BossState_Idle::ExitState()
{
}

///////////////// PreLunge /////////////////
void BossState_PreLunge::EnterState()
{
	_boss->GetComponent<Animator>()->SetAnimation(EBossState::BOSS_PRELUNGE);
}

void BossState_PreLunge::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	if (_boss->GetComponent<Animator>()->IsAnimationFinished())
	{
		_boss->ChangeState(EBossState::BOSS_LUNGE);
	}
}

void BossState_PreLunge::ExitState()
{
}

///////////////// Lunge /////////////////
void BossState_Lunge::EnterState()
{
	_boss->GetComponent<Animator>()->SetAnimation(EBossState::BOSS_LUNGE);
	Vector2 frontDir = _boss->GetFrontDir();
	_boss->GetComponent<EnemyMovementComponent>()->SetVelocity({ frontDir.x * 1000.f, -500.f });
	_boss->GetComponent<EnemyMovementComponent>()->SetOnGround(false);
}

void BossState_Lunge::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	if (_boss->GetComponent<EnemyMovementComponent>()->GetOnGround())
	{
		_boss->ChangeState(EBossState::BOSS_LUNGEATTACK);
	}
}

void BossState_Lunge::ExitState()
{
}

///////////////// LungeAttack /////////////////
void BossState_LungeAttack::EnterState()
{
	_boss->GetComponent<Animator>()->SetAnimation(EBossState::BOSS_LUNGEATTACK);
}

void BossState_LungeAttack::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	if (_boss->GetComponent<Animator>()->IsAnimationFinished())
	{
		_boss->ChangeState(EBossState::BOSS_IDLE);
	}
}

void BossState_LungeAttack::ExitState()
{
}

///////////////// PreJump /////////////////
void BossState_PreJump::EnterState()
{
	_boss->GetComponent<Animator>()->SetAnimation(EBossState::BOSS_PREJUMP);
}

void BossState_PreJump::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	if (_boss->GetComponent<Animator>()->IsAnimationFinished())
	{
		_boss->ChangeState(EBossState::BOSS_JUMP);
	}
}

void BossState_PreJump::ExitState()
{
}

///////////////// Jump /////////////////
void BossState_Jump::EnterState()
{
	_boss->GetComponent<Animator>()->SetAnimation(EBossState::BOSS_JUMP);
}

void BossState_Jump::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	if (_boss->GetComponent<EnemyMovementComponent>()->GetVelocity().y >= 0.f)
	{
		_boss->ChangeState(EBossState::BOSS_JUMPATTACK);
	}
}

void BossState_Jump::ExitState()
{
}

///////////////// JumpAttack /////////////////
void BossState_JumpAttack::EnterState()
{
	_boss->GetComponent<Animator>()->SetAnimation(EBossState::BOSS_JUMPATTACK);
}

void BossState_JumpAttack::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
}

void BossState_JumpAttack::ExitState()
{
}

///////////////// Land /////////////////
void BossState_Land::EnterState()
{
	_boss->GetComponent<Animator>()->SetAnimation(EBossState::BOSS_LAND);
}

void BossState_Land::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	if (_boss->GetComponent<Animator>()->IsAnimationFinished())
	{
		_boss->ChangeState(EBossState::BOSS_IDLE);
	}
}

void BossState_Land::ExitState()
{
}

///////////////// ThrowAxe /////////////////
void BossState_ThrowAxe::EnterState()
{
	_boss->GetComponent<Animator>()->SetAnimation(EBossState::BOSS_THROWAXE);
}

void BossState_ThrowAxe::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	if (_boss->GetComponent<Animator>()->IsAnimationFinished())
	{
		_boss->ChangeState(EBossState::BOSS_TUGAXE);
	}
}

void BossState_ThrowAxe::ExitState()
{
}

///////////////// TugAxe /////////////////
void BossState_TugAxe::EnterState()
{
	_boss->GetComponent<Animator>()->SetAnimation(EBossState::BOSS_TUGAXE);
	_boss->ThrowAxe();
}

void BossState_TugAxe::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
}

void BossState_TugAxe::ExitState()
{
}

///////////////// ReturnAxe /////////////////
void BossState_ReturnAxe::EnterState()
{
	_boss->GetComponent<Animator>()->SetAnimation(EBossState::BOSS_RETURNAXE);
}

void BossState_ReturnAxe::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	if (_boss->GetComponent<Animator>()->IsAnimationFinished())
	{
		_boss->ChangeState(EBossState::BOSS_IDLE);
	}
}

void BossState_ReturnAxe::ExitState()
{
}

///////////////// Block /////////////////
void BossState_Block::EnterState()
{
	_boss->GetComponent<Animator>()->SetAnimation(EBossState::BOSS_BLOCK);
}

void BossState_Block::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	if (_boss->GetComponent<Animator>()->IsAnimationFinished())
	{
		_boss->ChangeState(EBossState::BOSS_IDLE);
	}
}

void BossState_Block::ExitState()
{
}

///////////////// Hurt /////////////////
void BossState_Hurt::EnterState()
{
	_boss->GetComponent<Animator>()->SetAnimation(EBossState::BOSS_HURT);
	_boss->GetCollider()->InitAABB(150, 100);
	fRecoverTime = 0.f;
}

void BossState_Hurt::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);

	EnemyMovementComponent* movementComp = _boss->GetComponent<EnemyMovementComponent>();

	if (movementComp->GetOnGround())
	{
		if (_boss->GetComponent<Animator>()->GetCurrentFrame() == 2)
		{
			_boss->GetComponent<Animator>()->SetPlaying(true);
		}

		fRecoverTime += deltaTime;
		if (fRecoverTime > 2.f)
		{
			_boss->ChangeState(EBossState::BOSS_RECOVER);
		}
	}
	else
	{
		if (_boss->GetComponent<Animator>()->GetCurrentFrame() == 2)
		{
			_boss->GetComponent<Animator>()->SetPlaying(false);
		}
	}
}

void BossState_Hurt::ExitState()
{
	_boss->GetCollider()->InitAABB(50, 100);
}

///////////////// Recover /////////////////
void BossState_Recorver::EnterState()
{
	_boss->GetComponent<Animator>()->SetAnimation(EBossState::BOSS_RECOVER);
}

void BossState_Recorver::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	if (_boss->GetComponent<Animator>()->IsAnimationFinished())
	{
		_boss->ChangeState(EBossState::BOSS_IDLE);
	}
}

void BossState_Recorver::ExitState()
{
}

///////////////// Struggle /////////////////
void BossState_Struggle::EnterState()
{
	_boss->GetComponent<Animator>()->SetAnimation(EBossState::BOSS_STRUGGLE);
	fStruggleTime = 0.f;
}

void BossState_Struggle::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	fStruggleTime += deltaTime;
	if (fStruggleTime >= 3.f)
	{
		fStruggleTime = 0.f;
		_boss->ChangeState(EBossState::BOSS_RECOVER);
	}
}

void BossState_Struggle::ExitState()
{
	if (fStruggleTime == 0.f)
	{
		_boss->EndStruggle();
	}
}

///////////////// Finished /////////////////
void BossState_Finished::EnterState()
{
	_boss->GetCollider()->InitAABB(150, 100);
	_boss->GetComponent<Animator>()->SetAnimation(EBossState::BOSS_FINISHED);
}

void BossState_Finished::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	if (_boss->GetComponent<Animator>()->IsAnimationFinished())
	{
		_boss->ChangeState(EBossState::BOSS_DEFEAT);
	}
}

void BossState_Finished::ExitState()
{
	_boss->EndFinishMotion();
}

///////////////// Defeat /////////////////
void BossState_Defeat::EnterState()
{
	_boss->GetComponent<Animator>()->SetAnimation(EBossState::BOSS_DEFEAT);
}

void BossState_Defeat::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
}

void BossState_Defeat::ExitState()
{
}

///////////////// Die /////////////////
void BossState_Die::EnterState()
{
	_boss->GetComponent<Animator>()->SetAnimation(EBossState::BOSS_DIE);
}

void BossState_Die::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
}

void BossState_Die::ExitState()
{
}