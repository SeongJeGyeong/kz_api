#include "pch.h"
#include "Player.h"
#include "../../Game/Game.h"
#include "../../Managers/InputManager.h"
#include "../../Scenes/Scene.h"
#include "../../Components/Animator.h"
#include "../../Managers/ResourceManager.h"
#include "../../Components/OBBCollider.h"
#include "../../Managers/CollisionManager.h"
#include "../../Components/EffectorComponent.h"
#include "../../Components/InputComponent.h"
#include "../../States/PlayerState.h"
#include "../../States/StateMachine.h"
#include "../../Managers/TimeManager.h"
#include "../../Components/CameraComponent.h"
#include "../../Components/PlayerMovementComponent.h"

void Player::Init(Vector2 pos)
{
	Super::Init(pos);

    CreateAABBCollider(30, 50, ECollisionLayer::PLAYER);
    _components.AddComponent<Animator>();
    _components.AddComponent<EffectorComponent>();
    _components.AddComponent<InputComponent>();
    _components.AddComponent<CameraComponent>();
    _components.AddComponent<PlayerMovementComponent>();

    CameraComponent* camera = _components.GetComponent<CameraComponent>();
    camera->InitComponent(this);

    Animator* animator = _components.GetComponent<Animator>();
    if (animator != nullptr)
    {
        animator->SetOwner(this);
        animator->InitComponent(EPlayerState::PLAYER_END);
        animator->AddAnimation(EPlayerState::PLAYER_IDLE, ResourceManager::GetInstance()->GetSprite("spr_idle"), { 0, -10 });
        animator->AddAnimation(EPlayerState::PLAYER_IDLE_TO_RUN, ResourceManager::GetInstance()->GetSprite("spr_idle_to_run"), { 0, -10 });
        animator->AddAnimation(EPlayerState::PLAYER_RUN, ResourceManager::GetInstance()->GetSprite("spr_run"), { 0, -6 });
        animator->AddAnimation(EPlayerState::PLAYER_RUN_TO_IDLE, ResourceManager::GetInstance()->GetSprite("spr_run_to_idle"), { 0, -11 });
        animator->AddAnimation(EPlayerState::PLAYER_PRECROUCH, ResourceManager::GetInstance()->GetSprite("spr_precrouch"), { 0, -13 });
        animator->AddAnimation(EPlayerState::PLAYER_CROUCH, ResourceManager::GetInstance()->GetSprite("spr_crouch"), { 0, -13 });
        animator->AddAnimation(EPlayerState::PLAYER_POSTCROUCH, ResourceManager::GetInstance()->GetSprite("spr_postcrouch"), { 0, -13 });
        animator->AddAnimation(EPlayerState::PLAYER_JUMP, ResourceManager::GetInstance()->GetSprite("spr_jump"), { -10, 0 });
        animator->AddAnimation(EPlayerState::PLAYER_FALL, ResourceManager::GetInstance()->GetSprite("spr_fall"), { -10, -5 });
        animator->AddAnimation(EPlayerState::PLAYER_ATTACK, ResourceManager::GetInstance()->GetSprite("spr_attack"));
        animator->AddAnimation(EPlayerState::PLAYER_ROLL, ResourceManager::GetInstance()->GetSprite("spr_roll"), { 0, -10 });
        animator->AddAnimation(EPlayerState::PLAYER_HURT_BEGIN, ResourceManager::GetInstance()->GetSprite("spr_hurtfly_begin"), { 0, -10 });
        animator->AddAnimation(EPlayerState::PLAYER_HURT_LOOP, ResourceManager::GetInstance()->GetSprite("spr_hurtfly_loop"));
        animator->AddAnimation(EPlayerState::PLAYER_HURT_GROUND, ResourceManager::GetInstance()->GetSprite("spr_hurtground"));
        animator->AddAnimation(EPlayerState::PLAYER_HURT_RECOVER, ResourceManager::GetInstance()->GetSprite("spr_hurtrecover"), { 0, -20 });
    }

    EffectorComponent* effector = _components.GetComponent<EffectorComponent>();
    if (effector != nullptr)
    {
        effector->InitComponent();
        effector->SetOwner(this);
    }

    InputComponent* inputComp = _components.GetComponent<InputComponent>();
    if (inputComp != nullptr)
    {
        inputComp->InitComponent(this);
    }
    _stateMachine = new StateMachine<EPlayerState>(this);
    _stateMachine->AddState(new PlayerState_Idle(this));
    _stateMachine->AddState(new PlayerState_Idle_to_Run(this));
    _stateMachine->AddState(new PlayerState_Run(this));
    _stateMachine->AddState(new PlayerState_Run_to_Idle(this));
    _stateMachine->AddState(new PlayerState_PreCrouch(this));
    _stateMachine->AddState(new PlayerState_Crouch(this));
    _stateMachine->AddState(new PlayerState_PostCrouch(this));
    _stateMachine->AddState(new PlayerState_Jump(this));
    _stateMachine->AddState(new PlayerState_Fall(this));
    _stateMachine->AddState(new PlayerState_Attack(this));
    _stateMachine->AddState(new PlayerState_Roll(this));
    _stateMachine->AddState(new PlayerState_Struggle(this));
    _stateMachine->AddState(new PlayerState_HurtBegin(this));
    _stateMachine->AddState(new PlayerState_HurtFly(this));
    _stateMachine->AddState(new PlayerState_HurtGround(this));
    _stateMachine->AddState(new PlayerState_Recover(this));
    _stateMachine->AddState(new PlayerState_Finish(this));
    _stateMachine->ChangeState(EPlayerState::PLAYER_IDLE);

    _movementComp = _components.GetComponent<PlayerMovementComponent>();
    if (_movementComp != nullptr) _movementComp->InitComponent(this);

    _attackInfo._attackLayer = ECollisionLayer::PLAYER_HITBOX;
    _attackInfo.fWidth = 106 * 1.4f;
    _attackInfo.fHeight = 32 * 1.4f;
    _attackInfo.vAttackDir = { 0, 0 };
}

