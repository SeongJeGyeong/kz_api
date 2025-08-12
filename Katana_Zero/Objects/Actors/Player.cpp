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

    CreateAABBCollider(30, 50, ECollisionLayer::PLAYER);
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

    wstring str = std::format(L"IsGround({0}) IsJumped({1}) IsMaxJump({2}) bIsStair({3})", bIsGround, bIsJumped, bIsMaxJump, bIsStair);
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
    }
    else if (bIsStair)
    {
        vVelocity -= normalGravity * GravityLength;
    }
    else
    {
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
    float sideFactor = 300.f;

    Vector2 gravityVector = normalGravity * GravityLength;
    Vector2 sideVec = vVelocity - gravityVector;
    float sideLength = sideVec.Length();

    if (GravityLength > upFactor) gravityVector = normalGravity * upFactor;

    if (sideLength > sideFactor) sideVec = sideVec.GetNormalize() * sideFactor;
    else if (sideLength < -sideFactor) sideVec = sideVec.GetNormalize() * sideFactor;

    float friction = bIsGround ? 0.85f : 0.98f;
    sideVec *= friction;
    if (sideVec.Length() < 1.0f)
        sideVec = Vector2(0.f, 0.f);

    // 최종적인 velocity
    // v : (3,4), a :(3,0), b : (0,4)
    // v = a + b
    vVelocity = gravityVector + sideVec;

    // 속도(velocity) 위치를 변화시킨다.
    Vector2 oldPos = GetPos();
    Vector2 newPos = oldPos + (vVelocity * deltaTime);

    if (bIsStair)
    {
        Collider* stair = _currentStair->GetCollider();
        PlayerGroundCollisionResult result = CollisionManager::GetInstance()->CheckLineStairCollision(oldPos, newPos, GetCollider()->GetWidth(), GetCollider()->GetHeight(), stair, bIsStair);
        if (!result.isColliding)
        {
            bIsStair = false;
        }
        else
        {
            vector<PlayerGroundCollisionResult> colResult;
            colResult.push_back(result);
            ProcessStairCollisions(colResult, oldPos, newPos);
        }
    }

    vector<PlayerGroundCollisionResult> results = CollisionManager::GetInstance()->CheckPlayerCollision(this, oldPos, newPos);

    ProcessCollisionResults(results, oldPos, newPos);

    // 해당 새로운 위치로 설정하기 전에. 충돌체크를 한번해서 벽인지 확인
    SetPos(newPos);
}

