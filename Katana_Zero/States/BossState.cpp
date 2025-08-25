#include "pch.h"
#include "BossState.h"
#include "../Objects/Actors/Boss.h"
#include "../Components/Animator.h"
#include "../Components/BossMovementComponent.h"
#include "../Components/Collider.h"
#include "../Managers/TimeManager.h"
#include "../Managers/SoundManager.h"

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
	if (_boss->TargetNotFound()) return;
	fWaitTime += deltaTime;
	if (fWaitTime >= 0.5f)
	{
		fWaitTime = 0.f;
		if (_boss->CheckDistance() <= 230.f)
		{
			if (bSwitchPattern)
			{
				_boss->ChangeState(EBossState::BOSS_PREJUMP);
				bSwitchPattern = false;
			}
			else
			{
				_boss->ChangeState(EBossState::BOSS_THROWAXE);
				bSwitchPattern = true;
			}
		}
		else
		{
			if (bSwitchPattern)
			{
				_boss->ChangeState(EBossState::BOSS_THROWAXE);
				bSwitchPattern = false;
			}
			else
			{
				_boss->ChangeState(EBossState::BOSS_PRELUNGE);
				bSwitchPattern = true;
			}
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
	_boss->Lunge();
	SoundManager::GetInstance()->PlaySFX("sound_boss_kissyface_axelunge_01");
	SoundManager::GetInstance()->PlaySFX("sound_voiceboss_kissyface_axelunge_01");

}

void BossState_Lunge::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	if (_boss->GetComponent<BossMovementComponent>()->GetOnGround())
	{
		_boss->GetComponent<BossMovementComponent>()->SetLungeJump(false);
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
	SoundManager::GetInstance()->PlaySFX("sound_boss_kissyface_axeimpact_01");
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
	_boss->SetAttack(false);
}

///////////////// PreJump /////////////////
void BossState_PreJump::EnterState()
{
	_boss->GetComponent<Animator>()->SetAnimation(EBossState::BOSS_PREJUMP);
	SoundManager::GetInstance()->PlaySFX("sound_voiceboss_kissyface_crouch_01");
}

void BossState_PreJump::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	if (_boss->GetComponent<Animator>()->IsAnimationFinished())
	{
		fChargeTime += deltaTime;
		if (fChargeTime >= 0.1f)
		{
			_boss->ChangeState(EBossState::BOSS_JUMP);
		}
	}
}

void BossState_PreJump::ExitState()
{
	fChargeTime = 0.f;
}

///////////////// Jump /////////////////
void BossState_Jump::EnterState()
{
	_boss->GetComponent<Animator>()->SetAnimation(EBossState::BOSS_JUMP);
	_boss->GetComponent<BossMovementComponent>()->SetVelocityY(-700.f);
	_boss->GetComponent<BossMovementComponent>()->SetOnGround(false);
	SoundManager::GetInstance()->PlaySFX("sound_voiceboss_kissyface_jump_01");
	SoundManager::GetInstance()->PlaySFX("sound_boss_kissyface_axewhirl_01");
}

void BossState_Jump::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	if (_boss->GetComponent<Animator>()->IsAnimationFinished())
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
	_boss->ThrowAxe();
}

void BossState_JumpAttack::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	if (_boss->GetComponent<BossMovementComponent>()->GetOnGround())
	{
		_boss->ChangeState(EBossState::BOSS_LAND);
	}
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
	SoundManager::GetInstance()->PlaySFX("sound_voiceboss_kissyface_axeprep_01");
}

void BossState_ThrowAxe::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
	if (_boss->GetComponent<Animator>()->IsAnimationFinished())
	{
		_boss->ChangeState(EBossState::BOSS_TUGAXE);
	}
	else if (!bDoOnce && _boss->GetComponent<Animator>()->GetCurrentFrame() == 7)
	{
		SoundManager::GetInstance()->PlaySFX("sound_boss_kissyface_axethrow_01");
		SoundManager::GetInstance()->PlaySFX("sound_voiceboss_kissyface_axethrow_01");
		_boss->ThrowAxe();
		bDoOnce = true;
	}
}

void BossState_ThrowAxe::ExitState()
{
	bDoOnce = false;
}

///////////////// TugAxe /////////////////
void BossState_TugAxe::EnterState()
{
	_boss->GetComponent<Animator>()->SetAnimation(EBossState::BOSS_TUGAXE);
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
	SoundManager::GetInstance()->PlaySFX("sound_boss_kissyface_axecatch_01");
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
	SoundManager::GetInstance()->PlaySFX("sound_boss_kissyface_clash_01");
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
	SoundManager::GetInstance()->PlaySFX("sound_voiceboss_kissyface_hurt_01");
	SoundManager::GetInstance()->PlayEnemySlashKill();
}

void BossState_Hurt::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);

	BossMovementComponent* movementComp = _boss->GetComponent<BossMovementComponent>();

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
	SoundManager::GetInstance()->PlaySFX("sound_voiceboss_kissyface_free_01");
	SoundManager::GetInstance()->PlaySFX("sound_boss_kissyface_free_01");
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
	SoundManager::GetInstance()->PlaySFX("sound_boss_kissyface_struggle_01");
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
	SoundManager::GetInstance()->StopSFX(ESFXType::SFX_STRUGGLE);
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
	SoundManager::GetInstance()->StopBGM();
	SoundManager::GetInstance()->PlaySFX("sound_boss_kissyface_death_01");
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
	SoundManager::GetInstance()->PlaySFX("sound_enemy_death_generic_02");
}

void BossState_Die::UpdateState(float deltaTime)
{
	Super::UpdateState(deltaTime);
}

void BossState_Die::ExitState()
{
}