void Player::Update(float deltaTime)
{
    if (bBlocked)
    {
        fAttackEnableTime += deltaTime;
        if (fAttackEnableTime >= 1.f)
        {
            fAttackEnableTime = 0.f;
            bBlocked = false;
        }
    }

    if (GetCurrentState() == EPlayerState::PLAYER_STRUGGLE || GetCurrentState() == EPlayerState::PLAYER_FINISH) return;
    fAttackDelayTime += deltaTime;
    _stateMachine->Update(deltaTime);

    _movementComp->ApplyPhysics(deltaTime);
    Vector2 velocity = _movementComp->GetVelocity();
    // 속도(velocity) 위치를 변화시킨다.
    Vector2 NewPos = GetPos() + (velocity * deltaTime);
    _movementComp->SetNewPos(NewPos);

    if (_attackInfo.bIsAttack)
    {
        Vector2 pos = GetPos();
        CollisionManager::GetInstance()->CheckOBBHitBox(this, _attackInfo);
    }
}

void Player::PostUpdate(float deltaTime)
{
    if (GetCurrentState() == EPlayerState::PLAYER_STRUGGLE || GetCurrentState() == EPlayerState::PLAYER_FINISH) return;
    SetPos(_movementComp->GetNewPos());
    _movementComp->SetAcceleration({ 0,0 });
}

void Player::Render(HDC hdc)
{
    Super::Render(hdc);
    if (GetCurrentState() == EPlayerState::PLAYER_STRUGGLE || GetCurrentState() == EPlayerState::PLAYER_FINISH) return;
    _components.RenderComponents(hdc);

    SetTextColor(hdc, RGB(255, 255, 255));

    printState(hdc);
    wstring strs = std::format(L"bIsCrouch({0})", bIsCrouch);
    ::TextOut(hdc, 450, 50, strs.c_str(), static_cast<int32>(strs.size()));

    wstring str2 = std::format(L"Pos({0}, {1})", GetPos().x, GetPos().y);
    ::TextOut(hdc, 100, 70, str2.c_str(), static_cast<int32>(str2.size()));

    if (_attackInfo.bIsAttack)
    {
        Vector2 pos = _components.GetComponent<CameraComponent>()->ConvertScreenPos(GetPos());
        RenderHitbox(hdc, pos, _attackInfo.fAttackRadian, 1.4f, (!_attackInfo._hitActors.empty()) ? RGB(0, 255, 0) : RGB(255, 0, 0));
    }
}

