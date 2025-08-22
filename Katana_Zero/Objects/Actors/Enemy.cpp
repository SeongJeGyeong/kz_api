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
	_stateMachine->Update(deltaTime);

	if (bWasHit) return;

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

	HPEN hPen = CreatePen(PS_SOLID, 2, bDetected ? RGB(255, 0, 0) : RGB(0, 0, 255)); // 파란색, 2픽셀 두께
	HBRUSH hBrush = (HBRUSH)GetStockObject(HOLLOW_BRUSH); // 투명한 브러쉬

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

	// 사용한 객체 삭제
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

	// 계단 방향 판단
	Collider* stairCollider = collisionInfo.collisionActor->GetCollider();
	Vector2 stairStart = stairCollider->GetStartPoint();
	Vector2 stairEnd = stairCollider->GetEndPoint();
	Vector2 stairDir = stairEnd - stairStart;

	// 계단이 우상향인지 좌상향인지 판단
	bool isRightUpStair = (stairDir.x > 0 && stairDir.y < 0) || (stairDir.x < 0 && stairDir.y > 0);

	// 올바른 모서리 선택
	Vector2 targetCorner;
	Vector2 newPos = movementComp->GetNewPos();
	if (isRightUpStair)
	{
		// 우상향 계단: 오른쪽 모서리 사용
		targetCorner = Vector2(newPos.x + halfW, newPos.y + halfH);
	}
	else
	{
		// 좌상향 계단: 왼쪽 모서리 사용
		targetCorner = Vector2(newPos.x - halfW, newPos.y + halfH);
	}

	// 계단 선분 상에서 해당 X 좌표의 Y값 계산
	float stairYAtCorner;
	if (!CollisionManager::GetInstance()->GetYOnLineAtX(stairStart, stairEnd, targetCorner.x, stairYAtCorner))
	{
		// 모서리가 계단 범위를 벗어나면 계단에서 벗어난 것으로 처리
		movementComp->SetOnStair(false);
		return;
	}

	// 플레이어를 계단에 맞춤 (선택된 모서리가 계단에 정확히 닿도록)
	float deltaY = stairYAtCorner - targetCorner.y;
	movementComp->AddNewPos({ 0, deltaY });

	if (movementComp->GetAcceleration().x != 0.f) // 실제로 이동 중일 때만
	{
		Vector2 stairDirection = stairDir.GetNormalize(); // 계단 방향 단위벡터
		Vector2 velocity = movementComp->GetVelocity();

		// 현재 수평 속도를 그대로 유지하되, 계단 방향으로 변환
		float currentHorizontalSpeed = abs(velocity.x);

		// 계단의 기울기를 고려한 속도 보정
		float stairSlope = abs(stairDirection.y / stairDirection.x); // 기울기
		float speedMultiplier = 1.0f / sqrt(1.0f + stairSlope * stairSlope); // 빗변 보정

		// 실제로는 더 빠르게 만들어서 바닥과 비슷한 속도감 제공
		speedMultiplier *= 1.8f; // 속도 증가

		// 이동 방향에 따른 속도 설정
		if ((velocity.x > 0 && stairDirection.x > 0) || (velocity.x < 0 && stairDirection.x < 0))
		{
			// 계단 방향으로 이동
			velocity = stairDirection * currentHorizontalSpeed * speedMultiplier;
		}
		else if ((velocity.x > 0 && stairDirection.x < 0) || (velocity.x < 0 && stairDirection.x > 0))
		{
			// 계단 반대 방향으로 이동
			velocity = stairDirection * (-currentHorizontalSpeed) * speedMultiplier;
		}
		movementComp->SetVelocity(velocity);
	}

	// 계단 상태 설정
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

	// 시작/끝 각도에 해당하는 점 좌표
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
}

void Enemy::TakeDamage(Actor* damageCauser, const Vector2& attackDirection)
{
	if (bWasHit) return;
	bIsDead = true;
	bWasHit = true;
	_stateMachine->ChangeState(EEnemyState::ENEMY_HURT_FLY);
	EnemyMovementComponent* movementComp = _components.GetComponent<EnemyMovementComponent>();
	movementComp->SetOnGround(false);
	movementComp->SetVelocityX(attackDirection.x * 10000.f);
	movementComp->SetVelocityY(attackDirection.y * 1000.f);
}

Vector2 Enemy::GetNewPos()
{
	return _components.GetComponent<EnemyMovementComponent>()->GetNewPos();
}