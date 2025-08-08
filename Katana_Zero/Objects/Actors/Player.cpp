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

void Player::Init(Vector2 pos)
{
	Super::Init(pos);

    CreateAABBCollider(30, 50, {0, 10 }, ECollisionLayer::PLAYER);
    halfHeight = GetCollider()->GetHeight() * 0.5f;

    _components.AddComponent<Animator>();
    _components.AddComponent<EffectorComponent>();
    _components.AddComponent<InputComponent>();

    Animator* animator = _components.GetComponent<Animator>();
    if (animator != nullptr)
    {
        animator->SetOwner(this);
        animator->InitComponent(EPlayerState::PLAYER_END);
        animator->AddAnimation(EPlayerState::PLAYER_IDLE, ResourceManager::GetInstance()->GetTexture("zero_idle"));
        animator->AddAnimation(EPlayerState::PLAYER_IDLE_TO_RUN, ResourceManager::GetInstance()->GetTexture("zero_idle_to_run"));
        animator->AddAnimation(EPlayerState::PLAYER_RUN, ResourceManager::GetInstance()->GetTexture("zero_run"));
        animator->AddAnimation(EPlayerState::PLAYER_RUN_TO_IDLE, ResourceManager::GetInstance()->GetTexture("zero_run_to_idle"));
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
    _stateMachine = new StateMachine(this);
    _stateMachine->AddState(new PlayerState_Idle());
    _stateMachine->AddState(new PlayerState_Idle_to_Run());
    _stateMachine->AddState(new PlayerState_Run());
    _stateMachine->AddState(new PlayerState_Run_to_Idle());
    _stateMachine->ChangeState(EPlayerState::PLAYER_IDLE);
}

void Player::Update(float deltaTime)
{
	vAcceleration = Vector2(0, 0);

    _stateMachine->Update(deltaTime);

    ApplyPhysics(deltaTime);
}

void Player::Render(HDC hdc)
{
    _components.RenderComponents(hdc);
	Super::Render(hdc);

    wstring str = std::format(L"IsGround({0}) IsJumped({1})", bIsGround, bIsJumped);
    ::TextOut(hdc, 300, 30, str.c_str(), static_cast<int32>(str.size()));
    wstring str2 = std::format(L"Pos({0}, {1})", GetPos().x, GetPos().y);
    ::TextOut(hdc, 100, 50, str2.c_str(), static_cast<int32>(str2.size()));

    wstring str4 = std::format(L"hitnNormal( {0}, {1} )", vHitNormal.x, vHitNormal.y);
    ::TextOut(hdc, 100, 100, str4.c_str(), static_cast<int32>(str4.size()));

    float halfHeight = GetCollider()->GetHeight() * 0.5f;
}

void Player::OnCollisionHit(Collider* other)
{
	_collider->SetBlocked(true);
}

void Player::ApplyPhysics(float deltaTime)
{
    // 중력의 방향 단위 벡터와 속도를 내적하여, 중력 방향의 값만 가진 벡터를 얻음.
    Vector2 normalGravity = vGravity.GetNormalize();
    float GravityLength = vVelocity.Dot(normalGravity);

    if (bIsGround)
    {
        bIsJumped = false;
        vVelocity -= normalGravity * GravityLength;
        //fJumpForce = 0;
    }
    else
    {
        //vAcceleration.y += fJumpForce;
        // 중력 가속도 적용
        vAcceleration += vGravity;

        // 최고점 근처에서 체공 효과를 위한 공기저항 시뮬레이션
        if (vVelocity.y < 0) // 상승 중일 때
        {
            // 속도가 느려질수록 공기저항도 감소 (자연스러운 감속)
            float airResistance = fAirResistance * (vVelocity.y * vVelocity.y) / (fJumpInitialVelocity * fJumpInitialVelocity);
            vAcceleration.y += airResistance;
        }
    }
    // 가속도는 속도(velocity)를 변화시킨다.
    vVelocity += vAcceleration * deltaTime;

    float upFactor = 400.f;
    float sideFactor = 150.f;

    Vector2 gravityVector = normalGravity * GravityLength;
    Vector2 sideVec = vVelocity - gravityVector;
    float sideLength = sideVec.Length();

    if (GravityLength > upFactor) gravityVector = normalGravity * upFactor;

    if (sideLength > sideFactor) sideVec = sideVec.GetNormalize() * sideFactor;
    else if (sideLength < -sideFactor) sideVec = sideVec.GetNormalize() * sideFactor;

    float friction = bIsGround ? 0.85f : 0.98f;
    //float friction = 0.98f;
    sideVec *= friction;
    if (sideVec.Length() < 1.0f)
        sideVec = Vector2(0.f, 0.f);

    // 최종적인 velocity
    // v : (3,4), a :(3,0), b : (0,4)
    // v = a + b
    vVelocity = gravityVector + sideVec;

    // 속도(velocity) 위치를 변화시킨다.
    Vector2 oldPos = _collider->GetCenterOffset();
    Vector2 newPos = oldPos + (vVelocity * deltaTime);

    float halfHeight = GetCollider()->GetHeight() * 0.5f;

    PlayerGroundCollisionResult groundResult = CollisionManager::GetInstance()->CheckPlayerGroundCollision(this, oldPos, newPos);

    if (groundResult.isColliding)
    {
        // 바닥에 충돌함 - 위치 조정
        newPos.y = groundResult.collisionPoint.y - GetCollider()->GetHeight() * 0.5f;
        vVelocity.y = 0; // 수직 속도 제거
        bIsGround = true;

        // 추가적인 충돌 처리 로직...
    }

    vHitNormal = groundResult.normal;

    newPos.y -= 10;

    // 해당 새로운 위치로 설정하기 전에. 충돌체크를 한번해서 벽인지 확인
    SetPos(newPos);
}

void Player::Jump()
{
    vVelocity.y = -fJumpInitialVelocity; // 음수는 위쪽 방향
    bIsGround = false;
    bIsJumped = false;
    fJumpPressedTime = 0.f;
}

void Player::IncreaseJump(float deltaTime)
{
    fJumpPressedTime += deltaTime;
    if (fJumpPressedTime >= 0.07f)
    {
        if (fJumpPressedTime >= fMaxJumpHoldTime)
        {
            fPrevPressedTime = fJumpPressedTime;
            fJumpPressedTime = 0.f;
            bIsJumped = true;
        }
        else
        {
            float holdFactor = 1.0f - (fJumpPressedTime / fMaxJumpHoldTime) * 0.7f;
            vAcceleration.y -= fJumpHoldForce * holdFactor;
        }
    }
}

void Player::ReleaseJump()
{
    bIsJumped = true;
    fPrevPressedTime = fJumpPressedTime;
    fJumpPressedTime = 0;
}

void Player::Move(bool dir)
{
    Vector2 prevDir = vFrontDir;
    if (dir)
    {
        if (vHitNormal.x > -1.f)
        {
            vAcceleration.x += fMoveForce / fMass;
            _components.GetComponent<Animator>()->SetFlipped(false);
            vFrontDir = { 1, 0 };
        }
    }
    else
    {
        if (vHitNormal.x < 1.f)
        {
            vAcceleration.x -= fMoveForce / fMass;
            _components.GetComponent<Animator>()->SetFlipped(true);
            vFrontDir = { -1, 0 };
        }
    }
    if (_stateMachine->GetCurrentStateType() == EPlayerState::PLAYER_IDLE || prevDir != vFrontDir)
    {
        _stateMachine->ChangeState(EPlayerState::PLAYER_IDLE_TO_RUN);
    }
}

void Player::Attack()
{
    Vector2 mousePos = InputManager::GetInstance()->GetMousePos();
    Vector2 pos = GetPos();
    Vector2 dir = mousePos - pos;
    dir.Normalize();

    float rad = atan2(dir.y, dir.x);

    // 플레이어의 후면을 공격할 경우, 반전된 스프라이트를 사용하므로
    // 정면 각도와 대칭이 되는 각도를 반환하도록 -180도 빼줌
    if (dir.x < 0) rad -= PI;

    _components.GetComponent<EffectorComponent>()->PlayEffect((dir.x < 0), rad, 1.5f, true);
}

int32 Player::GetCurrentState()
{
    return _stateMachine->GetCurrentStateType();
}

void Player::ChangeState(EPlayerState stateType)
{
    _stateMachine->ChangeState(stateType);
}