void Player::Jump()
{
    _movementComp->Jump();
    bIsMaxJump = false;
    fJumpPressedTime = 0.f;
    _stateMachine->ChangeState(EPlayerState::PLAYER_JUMP);
}

void Player::IncreaseJump(float deltaTime)
{
    fJumpPressedTime += deltaTime;
    if (fJumpPressedTime >= 0.07f)
    {
        if (fJumpPressedTime >= fMaxJumpHoldTime)
        {
            fJumpPressedTime = 0.f;
            bIsMaxJump = true;
        }
        else
        {
            float holdFactor = 1.0f - (fJumpPressedTime / fMaxJumpHoldTime) * 0.7f;

            _movementComp->AddAcceleration({ 0, -fJumpHoldForce * holdFactor });
            //vAcceleration.y -= fJumpHoldForce * holdFactor;
        }
    }
}

void Player::ReleaseJump()
{
    bIsMaxJump = true;
    fJumpPressedTime = 0;
}

void Player::Move(bool dir)
{
    Vector2 prevDir = vFrontDir;

    if (dir)
    {
        if (vHitNormal.x > -1.f)
        {
            _movementComp->AddAcceleration({ fMoveForce, 0 });
            //vAcceleration.x += fMoveForce;
            _components.GetComponent<Animator>()->SetFlipped(false);
            vFrontDir = { 1, 0 };
        }
    }
    else
    {
        if (vHitNormal.x < 1.f)
        {
            _movementComp->AddAcceleration({ -fMoveForce, 0 });
            //vAcceleration.x -= fMoveForce;
            _components.GetComponent<Animator>()->SetFlipped(true);
            vFrontDir = { -1, 0 };
        }
    }
    if ((_movementComp->GetOnGround() || _movementComp->GetIsPlatform() || _movementComp->GetOnStair()) && _movementComp->GetAcceleration().x != 0.f)
    {
        if (_stateMachine->GetCurrentStateType() == EPlayerState::PLAYER_IDLE || prevDir != vFrontDir)
        {
            _stateMachine->ChangeState(EPlayerState::PLAYER_IDLE_TO_RUN);
        }
    }
}

void Player::Crouch(bool active)
{
    bIsCrouch = active;
    if (active)
    {
        _stateMachine->ChangeState(EPlayerState::PLAYER_PRECROUCH);
    }
    else
    {
        _stateMachine->ChangeState(EPlayerState::PLAYER_POSTCROUCH);
    }
}

void Player::Roll(bool dir)
{
    if (dir)
    {
        if (vHitNormal.x > -1.f)
        {
            _movementComp->AddVelocity({100000.f, 0});
            _components.GetComponent<Animator>()->SetFlipped(false);
            vFrontDir = { 1, 0 };
        }
    }
    else
    {
        if (vHitNormal.x < 1.f)
        {
            _movementComp->AddVelocity({ -100000.f, 0 });
            _components.GetComponent<Animator>()->SetFlipped(true);
            vFrontDir = { -1, 0 };
        }
    }
    bIsCrouch = false;
    _stateMachine->ChangeState(EPlayerState::PLAYER_ROLL);
}

void Player::Attack()
{
    if (bBlocked) return;
    Vector2 mousePos = InputManager::GetInstance()->GetMousePos();
    Vector2 worldPos = _components.GetComponent<CameraComponent>()->ConvertWorldPos(mousePos);
    Vector2 pos = GetPos();
    Vector2 dir = worldPos - pos;
    dir.Normalize();

    float rad = atan2(dir.y, dir.x);

    // 플레이어의 후면을 공격할 경우, 반전된 스프라이트를 사용하므로
    // 정면 각도와 대칭이 되는 각도를 반환하도록 -180도 빼줌
    if (dir.x < 0)
    {
        rad -= PI;
        vFrontDir = Vector2(-1, 0);
    }
    else
    {
        vFrontDir = Vector2(1, 0);
    }

    _components.GetComponent<EffectorComponent>()->PlayEffect("spr_slash", (dir.x < 0), rad, 1.5f, true);
    _movementComp->AttackForce(dir);
    _components.GetComponent<Animator>()->SetFlipped((dir.x < 0));
    _stateMachine->ChangeState(EPlayerState::PLAYER_ATTACK);
    fAttackDelayTime = 0.f;
    _attackInfo.vAttackDir = dir;
    _attackInfo.bIsAttack = true;
    _attackInfo.fAttackRadian = rad;
}

