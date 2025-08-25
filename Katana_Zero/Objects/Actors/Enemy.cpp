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
#include "../../Managers/CollisionManager.h"
#include "../Camera.h"
#include "../../Managers/TimeManager.h"
#include "../../Managers/SoundManager.h"

Enemy::~Enemy()
{
	SAFE_DELETE(_stateMachine);
}

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
		animator->AddAnimation(EEnemyState::ENEMY_RUN, ResourceManager::GetInstance()->GetSprite("spr_gangsterrun"), { 0, -5 });
		animator->AddAnimation(EEnemyState::ENEMY_AIM, ResourceManager::GetInstance()->GetSprite("spr_gangsteridle"), { 0, -15 });
		animator->AddAnimation(EEnemyState::ENEMY_HURT_FLY, ResourceManager::GetInstance()->GetSprite("spr_gangsterhurtfly"), { 0, 0 });
		animator->AddAnimation(EEnemyState::ENEMY_HURT_GROUND, ResourceManager::GetInstance()->GetSprite("spr_gangsterhurtground"), { 0, 0 });
	}

	EffectorComponent* effector = _components.GetComponent<EffectorComponent>();
	if (effector != nullptr)
	{
		effector->InitComponent();
		effector->SetOwner(this);
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
	TimeManager* timeManager = TimeManager::GetInstance();
	_components.GetComponent<EffectorComponent>()->UpdateConstantEffect(timeManager->GetConstDeltaTime());
	if (timeManager->GetHitStop()) return;
	_stateMachine->Update(deltaTime);

	if (bIsDead) return;
	if (bWasHit)
	{
		fWaitDyingTime += timeManager->GetConstDeltaTime();
		if (fWaitDyingTime >= 0.05f)
		{
			fWaitDyingTime = 0.f;
			bWasHit = false;
			Die();
		}
		return;
	}

	if (vFrontDir.x == 1)
	{
		bDetected = DetectPlayer(fDetectRange, -40.f, 40.f);
	}
	else
	{
		bDetected = DetectPlayer(fDetectRange, 140.f, 220.f);
	}
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

	if (bWasHit) return;

	HPEN hPen = CreatePen(PS_SOLID, 2, bDetected ? RGB(255, 0, 0) : RGB(0, 0, 255)); // �Ķ���, 2�ȼ� �β�
	HBRUSH hBrush = (HBRUSH)GetStockObject(HOLLOW_BRUSH); // ������ �귯��

	SelectObject(hdc, hPen);
	SelectObject(hdc, hBrush);

	Vector2 screenPos = Game::GetInstance()->GetCurrentSceneCamera()->ConvertScreenPos(GetPos());

	if (vFrontDir.x == 1)
	{
		DrawSight(hdc, screenPos.x, screenPos.y, fDetectRange, -40.f, 40.f);
	}
	else
	{
		DrawSight(hdc, screenPos.x, screenPos.y, fDetectRange, 140.f, 220.f);
	}

	// ����� ��ü ����
	DeleteObject(hPen);
	DeleteObject(hBrush);
}

void Enemy::OnCollisionBeginOverlap(const CollisionInfo& info)
{
	switch (info.collisionLayer)
	{
	case ECollisionLayer::GROUND:
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
	case ECollisionLayer::STAIR:
		//if (_movementComp->GetIsPlatform()) break;
		ProcessStairCollision(info, GetPos());
		break;
	case ECollisionLayer::ENEMY_HITBOX:
		TakeDamage(info.collisionActor, info.vHitNormal);
		break;
	default:
		break;
	}
}

