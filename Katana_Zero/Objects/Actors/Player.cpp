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
    // �߷��� ���� ���� ���Ϳ� �ӵ��� �����Ͽ�, �߷� ������ ���� ���� ���͸� ����.
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
        // �߷� ���ӵ� ����
        vAcceleration += vGravity;

        // �ְ��� ��ó���� ü�� ȿ���� ���� �������� �ùķ��̼�
        if (vVelocity.y < 0) // ��� ���� ��
        {
            // �ӵ��� ���������� �������׵� ���� (�ڿ������� ����)
            float airResistance = fAirResistance * (vVelocity.y * vVelocity.y) / (fJumpInitialVelocity * fJumpInitialVelocity);
            vAcceleration.y += airResistance;
        }
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

    float friction = bIsGround ? 0.85f : 0.98f;
    sideVec *= friction;
    if (sideVec.Length() < 1.0f)
        sideVec = Vector2(0.f, 0.f);

    // �������� velocity
    // v : (3,4), a :(3,0), b : (0,4)
    // v = a + b
    vVelocity = gravityVector + sideVec;

    // �ӵ�(velocity) ��ġ�� ��ȭ��Ų��.
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

    // �ش� ���ο� ��ġ�� �����ϱ� ����. �浹üũ�� �ѹ��ؼ� ������ Ȯ��
    SetPos(newPos);
}

void Player::Jump()
{
    vVelocity.y = -fJumpInitialVelocity; // ������ ���� ����
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

    // �浹 ����� �������� �з�
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

    // 1. �ٴ�/�÷��� �浹 ó�� (���� �̵� ����)
    if (!groundCollisions.empty() || !platformCollisions.empty())
    {
        ProcessGroundCollisions(groundCollisions, platformCollisions, newPos);
    }

    // 2. �� �浹 ó�� (���� �̵� ����)
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

            // �÷����� ��ܺ��� ���ų� ������ ��� ����
            if (platY <= stairY)
                ignoreStair = true;
        }

        if (!ignoreStair)
        {
            // 3. ��� �浹 ó�� (���� ���� �̵�)
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

    // ���� ����(���� ���� Y��) �ٴ� ã��
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

    // �÷����� Ȯ�� (�Ʒ����� ���� �ö� ����)
    if (vVelocity.y > 0) // �Ʒ��� ������ ���� �÷��� �浹 ó��
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
            // �� ���⿡ ���� ��ġ ����
            if (collision.normal.x < 0) // ������ ��
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
            vHitNormal = collision.normal;
        }
    }
}

void Player::ProcessStairCollisions(const vector<PlayerGroundCollisionResult>& stairCollisions, Vector2 oldPos, Vector2& newPos)
{
    // ���� ����� ��� �浹 ����
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

    // ���õ� �𼭸��� ��ܿ� ����
    float deltaY = best.collisionPoint.y - (newPos.y + halfH);
    newPos.y += deltaY;

    // ��� ���� ����
    bIsStair = true;
    bIsGround = true;
    bIsJumped = false;
    bIsMaxJump = false;

    // ��� ���� ����
    vHitNormal = best.normal;

    // �ӵ� ����: ��� �������� �İ��� ���� ����
    float vDot = vVelocity.Dot(best.normal);
    if (vDot < 0)
        vVelocity -= best.normal * vDot;

    //// ���� ����� ��� �浹 ����
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

    //// ��� �󿡼� "������ �𼭸� X ��ǥ" ���
    //float cornerX = newPos.x; // �⺻: �߾�
    //if (bestStairCollision.hitCorner == 1)
    //{
    //    cornerX = newPos.x - halfWidth;
    //}
    //else if (bestStairCollision.hitCorner == 2)
    //{
    //    cornerX = newPos.x + halfWidth;
    //}

    //// ��� ���� �󿡼� cornerX������ Y ���
    //float stairYAtCorner = 0.0f; bool inSegment = false;

    //// line �� �ʿ��ϹǷ� groundActor(��� Collider)�� ����� �������� start/end�� ������ ��.
    //Collider* stairCollider = static_cast<Collider*>(bestStairCollision.groundActor->GetCollider());
    //if (stairCollider)
    //{
    //    Vector2 start = stairCollider->GetStartPoint();
    //    Vector2 end = stairCollider->GetEndPoint();
    //    CollisionManager::GetInstance()->GetYOnLineAtX(start, end, cornerX, stairYAtCorner, inSegment);
    //}

    //// �𼭸� X�� ��� ���� ���� ���̸� ��ܿ��� ��� ������ �����ϰ� ����
    //if (!inSegment)
    //{
    //    bIsStair = false;
    //    return;
    //    // fallback: collision point �������� ��ġ
    //    //Vector2 desiredBottomPos = stairCollisionPoint - stairNormal * 1.0f;
    //    //newPos.y = desiredBottomPos.y - halfHeight;
    //}
    //else
    //{
    //    // corner�� Y�� �������� �÷��̾ ��ܿ� ����
    //    Vector2 desiredBottomPos = Vector2(cornerX, stairYAtCorner) - stairNormal * 1.0f; // �ణ ���� ���

    //    // newPos.x�� �ǵ帮�� �ʰ� Y�� ��ܿ� �����
    //    newPos.y = desiredBottomPos.y - halfHeight;
    //}

    //// �ӵ� ����: ��ܿ� ������ �ʵ��� ���� ���� ����
    //float velocityNormalComp = vVelocity.Dot(stairNormal);
    //if (velocityNormalComp > 0) // y�� �������� ���� ���������� ����(��ǥü�迡 ���� ���� �ʿ�)
    //{
    //    // ��� �����̸� �������� ���� (������ �Ʒ��� ���ϴ� �߷��� ����)
    //}
    //else
    //{
    //    // ������� �İ��� ���� ������ ����
    //    vVelocity -= stairNormal * velocityNormalComp;
    //}

    //// �߷� ����: ��� ���� ���� ���� ���� (��ܿ� �ٵ���)
    //float gravityNormalComp = vGravity.Dot(stairNormal);
    //if (gravityNormalComp < 0)
    //{
    //    vAcceleration -= stairNormal * gravityNormalComp;
    //}

    //// �ٴ� ���� ����
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