int32 Player::GetCurrentState()
{
    return _stateMachine->GetCurrentStateType();
}

void Player::ChangeState(int32 stateType)
{
    _stateMachine->ChangeState(static_cast<EPlayerState>(stateType));
}

void Player::ProcessGroundCollision(const CollisionInfo& collisionInfo)
{
    switch (collisionInfo.hitCorner)
    {
    case 0: // 바닥 충돌
        ProcessGroundFloor(collisionInfo);
        break;
    case 3: // 천장 충돌
        ProcessGroundCeiling(collisionInfo);
        break;
    case 1: // 왼쪽 벽 충돌
    case 2: // 오른쪽 벽 충돌
        ProcessGroundWall(collisionInfo);
        break;
    }
}

void Player::ProcessGroundFloor(const CollisionInfo& collisionInfo)
{
    _movementComp->SetNewPosY(collisionInfo.collisionPoint.y - GetCollider()->GetHeight() * 0.5f);
    //newPos.y = collisionInfo.collisionPoint.y - GetCollider()->GetHeight() * 0.5f;
    _movementComp->SetVelocityY(0.f);
    _movementComp->SetOnGround(true);
    vHitNormal = collisionInfo.vHitNormal;
}

void Player::ProcessGroundCeiling(const CollisionInfo& collisionInfo)
{
    _movementComp->SetNewPosY(collisionInfo.collisionPoint.y + GetCollider()->GetHeight() * 0.5f);
    //newPos.y = collisionInfo.collisionPoint.y + GetCollider()->GetHeight() * 0.5f;
    Vector2 velocity = _movementComp->GetVelocity();
    if (velocity.y < 0)
    {
        _movementComp->SetVelocityY(0.f);
    }
}

void Player::ProcessGroundWall(const CollisionInfo& collisionInfo)
{
    float halfWidth = GetCollider()->GetWidth() * 0.5f;
    if (collisionInfo.vHitNormal.x > 0) // 왼쪽 벽
    {
        _movementComp->SetNewPosX(collisionInfo.collisionPoint.x + halfWidth);
        //newPos.x = collisionInfo.collisionPoint.x + halfWidth;
    }
    else if(collisionInfo.vHitNormal.x < 0)// 오른쪽 벽
    {
        _movementComp->SetNewPosX(collisionInfo.collisionPoint.x - halfWidth);
        //newPos.x = collisionInfo.collisionPoint.x - halfWidth;
    }

    Vector2 velocity = _movementComp->GetVelocity();

    if ((collisionInfo.vHitNormal.x > 0 && velocity.x < 0) ||
        (collisionInfo.vHitNormal.x < 0 && velocity.x > 0))
    {
        _movementComp->SetVelocityX(0.f);
    }

    vHitNormal = collisionInfo.vHitNormal;
}

void Player::ProcessPlatformCollision(const CollisionInfo& collisionInfo)
{
    if (_components.GetComponent<InputComponent>()->GetPressedDown()) return;

    Vector2 velocity = _movementComp->GetVelocity();

    // 플랫폼은 아래에서 위로 올라갈 때만 충돌 처리 (위에서 아래로는 통과)
    if (velocity.y < 0) // 위로 올라가거나 정지 상태면 플랫폼 무시
        return;

    // 플랫폼은 위쪽 면만 충돌 처리 (법선이 위쪽을 향하는 경우만)
    if (collisionInfo.vHitNormal.y < -0.5f)
    {
        // 플랫폼 위에 착지
        _movementComp->SetNewPosY(collisionInfo.collisionPoint.y - GetCollider()->GetHeight() * 0.5f);
        //newPos.y = collisionInfo.collisionPoint.y - GetCollider()->GetHeight() * 0.5f;
        velocity.y = 0; // 아래로 떨어지는 속도 제거
        _movementComp->SetVelocity(velocity);
        _movementComp->SetIsPlatform(true);

        vHitNormal = collisionInfo.vHitNormal;

        // 점프 관련 상태 초기화
        _movementComp->SetIsJumped(false);
        bIsMaxJump = false;
        fJumpPressedTime = 0.f;
    }
}

