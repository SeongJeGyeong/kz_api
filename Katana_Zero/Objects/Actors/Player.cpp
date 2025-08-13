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
    // �߷��� ���� ���� ���Ϳ� �ӵ��� �����Ͽ�, �߷� ������ ���� ���� ���͸� ����.
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
        // �߷� ���ӵ� ����
        vAcceleration += vGravity;
    }

    // �ְ��� ��ó���� ü�� ȿ���� ���� �������� �ùķ��̼�
    if (vVelocity.y < 0) // ��� ���� ��
    {
        // �ӵ��� ���������� �������׵� ���� (�ڿ������� ����)
        float airResistance = fAirResistance * (vVelocity.y * vVelocity.y) / (fJumpInitialVelocity * fJumpInitialVelocity);
        vAcceleration.y += airResistance;
    }

    // ���ӵ��� �ӵ�(velocity)�� ��ȭ��Ų��.
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

    // �������� velocity
    // v : (3,4), a :(3,0), b : (0,4)
    // v = a + b
    vVelocity = gravityVector + sideVec;

    // �ӵ�(velocity) ��ġ�� ��ȭ��Ų��.
    vNewPos = vCurPos + (vVelocity * deltaTime);
}

void Player::UpdatePosition(float deltaTime)
{
    // �浹 �˻� ���� ���� ���� ����
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
    vVelocity.y = -fJumpInitialVelocity; // ������ ���� ����
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

    // �÷��̾��� �ĸ��� ������ ���, ������ ��������Ʈ�� ����ϹǷ�
    // ���� ������ ��Ī�� �Ǵ� ������ ��ȯ�ϵ��� -180�� ����
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

    // �浹 ����� �������� �з�
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

    // 1. �ٴ�/�÷��� �浹 ó�� (���� �̵� ����)
    if (!groundCollisions.empty())
    {
        ProcessGroundCollisions(groundCollisions, newPos);
    }

    if (!platformCollisions.empty())
    {
        ProcessPlatformCollisions(platformCollisions, newPos);
    }

    // 2. �� �浹 ó�� (���� �̵� ����)
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
    // �浹�� ���⺰�� �з�
    vector<CollisionInfo> floorCollisions;   // �ٴ� (������ �Ʒ���)
    vector<CollisionInfo> ceilingCollisions; // õ�� (�Ʒ����� ����)
    vector<CollisionInfo> wallCollisions;    // �� (�¿�)

    for (const auto& collision : groundCollisions)
    {
        switch (collision.hitCorner)
        {
        case 0: // �ٴ� �浹
            floorCollisions.push_back(collision);
            break;
        case 3: // õ�� �浹
            ceilingCollisions.push_back(collision);
            break;
        case 1: // ���� �� �浹
        case 2: // ������ �� �浹
            wallCollisions.push_back(collision);
            break;
        }
    }

    // 1. õ�� �浹 ó�� (���� �켱)
    if (!ceilingCollisions.empty())
    {
        // ���� ���� õ���� ã��
        CollisionInfo bestCeiling = ceilingCollisions[0];
        for (const auto& collision : ceilingCollisions)
        {
            if (collision.collisionPoint.y > bestCeiling.collisionPoint.y)
            {
                bestCeiling = collision;
            }
        }

        newPos.y = bestCeiling.collisionPoint.y + GetCollider()->GetHeight() * 0.5f;
        if (vVelocity.y < 0) vVelocity.y = 0; // ���� �ö󰡴� �ӵ��� ����
        vHitNormal = bestCeiling.vHitNormal;
        return; // õ�� �浹�� ������ �ٸ� �浹�� ����
    }

    // 2. �ٴ� �浹 ó��
    if (!floorCollisions.empty())
    {
        // ���� ���� �ٴ��� ã��
        CollisionInfo bestFloor = floorCollisions[0];
        for (const auto& collision : floorCollisions)
        {
            if (collision.collisionPoint.y < bestFloor.collisionPoint.y)
            {
                bestFloor = collision;
            }
        }

        newPos.y = bestFloor.collisionPoint.y - GetCollider()->GetHeight() * 0.5f;
        if (vVelocity.y > 0) vVelocity.y = 0; // �Ʒ��� �������� �ӵ��� ����
        bIsGround = true;
        bOnStair = false;
        vHitNormal = bestFloor.vHitNormal;

        // �÷������� Ȯ��
        if (bestFloor.collisionLayer == ECollisionLayer::PLATFORM)
        {
            bIsPlatform = true;
        }
    }

    // 3. �� �浹 ó��
    if (!wallCollisions.empty())
    {
        // ���� ����� ���� ã��
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
        if (bestWall.vHitNormal.x > 0) // ������ ��
        {
            newPos.x = bestWall.collisionPoint.x + halfWidth;
        }
        else // ���� ��
        {
            newPos.x = bestWall.collisionPoint.x - halfWidth;
        }

        if ((bestWall.vHitNormal.x > 0 && vVelocity.x < 0) ||
            (bestWall.vHitNormal.x < 0 && vVelocity.x > 0))
        {
            vVelocity.x = 0; // �� �������� �̵��ϴ� �ӵ��� ����
        }

        bIsWall = true;
        vHitNormal = bestWall.vHitNormal;
    }
}