void Enemy::OnCollisionStayOverlap(const CollisionInfo& info)
{
	switch (info.collisionLayer)
	{
	case ECollisionLayer::GROUND:
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
		case ECollisionLayer::STAIR:
			ProcessStairCollision(info, GetPos());
			break;
	default:
		break;
	}
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

void Enemy::ProcessStairCollision(const CollisionInfo& collisionInfo, Vector2 oldPos)
{
	EnemyMovementComponent* movementComp = _components.GetComponent<EnemyMovementComponent>();

	float halfW = GetCollider()->GetWidth() * 0.5f;
	float halfH = GetCollider()->GetHeight() * 0.5f;

	// ��� ���� �Ǵ�
	Collider* stairCollider = collisionInfo.collisionActor->GetCollider();
	Vector2 stairStart = stairCollider->GetStartPoint();
	Vector2 stairEnd = stairCollider->GetEndPoint();
	Vector2 stairDir = stairEnd - stairStart;

	// ����� ��������� �»������� �Ǵ�
	bool isRightUpStair = (stairDir.x > 0 && stairDir.y < 0) || (stairDir.x < 0 && stairDir.y > 0);

	// �ùٸ� �𼭸� ����
	Vector2 targetCorner;
	Vector2 newPos = movementComp->GetNewPos();
	if (isRightUpStair)
	{
		// ����� ���: ������ �𼭸� ���
		targetCorner = Vector2(newPos.x + halfW, newPos.y + halfH);
	}
	else
	{
		// �»��� ���: ���� �𼭸� ���
		targetCorner = Vector2(newPos.x - halfW, newPos.y + halfH);
	}

	// ��� ���� �󿡼� �ش� X ��ǥ�� Y�� ���
	float stairYAtCorner;
	if (!CollisionManager::GetInstance()->GetYOnLineAtX(stairStart, stairEnd, targetCorner.x, stairYAtCorner))
	{
		// �𼭸��� ��� ������ ����� ��ܿ��� ��� ������ ó��
		movementComp->SetOnStair(false);
		return;
	}

	// �÷��̾ ��ܿ� ���� (���õ� �𼭸��� ��ܿ� ��Ȯ�� �굵��)
	float deltaY = stairYAtCorner - targetCorner.y;
	movementComp->AddNewPos({ 0, deltaY });

	if (movementComp->GetAcceleration().x != 0.f) // ������ �̵� ���� ����
	{
		Vector2 stairDirection = stairDir.GetNormalize(); // ��� ���� ��������
		Vector2 velocity = movementComp->GetVelocity();

		// ���� ���� �ӵ��� �״�� �����ϵ�, ��� �������� ��ȯ
		float currentHorizontalSpeed = abs(velocity.x);

		// ����� ���⸦ ����� �ӵ� ����
		float stairSlope = abs(stairDirection.y / stairDirection.x); // ����
		float speedMultiplier = 1.0f / sqrt(1.0f + stairSlope * stairSlope); // ���� ����

		// �����δ� �� ������ ���� �ٴڰ� ����� �ӵ��� ����
		speedMultiplier *= 1.8f; // �ӵ� ����

		// �̵� ���⿡ ���� �ӵ� ����
		if ((velocity.x > 0 && stairDirection.x > 0) || (velocity.x < 0 && stairDirection.x < 0))
		{
			// ��� �������� �̵�
			velocity = stairDirection * currentHorizontalSpeed * speedMultiplier;
		}
		else if ((velocity.x > 0 && stairDirection.x < 0) || (velocity.x < 0 && stairDirection.x > 0))
		{
			// ��� �ݴ� �������� �̵�
			velocity = stairDirection * (-currentHorizontalSpeed) * speedMultiplier;
		}
		movementComp->SetVelocity(velocity);
	}

	// ��� ���� ����
	movementComp->SetOnStair(true);
}

void Enemy::ChangeState(int32 stateType)
{
	_stateMachine->ChangeState(static_cast<EEnemyState>(stateType));
}

bool Enemy::PlayerIsBack()
{
	float dirX = _targetPlayer->GetPos().x - GetPos().x;

	return (vFrontDir.x > 0.f && dirX < 0.f || vFrontDir.x < 0.f && dirX > 0.f);
}

bool Enemy::DetectPlayer(float radius, float angleStart, float angleEnd)
{
	float distX = _targetPlayer->GetPos().x - GetPos().x;
	float distY = _targetPlayer->GetPos().y - GetPos().y;
	float squareRoot = distX * distX + distY * distY;

	if (squareRoot > radius * radius) return false;

	float rad = atan2(distY, distX);

	float startRad = DegreeToRadian(angleStart);
	float endRad = DegreeToRadian(angleEnd);

	if (rad < 0) rad += 2 * 3.14159265f;
	if (startRad < 0) startRad += 2 * 3.14159265f;
	if (endRad < 0) endRad += 2 * 3.14159265f;

	if (startRad <= endRad)
	{
		return rad >= startRad && rad <= endRad;
	}
	else
	{
		return rad >= startRad || rad <= endRad;
	}

	return false;
}

bool Enemy::InShotRange()
{
	Vector2 dir = _targetPlayer->GetPos() - GetPos();
	float dist = dir.LengthSquared();

	return (dist <= fShotRnage * fShotRnage);
}

bool Enemy::IsBlockedObstacles()
{
	return false;
}

void Enemy::DrawSight(HDC hdc, int centerX, int centerY, int radius, float startAngle, float endAngle)
{
	int left = centerX - radius;
	int top = centerY - radius;
	int right = centerX + radius;
	int bottom = centerY + radius;

	double startRad = DegreeToRadian(startAngle);
	double endRad = DegreeToRadian(endAngle);

	// ����/�� ������ �ش��ϴ� �� ��ǥ
	int x1 = centerX + static_cast<int>(radius * cosf(startRad));
	int y1 = centerY - static_cast<int>(radius * sinf(startRad));
	int x2 = centerX + static_cast<int>(radius * cosf(endRad));
	int y2 = centerY - static_cast<int>(radius * sinf(endRad));

	Pie(hdc, left, top, right, bottom, x1, y1, x2, y2);
}

void Enemy::ShotBullet()
{
	Vector2 ShotPoint = GetPos();
	ShotPoint.y -= 10.f;
	ShotPoint.x += vFrontDir.x * 60.f;

	Vector2 dir = _targetPlayer->GetPos() - ShotPoint;
	dir.Normalize();
	float rad = atan2(dir.y, dir.x);

	OnCreateBullet(ShotPoint, dir, 34.f, rad);
	Vector2 pos = GetPos();
	pos.x += vFrontDir.x * 80.f;
	pos.y -= 10.f;
	GetComponent<EffectorComponent>()->PlayEffect("spr_gunspark3", (vFrontDir.x < 1.f), 0.f, 1.f, false, pos);
	GetComponent<EffectorComponent>()->PlayEffect("spr_gunsmoke3", (vFrontDir.x < 1.f), 0.f, 1.f, false, pos);
	SoundManager::GetInstance()->PlaySFX("sound_gun_fire_1");
}

void Enemy::TakeDamage(Actor* damageCauser, const Vector2& attackDirection)
{
	if (damageCauser == nullptr) return;
	if (damageCauser->GetCollider()->GetCollisionLayer() == ECollisionLayer::ENEMY_HITBOX && !damageCauser->GetWasHit()) return;
	if (bWasHit || bIsDead) return;
	bWasHit = true;

	_stateMachine->ChangeState(EEnemyState::ENEMY_HURT_FLY);
	EnemyMovementComponent* movementComp = _components.GetComponent<EnemyMovementComponent>();
	movementComp->SetOnGround(false);
	movementComp->SetVelocityX(attackDirection.x * 2000.f);
	movementComp->SetVelocityY(attackDirection.y * 2000.f);
	_components.GetComponent<EffectorComponent>()->PlayConstantEffect("spr_slashfx", false, 0, 1.5f, true);

	float rad = atan2(attackDirection.y, attackDirection.x);

	if (damageCauser->GetCollider()->GetColliderType() == EColliderType::LINE)
	{
		damageCauser->SetIsDead(true);
		SoundManager::GetInstance()->PlaySFX("sound_enemy_death_bullet");
	}
	else if (damageCauser->GetCollider()->GetCollisionLayer() == ECollisionLayer::PLAYER)
	{
		if (attackDirection.x < 0)
		{
			vFrontDir = Vector2(1, 0);
		}
		else
		{
			rad -= PI;
			vFrontDir = Vector2(-1, 0);
		}

		Vector2 offset = { vFrontDir.x * 30.f, 0 };

		_components.GetComponent<EffectorComponent>()->PlayConstantEffect("spr_hit_impact", (attackDirection.x < 0), rad, 1.5f, true, offset);
		SoundManager::GetInstance()->PlayEnemySlashKill();
	}
}

Vector2 Enemy::GetNewPos()
{
	return _components.GetComponent<EnemyMovementComponent>()->GetNewPos();
}

void Enemy::Die()
{
	bIsDead = true;
	TimeManager::GetInstance()->SetHitStop();
	Game::GetInstance()->GetCurrentSceneCamera()->SetCameraShake(true);
}