void Player::ProcessWallCollision(const CollisionInfo& collisionInfo)
{
    float halfWidth = GetCollider()->GetWidth() * 0.5f;
    // 벽 방향에 따른 위치 조정
    if (collisionInfo.vHitNormal.x < 0) // 오른쪽 벽
    {
        _movementComp->SetNewPosX(collisionInfo.collisionPoint.x - halfWidth);
        //newPos.x = collisionInfo.collisionPoint.x - halfWidth;
    }
    else if(collisionInfo.vHitNormal.x > 0)// 왼쪽 벽
    {
        _movementComp->SetNewPosX(collisionInfo.collisionPoint.x + halfWidth);
        //newPos.x = collisionInfo.collisionPoint.x + halfWidth;
    }

    _movementComp->SetVelocityX(0.f);
    _movementComp->SetIsWall(true);
    vHitNormal = collisionInfo.vHitNormal;
}

void Player::ProcessCeilingCollision(const CollisionInfo& collisionInfo)
{
    float halfHeight = GetCollider()->GetHeight() * 0.5f;

    if (collisionInfo.vHitNormal.y > 0)
    {
        _movementComp->SetNewPosY(collisionInfo.collisionPoint.y + halfHeight);
        //newPos.y = collisionInfo.collisionPoint.y + halfHeight;
    }

    _movementComp->SetVelocityY(0.f);
    vHitNormal = collisionInfo.vHitNormal;
}

void Player::ProcessStairCollision(const CollisionInfo& collisionInfo, Vector2 oldPos)
{
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
    Vector2 newPos = _movementComp->GetNewPos();
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
        _movementComp->SetOnStair(false);
        return;
    }

    // 플레이어를 계단에 맞춤 (선택된 모서리가 계단에 정확히 닿도록)
    float deltaY = stairYAtCorner - targetCorner.y;
    _movementComp->AddNewPos({0, deltaY });
    //newPos.y += deltaY;

    if (_movementComp->GetAcceleration().x != 0.f) // 실제로 이동 중일 때만
    {
        Vector2 stairDirection = stairDir.GetNormalize(); // 계단 방향 단위벡터
        Vector2 velocity = _movementComp->GetVelocity();

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
        _movementComp->SetVelocity(velocity);
    }

    // 계단 상태 설정
    _movementComp->SetOnStair(true);
    _movementComp->SetIsJumped(false);
    bIsMaxJump = false;

    Vector2 stairNormal = Vector2(-stairDir.y, stairDir.x).GetNormalize();
    if (stairNormal.y > 0) stairNormal *= -1; // 위쪽을 향하도록
    vHitNormal = stairNormal;

}

void Player::OnCollisionBeginOverlap(const CollisionInfo& info)
{
    if (GetCurrentState() == EPlayerState::PLAYER_STRUGGLE) return;

    switch (info.collisionLayer)
    {
    case ECollisionLayer::GROUND:
        ProcessGroundCollision(info);
        break;
    case ECollisionLayer::PLATFORM:
        ProcessPlatformCollision(info);
        break;
    case ECollisionLayer::WALL:
        ProcessWallCollision(info);
        break;
    case ECollisionLayer::CEILING:
        ProcessCeilingCollision(info);
        break;
    case ECollisionLayer::STAIR:
        if (_movementComp->GetIsPlatform()) break;
        ProcessStairCollision(info, GetPos());
        break;
    case ECollisionLayer::ENEMY_HITBOX:
        if(!info.collisionActor->GetIsActive())break;
        TakeDamage(info.collisionActor, { -info.vHitNormal.x, -info.vHitNormal.y });
        break;
    default:
        break;
    }
}

void Player::OnCollisionStayOverlap(const CollisionInfo& info)
{
    if (GetCurrentState() == EPlayerState::PLAYER_STRUGGLE) return;

    switch (info.collisionLayer)
    {
    case ECollisionLayer::GROUND:
        ProcessGroundCollision(info);
        break;
    case ECollisionLayer::PLATFORM:
        break;
    case ECollisionLayer::WALL:
        if (info.vHitNormal.x != 0)
        {
            vHitNormal.x = info.vHitNormal.x;
        }
        else
        {
            vHitNormal.x = 0;
        }
        break;
    case ECollisionLayer::CEILING:
        break;
    case ECollisionLayer::STAIR:
        ProcessStairCollision(info, GetPos());
        break;
    case ECollisionLayer::ENEMY_HITBOX:
        if (!info.collisionActor->GetIsActive())break;
        TakeDamage(info.collisionActor, info.vHitNormal);
        break;
    default:
        break;
    }
}