void Player::ProcessPlatformCollisions(const vector<CollisionInfo>& platformCollisions, Vector2& newPos)
{
    // �÷����� �Ʒ����� ���� �ö� ���� �浹 ó�� (������ �Ʒ��δ� ���)
    if (vVelocity.y <= 0) // ���� �ö󰡰ų� ���� ���¸� �÷��� ����
        return;

    // ���� ���� �÷����� ã��
    CollisionInfo bestPlatform;
    bool hasPlatformCollision = false;
    float bestPlatformY = FLT_MAX;

    for (const auto& collision : platformCollisions)
    {
        // �÷����� ���� �鸸 �浹 ó�� (������ ������ ���ϴ� ��츸)
        if (collision.vHitNormal.y < -0.5f && collision.collisionPoint.y < bestPlatformY)
        {
            bestPlatformY = collision.collisionPoint.y;
            bestPlatform = collision;
            hasPlatformCollision = true;
        }
    }

    if (hasPlatformCollision)
    {
        // �÷��� ���� ����
        newPos.y = bestPlatform.collisionPoint.y - GetCollider()->GetHeight() * 0.5f;
        vVelocity.y = 0; // �Ʒ��� �������� �ӵ� ����
        bIsPlatform = true;
        bIsGround = true; // �÷����� �ٴ����� ���
        bOnStair = false;
        vHitNormal = bestPlatform.vHitNormal;

        // ���� ���� ���� �ʱ�ȭ (�߿�!)
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
            // �� ���⿡ ���� ��ġ ����
            if (collision.vHitNormal.x < 0) // ������ ��
            {
                newPos.x = collision.collisionPoint.x - halfWidth;
                //newPos.x -= 0.001f;
            }
            else // ���� ��
            {
                newPos.x = collision.collisionPoint.x + halfWidth;
                //newPos.x += 0.001f;
            }

            // ������ �浹 �� ���� �ӵ��� ����
            vVelocity.x = 0;
            bIsWall = true;
            vHitNormal = collision.vHitNormal;
        }
    }
}

