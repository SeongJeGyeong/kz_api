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
    vCurPos = GetPos();
	vAcceleration = Vector2(0, 0);

    _stateMachine->Update(deltaTime);

    ApplyPhysics(deltaTime);
}

void Player::PostUpdate(float deltaTime)
{
    UpdatePosition(deltaTime);
}

void Player::Render(HDC hdc)
{
    _components.RenderComponents(hdc);
	Super::Render(hdc);

    wstring str = std::format(L"IsGround({0}) IsJumped({1}) IsMaxJump({2}) bIsStair({3}) bIsPlatform({4})", bIsGround, bIsJumped, bIsMaxJump, bOnStair, bIsPlatform);
    ::TextOut(hdc, 300, 30, str.c_str(), static_cast<int32>(str.size()));
    wstring str2 = std::format(L"Pos({0}, {1})", GetPos().x, GetPos().y);
    ::TextOut(hdc, 100, 50, str2.c_str(), static_cast<int32>(str2.size()));

    wstring str4 = std::format(L"hitnNormal( {0}, {1} )", vHitNormal.x, vHitNormal.y);
    ::TextOut(hdc, 100, 100, str4.c_str(), static_cast<int32>(str4.size()));

    wstring str5 = std::format(L"velocity ( {0}, {1} )", vVelocity.x, vVelocity.y);
    ::TextOut(hdc, 100, 140, str5.c_str(), static_cast<int32>(str5.size()));

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

    if (bIsGround || bIsPlatform)
    {
        if(vVelocity.y >= 0.f) bIsJumped = false;
        vVelocity.y = 0.f;
        vVelocity -= normalGravity * GravityLength;
    }
    else if (bOnStair)
    {
        bIsJumped = false;
        vVelocity -= normalGravity * GravityLength;
        
        //Vector2 plane = { 1.f, 0.f };
        //float rad = DegreeToRadian(fMoveDegree) * -1.f;
        //float s = sin(rad);
        //float c = cos(rad);

        //plane = { plane.x * c - plane.y * s, plane.x * s + plane.y * c };

        //Vector2 planeNormal = plane.GetNormalize();
        //Vector2 vertical = planeNormal * vVelocity.Dot(planeNormal);

        //vVelocity = vVelocity - vertical;
    }
    else
    {
        // 중력 가속도 적용
        vAcceleration += vGravity;
    }

    // 최고점 근처에서 체공 효과를 위한 공기저항 시뮬레이션
    if (vVelocity.y < 0) // 상승 중일 때
    {
        // 속도가 느려질수록 공기저항도 감소 (자연스러운 감속)
        float airResistance = fAirResistance * (vVelocity.y * vVelocity.y) / (fJumpInitialVelocity * fJumpInitialVelocity);
        vAcceleration.y += airResistance;
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

    float friction = 0.98f;
    if (bIsGround || bIsPlatform)
    {
        friction = 0.80f;
    }

    sideVec *= friction;
    if (sideVec.Length() < 1.0f)
        sideVec = Vector2(0.f, 0.f);

    // 최종적인 velocity
    // v : (3,4), a :(3,0), b : (0,4)
    // v = a + b
    vVelocity = gravityVector + sideVec;

    // 속도(velocity) 위치를 변화시킨다.
    vNewPos = vCurPos + (vVelocity * deltaTime);
}

void Player::UpdatePosition(float deltaTime)
{
    // 충돌 검사 전에 이전 상태 저장
    bool prevIsGround = bIsGround;
    bool prevIsPlatform = bIsPlatform;
    bool prevOnStair = bOnStair;

    if (bOnStair)
    {
        Collider* stair = _currentStair->GetCollider();
        CollisionInfo result = CollisionManager::GetInstance()->CheckLineStairCollision(GetPos(), vNewPos, GetCollider()->GetWidth(), GetCollider()->GetHeight(), stair, bOnStair);
        if (!result.isColliding)
        {
            bOnStair = false;
        }
        else
        {
            vector<CollisionInfo> colResult;
            colResult.push_back(result);
            ProcessStairCollisions(colResult, GetPos(), vNewPos);
        }
    }

    vector<CollisionInfo> results = CollisionManager::GetInstance()->CheckPlayerCollision(this, GetPos(), vNewPos);
    ProcessCollisionResults(results, GetPos(), vNewPos);

    SetPos(vNewPos);
}

void Player::Jump()
{
    vVelocity.y = -fJumpInitialVelocity; // 음수는 위쪽 방향
    bIsGround = false;
    bIsPlatform = false;
    bOnStair = false;
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

void Player::ProcessCollisionResults(const vector<CollisionInfo>& results, Vector2 oldPos, Vector2& newPos)
{
    bIsGround = false;
    bIsPlatform = false;
    bIsWall = false;
    if (results.empty())
    {
        bOnStair = false;
        return;
    }

    // 충돌 결과를 종류별로 분류
    vector<CollisionInfo> groundCollisions;
    vector<CollisionInfo> wallCollisions;
    vector<CollisionInfo> stairCollisions;
    vector<CollisionInfo> platformCollisions;

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
    if (!groundCollisions.empty())
    {
        ProcessGroundCollisions(groundCollisions, newPos);
    }

    if (!platformCollisions.empty())
    {
        ProcessPlatformCollisions(platformCollisions, newPos);
    }

    // 2. 벽 충돌 처리 (수평 이동 제한)
    if (!wallCollisions.empty())
    {
        ProcessWallCollisions(wallCollisions, newPos);
    }

    if (platformCollisions.empty() && !stairCollisions.empty())
    {
        ProcessStairCollisions(stairCollisions, oldPos, newPos);
    }
    else
    {
        bOnStair = false;
    }
}

void Player::ProcessGroundCollisions(const vector<CollisionInfo>& groundCollisions, Vector2& newPos)
{
    // 충돌을 방향별로 분류
    vector<CollisionInfo> floorCollisions;   // 바닥 (위에서 아래로)
    vector<CollisionInfo> ceilingCollisions; // 천장 (아래에서 위로)
    vector<CollisionInfo> wallCollisions;    // 벽 (좌우)

    for (const auto& collision : groundCollisions)
    {
        switch (collision.hitCorner)
        {
        case 0: // 바닥 충돌
            floorCollisions.push_back(collision);
            break;
        case 3: // 천장 충돌
            ceilingCollisions.push_back(collision);
            break;
        case 1: // 왼쪽 벽 충돌
        case 2: // 오른쪽 벽 충돌
            wallCollisions.push_back(collision);
            break;
        }
    }

    // 1. 천장 충돌 처리 (가장 우선)
    if (!ceilingCollisions.empty())
    {
        // 가장 낮은 천장을 찾음
        CollisionInfo bestCeiling = ceilingCollisions[0];
        for (const auto& collision : ceilingCollisions)
        {
            if (collision.collisionPoint.y > bestCeiling.collisionPoint.y)
            {
                bestCeiling = collision;
            }
        }

        newPos.y = bestCeiling.collisionPoint.y + GetCollider()->GetHeight() * 0.5f;
        if (vVelocity.y < 0) vVelocity.y = 0; // 위로 올라가는 속도만 제거
        vHitNormal = bestCeiling.vHitNormal;
        return; // 천장 충돌이 있으면 다른 충돌은 무시
    }

    // 2. 바닥 충돌 처리
    if (!floorCollisions.empty())
    {
        // 가장 높은 바닥을 찾음
        CollisionInfo bestFloor = floorCollisions[0];
        for (const auto& collision : floorCollisions)
        {
            if (collision.collisionPoint.y < bestFloor.collisionPoint.y)
            {
                bestFloor = collision;
            }
        }

        newPos.y = bestFloor.collisionPoint.y - GetCollider()->GetHeight() * 0.5f;
        if (vVelocity.y > 0) vVelocity.y = 0; // 아래로 떨어지는 속도만 제거
        bIsGround = true;
        bOnStair = false;
        vHitNormal = bestFloor.vHitNormal;

        // 플랫폼인지 확인
        if (bestFloor.collisionLayer == ECollisionLayer::PLATFORM)
        {
            bIsPlatform = true;
        }
    }

    // 3. 벽 충돌 처리
    if (!wallCollisions.empty())
    {
        // 가장 가까운 벽을 찾음
        CollisionInfo bestWall = wallCollisions[0];
        float minPenetration = bestWall.penetrationDepth;

        for (const auto& collision : wallCollisions)
        {
            if (collision.penetrationDepth < minPenetration)
            {
                minPenetration = collision.penetrationDepth;
                bestWall = collision;
            }
        }

        float halfWidth = GetCollider()->GetWidth() * 0.5f;
        if (bestWall.vHitNormal.x > 0) // 오른쪽 벽
        {
            newPos.x = bestWall.collisionPoint.x + halfWidth;
        }
        else // 왼쪽 벽
        {
            newPos.x = bestWall.collisionPoint.x - halfWidth;
        }

        if ((bestWall.vHitNormal.x > 0 && vVelocity.x < 0) ||
            (bestWall.vHitNormal.x < 0 && vVelocity.x > 0))
        {
            vVelocity.x = 0; // 벽 방향으로 이동하는 속도만 제거
        }

        bIsWall = true;
        vHitNormal = bestWall.vHitNormal;
    }
}

void Player::ProcessPlatformCollisions(const vector<CollisionInfo>& platformCollisions, Vector2& newPos)
{
    // 플랫폼은 아래에서 위로 올라갈 때만 충돌 처리 (위에서 아래로는 통과)
    if (vVelocity.y <= 0) // 위로 올라가거나 정지 상태면 플랫폼 무시
        return;

    // 가장 높은 플랫폼을 찾음
    CollisionInfo bestPlatform;
    bool hasPlatformCollision = false;
    float bestPlatformY = FLT_MAX;

    for (const auto& collision : platformCollisions)
    {
        // 플랫폼은 위쪽 면만 충돌 처리 (법선이 위쪽을 향하는 경우만)
        if (collision.vHitNormal.y < -0.5f && collision.collisionPoint.y < bestPlatformY)
        {
            bestPlatformY = collision.collisionPoint.y;
            bestPlatform = collision;
            hasPlatformCollision = true;
        }
    }

    if (hasPlatformCollision)
    {
        // 플랫폼 위에 착지
        newPos.y = bestPlatform.collisionPoint.y - GetCollider()->GetHeight() * 0.5f;
        vVelocity.y = 0; // 아래로 떨어지는 속도 제거
        bIsPlatform = true;
        bIsGround = true; // 플랫폼도 바닥으로 취급
        bOnStair = false;
        vHitNormal = bestPlatform.vHitNormal;

        // 점프 관련 상태 초기화 (중요!)
        bIsJumped = false;
        bIsMaxJump = false;
        fJumpPressedTime = 0.f;
    }
}

void Player::ProcessWallCollisions(const vector<CollisionInfo>& wallCollisions, Vector2& newPos)
{
    for (const auto& collision : wallCollisions)
    {
        if (fabs(collision.vHitNormal.x) > 0.5f)
        {
            float halfWidth = GetCollider()->GetWidth() * 0.5f;
            // 벽 방향에 따른 위치 조정
            if (collision.vHitNormal.x < 0) // 오른쪽 벽
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
            vHitNormal = collision.vHitNormal;
        }
    }
}

void Player::ProcessStairCollisions(const vector<CollisionInfo>& stairCollisions, Vector2 oldPos, Vector2& newPos)
{
    // 가장 가까운 계단 충돌 선택
    CollisionInfo best = stairCollisions[0];
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

    float halfW = GetCollider()->GetWidth() * 0.5f;
    float halfH = GetCollider()->GetHeight() * 0.5f;

    // 계단 방향 판단
    Collider* stairCollider = best.groundActor->GetCollider();
    Vector2 stairStart = stairCollider->GetStartPoint();
    Vector2 stairEnd = stairCollider->GetEndPoint();
    Vector2 stairDir = stairEnd - stairStart;

    // 계단이 우상향인지 좌상향인지 판단
    bool isRightUpStair = (stairDir.x > 0 && stairDir.y < 0) || (stairDir.x < 0 && stairDir.y > 0);

    // 올바른 모서리 선택
    Vector2 targetCorner;
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
    bool inSegment;
    CollisionManager::GetInstance()->GetYOnLineAtX(stairStart, stairEnd, targetCorner.x, stairYAtCorner, inSegment);

    if (!inSegment)
    {
        // 모서리가 계단 범위를 벗어나면 계단에서 벗어난 것으로 처리
        bOnStair = false;
        return;
    }

    // 플레이어를 계단에 맞춤 (선택된 모서리가 계단에 정확히 닿도록)
    float deltaY = stairYAtCorner - targetCorner.y;
    newPos.y += deltaY;

    // 계단 상태 설정
    bOnStair = true;
    bIsGround = true;
    bIsJumped = false;
    bIsMaxJump = false;

    // 계단 법선 저장
    Vector2 stairNormal = Vector2(-stairDir.y, stairDir.x).GetNormalize();
    if (stairNormal.y > 0) stairNormal *= -1; // 위쪽을 향하도록
    vHitNormal = stairNormal;

    if (vAcceleration.x != 0.f) // 실제로 이동 중일 때만
    {
        Vector2 stairDirection = stairDir.GetNormalize(); // 계단 방향 단위벡터

        // 현재 수평 속도를 그대로 유지하되, 계단 방향으로 변환
        float currentHorizontalSpeed = abs(vVelocity.x);

        // 계단의 기울기를 고려한 속도 보정
        float stairSlope = abs(stairDirection.y / stairDirection.x); // 기울기
        float speedMultiplier = 1.0f / sqrt(1.0f + stairSlope * stairSlope); // 빗변 보정

        // 실제로는 더 빠르게 만들어서 바닥과 비슷한 속도감 제공
        speedMultiplier *= 1.2f; // 20% 속도 증가로 자연스러운 이동감

        // 이동 방향에 따른 속도 설정
        if ((vVelocity.x > 0 && stairDirection.x > 0) || (vVelocity.x < 0 && stairDirection.x < 0))
        {
            // 계단 방향으로 이동
            vVelocity = stairDirection * currentHorizontalSpeed * speedMultiplier;
        }
        else if ((vVelocity.x > 0 && stairDirection.x < 0) || (vVelocity.x < 0 && stairDirection.x > 0))
        {
            // 계단 반대 방향으로 이동
            vVelocity = stairDirection * (-currentHorizontalSpeed) * speedMultiplier;
        }

        // 계단으로 파고드는 속도 성분 제거
        float normalDot = vVelocity.Dot(stairNormal);
        if (normalDot < 0)
            vVelocity -= stairNormal * normalDot;

        //// 현재 수평 속도를 계단 방향으로 투영
        //float horizontalSpeed = vVelocity.x;
        //float stairSpeed = horizontalSpeed * stairDirection.x;

        //// 계단 방향의 속도 벡터 생성
        //vVelocity = stairDirection * stairSpeed;

        //// 계단으로 파고드는 속도 성분 제거
        //float normalDot = vVelocity.Dot(stairNormal);
        //if (normalDot < 0)
        //    vVelocity -= stairNormal * normalDot;
    }
    else
    {
        // 입력이 없을 때는 빠르게 정지
        vVelocity *= 0.3f; // 강한 마찰력으로 빠르게 감속

        if (vVelocity.Length() < 20.0f)
        {
            vVelocity = Vector2(0, 0); // 완전 정지
        }
    }

    //// 속도 보정: 계단으로 파고드는 성분 제거
    //float vDot = vVelocity.Dot(stairNormal);
    //if (vDot < 0)
    //    vVelocity -= stairNormal * vDot;

    // 내려갈 때 계단에 붙도록 중력 방향 조정
    //Vector2 gravityAlongStair = vGravity - stairNormal * vGravity.Dot(stairNormal);
    //vAcceleration += gravityAlongStair * 0.3f; // 계단을 따라 내려가는 추가 가속도
}

float Player::GetLowestCollisionY(const vector<CollisionInfo>& cols)
{
    float minY = FLT_MAX;
    for (auto& c : cols)
        minY = min(minY, c.collisionPoint.y);
    return minY;
}

void Player::OnCollisionBeginOverlap(const CollisionInfo& info)
{
  switch (info.collisionLayer)
    {
    case ECollisionLayer::GROUND:
        bIsGround = true;
        vNewPos.y = info.collisionPoint.y - GetCollider()->GetHeight() * 0.5f;
        vVelocity.y = 0;
        vHitNormal = info.vHitNormal;
        break;
    case ECollisionLayer::PLATFORM:
        if (vVelocity.y >= 0)
        {
            bIsPlatform = true;
            vNewPos.y = info.collisionPoint.y - GetCollider()->GetHeight() * 0.5f;
            vVelocity.y = 0;
            vHitNormal = info.vHitNormal;
        }
        break;
    case ECollisionLayer::WALL:
        bIsWall = true;
        break;
    case ECollisionLayer::CEILING:
        break;
    case ECollisionLayer::STAIR:
        {   
            Collider* other = info.groundActor->GetCollider();
            float y1 = other->GetStartPoint().y;
            float y2 = other->GetEndPoint().y;
            float height = (y1 > y2) ? y1 : y2;
            if (!bIsGround && !bIsPlatform)
            {
                bOnStair = true;
            }
            else if ((bIsGround || bIsPlatform) && height > GetPos().y + GetCollider()->GetHeight() * 0.5f)
            {
                bOnStair = true;
            }
        }
        break;
    default:
        break;
    }
}

void Player::OnCollisionStayOverlap(const CollisionInfo& info)
{
    switch (info.collisionLayer)
    {
    case ECollisionLayer::GROUND:
        bIsGround = true;
        vNewPos.y = info.collisionPoint.y - GetCollider()->GetHeight() * 0.5f;
        vVelocity.y = 0;
        vHitNormal = info.vHitNormal;
        break;
    case ECollisionLayer::PLATFORM:
        if (vVelocity.y >= 0)
        {
            bIsPlatform = true;
            vNewPos.y = info.collisionPoint.y - GetCollider()->GetHeight() * 0.5f;
            vVelocity.y = 0;
            vHitNormal = info.vHitNormal;
        }
        break;
    case ECollisionLayer::WALL:
        bIsWall = true;
        break;
    case ECollisionLayer::CEILING:
        break;
    case ECollisionLayer::STAIR:
    {
        Collider* other = info.groundActor->GetCollider();
        float y1 = other->GetStartPoint().y;
        float y2 = other->GetEndPoint().y;
        float height = (y1 > y2) ? y1 : y2;
        if (!bIsGround && !bIsPlatform)
        {
            bOnStair = true;
        }
        else if ((bIsGround || bIsPlatform) && height > GetPos().y + GetCollider()->GetHeight() * 0.5f)
        {
            bOnStair = true;
        }
    }
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
        bIsGround = false;
        break;
    case ECollisionLayer::PLATFORM:
        bIsPlatform = false;
        break;
    case ECollisionLayer::WALL:
        bIsWall = false;
        break;
    case ECollisionLayer::CEILING:
        break;
    case ECollisionLayer::STAIR:
        bOnStair = false;
        break;
    default:
        break;
    }
}