void Player::OnCollisionEndOverlap(const CollisionInfo& info)
{
   switch (info.collisionLayer)
    {
    case ECollisionLayer::GROUND:
        vHitNormal = { 0, 0 };
        _movementComp->SetOnGround(false);
        break;
    case ECollisionLayer::PLATFORM:
        _movementComp->SetIsPlatform(false);
        break;
    case ECollisionLayer::WALL:
        vHitNormal = { 0, 0 };
        _movementComp->SetIsWall(false);
        break;
    case ECollisionLayer::CEILING:
        break;
    case ECollisionLayer::STAIR:
        _movementComp->SetOnStair(false);
        break;
    default:
        break;
    }
}

void Player::AddForce(Vector2 force)
{
    _movementComp->SetVelocity(force);
    if (force.y < 0)_movementComp->SetOnGround(false);
}

void Player::SetPlayerCamera(Camera* camera)
{
    CameraComponent* cameraComp = _components.GetComponent<CameraComponent>();
    if (cameraComp) cameraComp->SetCamera(camera);
}

void Player::printState(HDC hdc)
{
    wstring str;
    switch (_stateMachine->GetCurrentStateType())
    {
    case EPlayerState::PLAYER_IDLE :
        str = L"PLAYER_IDLE";
       break;
    case EPlayerState::PLAYER_IDLE_TO_RUN:
        str = L"PLAYER_IDLE_TO_RUN";
        break;
    case EPlayerState::PLAYER_RUN:
        str = L"PLAYER_RUN";
        break;
    case EPlayerState::PLAYER_RUN_TO_IDLE:
        str = L"PLAYER_RUN_TO_IDLE";
        break;
    case EPlayerState::PLAYER_JUMP:
        str = L"PLAYER_JUMP";
        break;
    case EPlayerState::PLAYER_FALL:
        str = L"PLAYER_FALL";
        break;
    case EPlayerState::PLAYER_PRECROUCH:
        str = L"PLAYER_PRECROUCH";
        break;
    case EPlayerState::PLAYER_CROUCH:
        str = L"PLAYER_CROUCH";
        break;
    case EPlayerState::PLAYER_POSTCROUCH:
        str = L"PLAYER_POSTCROUCH";
        break;
    case EPlayerState::PLAYER_ATTACK:
        str = L"PLAYER_ATTACK";
        break;
    case EPlayerState::PLAYER_ROLL:
        str = L"PLAYER_ROLL";
        break;
    default:
        break;
    }

    ::TextOut(hdc, 100, 50, str.c_str(), static_cast<int32>(str.size()));
}

void Player::RenderHitbox(HDC hdc, Vector2 pos, float radian, float scale, COLORREF color)
{
    vector<Vector2> corners = GetRotatedCorners(pos.x, pos.y, radian, 106 * scale, 32 * scale);

    // 마지막 점은 처음 점과 연결해야 하므로 복사
    POINT polygon[5];
    for (int32 i = 0; i < 5; ++i)
    {
        int32 idx = (i + 1) % 4;
        polygon[i] = { (LONG)corners[idx].x, (LONG)corners[idx].y };
    }

    // 빨간색 펜으로 OBB 출력
    HPEN hPen = CreatePen(PS_SOLID, 1, color);
    HGDIOBJ oldPen = SelectObject(hdc, hPen);
    HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));

    Polyline(hdc, polygon, 5);

    // 원래 펜/브러시 복구
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(hPen);
}

void Player::AttackBlocked()
{
    bBlocked = true;
}

void Player::TakeDamage(Actor* damageCauser, const Vector2& attackDirection)
{
    if (bInvincible || bIsDead) return;

    bIsDead = true;
    ChangeState(EPlayerState::PLAYER_HURT_BEGIN);
    AddForce({ attackDirection.x * 2000.f, -200.f });
}

Vector2 Player::GetNewPos()
{
    return _components.GetComponent<PlayerMovementComponent>()->GetNewPos();
}