void Player::Jump()
{
    vVelocity.y = -fJumpInitialVelocity; // 음수는 위쪽 방향
    bIsGround = false;
    bIsStair = false;
    bIsJumped = true;
    bIsMaxJump = false;
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
            bIsMaxJump = true;
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
    bIsMaxJump = true;
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

void Player::ProcessCollisionResults(const vector<PlayerGroundCollisionResult>& results, Vector2 oldPos, Vector2& newPos)
{
    bIsGround = false;
    bIsPlatform = false;
    bIsWall = false;
    if (results.empty())
    {
        bIsStair = false;
        return;
    }

    // 충돌 결과를 종류별로 분류
    vector<PlayerGroundCollisionResult> groundCollisions;
    vector<PlayerGroundCollisionResult> wallCollisions;
    vector<PlayerGroundCollisionResult> stairCollisions;
    vector<PlayerGroundCollisionResult> platformCollisions;

    for (const auto& result : results)
    {
        switch (result.collisionLayer)
        {
        case ECollisionLayer::GROUND:
            groundCollisions.push_back(result);
            break;
        case ECollisionLayer::WALL:
            wallCollisions.push_back(result);
            break;
        case ECollisionLayer::STAIR:
            stairCollisions.push_back(result);
            break;
        case ECollisionLayer::PLATFORM:
            platformCollisions.push_back(result);
            break;
        }
    }

    // 1. 바닥/플랫폼 충돌 처리 (수직 이동 제한)
    if (!groundCollisions.empty() || !platformCollisions.empty())
    {
        ProcessGroundCollisions(groundCollisions, platformCollisions, newPos);
    }

    // 2. 벽 충돌 처리 (수평 이동 제한)
    if (!wallCollisions.empty())
    {
        ProcessWallCollisions(wallCollisions, newPos);
    }

    if (!bIsPlatform && !stairCollisions.empty())
    {
        bool ignoreStair = false;

        if (!platformCollisions.empty())
        {
            float stairY = GetLowestCollisionY(stairCollisions);
            float platY = GetLowestCollisionY(platformCollisions);

            // 플랫폼이 계단보다 높거나 같으면 계단 무시
            if (platY <= stairY)
                ignoreStair = true;
        }

        if (!ignoreStair)
        {
            // 3. 계단 충돌 처리 (경사면 따라 이동)
            ProcessStairCollisions(stairCollisions, oldPos, newPos);
        }
    }
    else
    {
        bIsStair = false;
    }
}

void Player::ProcessGroundCollisions(const vector<PlayerGroundCollisionResult>& groundCollisions, const vector<PlayerGroundCollisionResult>& platformCollisions, Vector2& newPos)
{
    PlayerGroundCollisionResult bestGroundCollision;
    bool hasGroundCollision = false;
    float bestGroundY = FLT_MAX;

    // 가장 높은(가장 적은 Y값) 바닥 찾기
    for (const auto& collision : groundCollisions)
    {
        if (fabs(collision.normal.y) > 0.5f && collision.collisionPoint.y < bestGroundY)
        {
            bestGroundY = collision.collisionPoint.y;
            bestGroundCollision = collision;
            hasGroundCollision = true;
            bIsGround = true;
        }
    }

    // 플랫폼도 확인 (아래에서 위로 올라갈 때만)
    if (vVelocity.y > 0) // 아래로 떨어질 때만 플랫폼 충돌 처리
    {
        for (const auto& collision : platformCollisions)
        {
            if (collision.normal.y < -0.5f && collision.collisionPoint.y < bestGroundY)
            {
                bestGroundY = collision.collisionPoint.y;
                bestGroundCollision = collision;
                hasGroundCollision = true;
                bIsPlatform = true;
            }
        }
    }

    if (hasGroundCollision)
    {
        newPos.y = bestGroundCollision.collisionPoint.y - GetCollider()->GetHeight() * 0.5f;
        vVelocity.y = 0;
        bIsStair = false;
        vHitNormal = bestGroundCollision.normal;
    }
}

void Player::ProcessWallCollisions(const vector<PlayerGroundCollisionResult>& wallCollisions, Vector2& newPos)
{
    for (const auto& collision : wallCollisions)
    {
        if (fabs(collision.normal.x) > 0.5f)
        {
            float halfWidth = GetCollider()->GetWidth() * 0.5f;
            // 벽 방향에 따른 위치 조정
            if (collision.normal.x < 0) // 오른쪽 벽
            {
                newPos.x = collision.collisionPoint.x - halfWidth;
                //newPos.x -= 0.001f;
            }
            else // 왼쪽 벽
            {
                newPos.x = collision.collisionPoint.x + halfWidth;
                //newPos.x += 0.001f;
            }

            // 벽과의 충돌 시 수평 속도만 제거
            vVelocity.x = 0;
            bIsWall = true;
            vHitNormal = collision.normal;
        }
    }
}

void Player::ProcessStairCollisions(const vector<PlayerGroundCollisionResult>& stairCollisions, Vector2 oldPos, Vector2& newPos)
{
    // 가장 가까운 계단 충돌 선택
    PlayerGroundCollisionResult best = stairCollisions[0];
    float minDist = (newPos - best.collisionPoint).Length();

    for (const auto& col : stairCollisions)
    {
        float dist = (newPos - col.collisionPoint).Length();
        if (dist < minDist)
        {
            minDist = dist;
            best = col;
        }
    }

    _currentStair = best.groundActor;

    float halfH = GetCollider()->GetHeight() * 0.5f;

    // 선택된 모서리를 계단에 맞춤
    float deltaY = best.collisionPoint.y - (newPos.y + halfH);
    newPos.y += deltaY;

    // 계단 상태 유지
    bIsStair = true;
    bIsGround = true;
    bIsJumped = false;
    bIsMaxJump = false;

    // 계단 법선 저장
    vHitNormal = best.normal;

    // 속도 보정: 계단 방향으로 파고드는 성분 제거
    float vDot = vVelocity.Dot(best.normal);
    if (vDot < 0)
        vVelocity -= best.normal * vDot;

    //// 가장 가까운 계단 충돌 선택
    //PlayerGroundCollisionResult bestStairCollision;
    //float closestDistance = FLT_MAX;
    //bool hasStairCollision = false;

    //for (const auto& collision : stairCollisions)
    //{
    //    float distance = (newPos - collision.collisionPoint).Length();
    //    if (distance < closestDistance)
    //    {
    //        closestDistance = distance;
    //        bestStairCollision = collision;
    //        hasStairCollision = true;
    //        _currentStair = collision.groundActor;
    //    }
    //}

    //if (!hasStairCollision) return;

    //Vector2 stairNormal = bestStairCollision.normal;
    //Vector2 stairCollisionPoint = bestStairCollision.collisionPoint;

    //float halfWidth = GetCollider()->GetWidth() * 0.5f;
    //float halfHeight = GetCollider()->GetHeight() * 0.5f;

    //// 계단 상에서 "고정할 모서리 X 좌표" 계산
    //float cornerX = newPos.x; // 기본: 중앙
    //if (bestStairCollision.hitCorner == 1)
    //{
    //    cornerX = newPos.x - halfWidth;
    //}
    //else if (bestStairCollision.hitCorner == 2)
    //{
    //    cornerX = newPos.x + halfWidth;
    //}

    //// 계단 선분 상에서 cornerX에서의 Y 계산
    //float stairYAtCorner = 0.0f; bool inSegment = false;

    //// line 가 필요하므로 groundActor(계단 Collider)에 저장된 정보에서 start/end를 꺼내야 함.
    //Collider* stairCollider = static_cast<Collider*>(bestStairCollision.groundActor->GetCollider());
    //if (stairCollider)
    //{
    //    Vector2 start = stairCollider->GetStartPoint();
    //    Vector2 end = stairCollider->GetEndPoint();
    //    CollisionManager::GetInstance()->GetYOnLineAtX(start, end, cornerX, stairYAtCorner, inSegment);
    //}

    //// 모서리 X가 계단 선분 범위 밖이면 계단에서 벗어난 것으로 간주하고 무시
    //if (!inSegment)
    //{
    //    bIsStair = false;
    //    return;
    //    // fallback: collision point 기준으로 배치
    //    //Vector2 desiredBottomPos = stairCollisionPoint - stairNormal * 1.0f;
    //    //newPos.y = desiredBottomPos.y - halfHeight;
    //}
    //else
    //{
    //    // corner의 Y를 기준으로 플레이어를 계단에 맞춤
    //    Vector2 desiredBottomPos = Vector2(cornerX, stairYAtCorner) - stairNormal * 1.0f; // 약간 위로 띄움

    //    // newPos.x는 건드리지 않고 Y만 계단에 맞춘다
    //    newPos.y = desiredBottomPos.y - halfHeight;
    //}

    //// 속도 조정: 계단에 박히지 않도록 법선 성분 제거
    //float velocityNormalComp = vVelocity.Dot(stairNormal);
    //if (velocityNormalComp > 0) // y축 기준으로 위로 빠져나가는 성분(좌표체계에 따라 조정 필요)
    //{
    //    // 상승 방향이면 보정하지 않음 (보통은 아래로 향하는 중력을 제거)
    //}
    //else
    //{
    //    // 계단으로 파고드는 음의 성분을 제거
    //    vVelocity -= stairNormal * velocityNormalComp;
    //}

    //// 중력 보정: 계단 법선 방향 성분 제거 (계단에 붙도록)
    //float gravityNormalComp = vGravity.Dot(stairNormal);
    //if (gravityNormalComp < 0)
    //{
    //    vAcceleration -= stairNormal * gravityNormalComp;
    //}

    //// 바닥 상태 갱신
    //bIsStair = true;
    //bIsJumped = false;
    //bIsMaxJump = false;
    //vHitNormal = stairNormal;
}

float Player::GetLowestCollisionY(const vector<PlayerGroundCollisionResult>& cols)
{
    float minY = FLT_MAX;
    for (auto& c : cols)
        minY = min(minY, c.collisionPoint.y);
    return minY;
}