void Player::ProcessStairCollisions(const vector<CollisionInfo>& stairCollisions, Vector2 oldPos, Vector2& newPos)
{
    // ���� ����� ��� �浹 ����
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

    // ��� ���� �Ǵ�
    Collider* stairCollider = best.groundActor->GetCollider();
    Vector2 stairStart = stairCollider->GetStartPoint();
    Vector2 stairEnd = stairCollider->GetEndPoint();
    Vector2 stairDir = stairEnd - stairStart;

    // ����� ��������� �»������� �Ǵ�
    bool isRightUpStair = (stairDir.x > 0 && stairDir.y < 0) || (stairDir.x < 0 && stairDir.y > 0);

    // �ùٸ� �𼭸� ����
    Vector2 targetCorner;
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
    bool inSegment;
    CollisionManager::GetInstance()->GetYOnLineAtX(stairStart, stairEnd, targetCorner.x, stairYAtCorner, inSegment);

    if (!inSegment)
    {
        // �𼭸��� ��� ������ ����� ��ܿ��� ��� ������ ó��
        bOnStair = false;
        return;
    }

    // �÷��̾ ��ܿ� ���� (���õ� �𼭸��� ��ܿ� ��Ȯ�� �굵��)
    float deltaY = stairYAtCorner - targetCorner.y;
    newPos.y += deltaY;

    // ��� ���� ����
    bOnStair = true;
    bIsGround = true;
    bIsJumped = false;
    bIsMaxJump = false;

    // ��� ���� ����
    Vector2 stairNormal = Vector2(-stairDir.y, stairDir.x).GetNormalize();
    if (stairNormal.y > 0) stairNormal *= -1; // ������ ���ϵ���
    vHitNormal = stairNormal;

    if (vAcceleration.x != 0.f) // ������ �̵� ���� ����
    {
        Vector2 stairDirection = stairDir.GetNormalize(); // ��� ���� ��������

        // ���� ���� �ӵ��� �״�� �����ϵ�, ��� �������� ��ȯ
        float currentHorizontalSpeed = abs(vVelocity.x);

        // ����� ���⸦ ����� �ӵ� ����
        float stairSlope = abs(stairDirection.y / stairDirection.x); // ����
        float speedMultiplier = 1.0f / sqrt(1.0f + stairSlope * stairSlope); // ���� ����

        // �����δ� �� ������ ���� �ٴڰ� ����� �ӵ��� ����
        speedMultiplier *= 1.2f; // 20% �ӵ� ������ �ڿ������� �̵���

        // �̵� ���⿡ ���� �ӵ� ����
        if ((vVelocity.x > 0 && stairDirection.x > 0) || (vVelocity.x < 0 && stairDirection.x < 0))
        {
            // ��� �������� �̵�
            vVelocity = stairDirection * currentHorizontalSpeed * speedMultiplier;
        }
        else if ((vVelocity.x > 0 && stairDirection.x < 0) || (vVelocity.x < 0 && stairDirection.x > 0))
        {
            // ��� �ݴ� �������� �̵�
            vVelocity = stairDirection * (-currentHorizontalSpeed) * speedMultiplier;
        }

        // ������� �İ��� �ӵ� ���� ����
        float normalDot = vVelocity.Dot(stairNormal);
        if (normalDot < 0)
            vVelocity -= stairNormal * normalDot;

        //// ���� ���� �ӵ��� ��� �������� ����
        //float horizontalSpeed = vVelocity.x;
        //float stairSpeed = horizontalSpeed * stairDirection.x;

        //// ��� ������ �ӵ� ���� ����
        //vVelocity = stairDirection * stairSpeed;

        //// ������� �İ��� �ӵ� ���� ����
        //float normalDot = vVelocity.Dot(stairNormal);
        //if (normalDot < 0)
        //    vVelocity -= stairNormal * normalDot;
    }
    else
    {
        // �Է��� ���� ���� ������ ����
        vVelocity *= 0.3f; // ���� ���������� ������ ����

        if (vVelocity.Length() < 20.0f)
        {
            vVelocity = Vector2(0, 0); // ���� ����
        }
    }

    //// �ӵ� ����: ������� �İ��� ���� ����
    //float vDot = vVelocity.Dot(stairNormal);
    //if (vDot < 0)
    //    vVelocity -= stairNormal * vDot;

    // ������ �� ��ܿ� �ٵ��� �߷� ���� ����
    //Vector2 gravityAlongStair = vGravity - stairNormal * vGravity.Dot(stairNormal);
    //vAcceleration += gravityAlongStair * 0.3f; // ����� ���� �������� �߰� ���ӵ�
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

