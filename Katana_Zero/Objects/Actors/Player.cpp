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
        animator->AddAnimation(EPlayerState::PLAYER_RUN, ResourceManager::GetInstance()->GetSprite("spr_run"), { 0, -5 });
        animator->AddAnimation(EPlayerState::PLAYER_RUN_TO_IDLE, ResourceManager::GetInstance()->GetSprite("spr_run_to_idle"), { 0, -11 });
        animator->AddAnimation(EPlayerState::PLAYER_PRECROUCH, ResourceManager::GetInstance()->GetSprite("spr_precrouch"), { 0, -13 });
        animator->AddAnimation(EPlayerState::PLAYER_CROUCH, ResourceManager::GetInstance()->GetSprite("spr_crouch"), { 0, -13 });
        animator->AddAnimation(EPlayerState::PLAYER_POSTCROUCH, ResourceManager::GetInstance()->GetSprite("spr_postcrouch"), { 0, -13 });
        animator->AddAnimation(EPlayerState::PLAYER_JUMP, ResourceManager::GetInstance()->GetSprite("spr_jump"), { -10, 0 });
        animator->AddAnimation(EPlayerState::PLAYER_FALL, ResourceManager::GetInstance()->GetSprite("spr_fall"), { -10, -5 });
        animator->AddAnimation(EPlayerState::PLAYER_ATTACK, ResourceManager::GetInstance()->GetSprite("spr_attack"));
        animator->AddAnimation(EPlayerState::PLAYER_ROLL, ResourceManager::GetInstance()->GetSprite("spr_roll"), { 0, -10 });
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
    _stateMachine->ChangeState(EPlayerState::PLAYER_IDLE);

    _movementComp = _components.GetComponent<PlayerMovementComponent>();
    if (_movementComp != nullptr) _movementComp->InitComponent(this);
}

void Player::Update(float deltaTime)
{
    fAttackDelayTime += deltaTime;
    _stateMachine->Update(deltaTime);

    _movementComp->ApplyPhysics(deltaTime);
    Vector2 velocity = _movementComp->GetVelocity();
    // �ӵ�(velocity) ��ġ�� ��ȭ��Ų��.
    Vector2 NewPos = GetPos() + (velocity * deltaTime);
    _movementComp->SetNewPos(NewPos);
}

void Player::PostUpdate(float deltaTime)
{
    SetPos(_movementComp->GetNewPos());
    _movementComp->SetAcceleration({ 0,0 });
}

void Player::Render(HDC hdc)
{
    _components.RenderComponents(hdc);
	Super::Render(hdc);

    SetTextColor(hdc, RGB(255, 255, 255));

    //wstring str = std::format(L"IsGround({0}) IsJumped({1}) IsMaxJump({2}) bIsStair({3}) bIsPlatform({4}) bIsWall({5})", bIsGround, bIsJumped, bIsMaxJump, bOnStair, bIsPlatform, bIsWall);
    //::TextOut(hdc, 450, 30, str.c_str(), static_cast<int32>(str.size()));

    printState(hdc);
    wstring strs = std::format(L"bIsCrouch({0})", bIsCrouch);
    ::TextOut(hdc, 450, 50, strs.c_str(), static_cast<int32>(strs.size()));

    wstring str2 = std::format(L"Pos({0}, {1})", GetPos().x, GetPos().y);
    ::TextOut(hdc, 100, 70, str2.c_str(), static_cast<int32>(str2.size()));

    //wstring str5 = std::format(L"velocity ( {0}, {1} )", vVelocity.x, vVelocity.y);
    //::TextOut(hdc, 100, 120, str5.c_str(), static_cast<int32>(str5.size()));

    if (bIsAttack)
    {
        Vector2 pos = _components.GetComponent<CameraComponent>()->ConvertScreenPos(GetPos());
        RenderHitbox(hdc, pos, fAttackRadian, 1.5f);
    }
}

//void Player::ApplyPhysics(float deltaTime)
//{
//    // �߷��� ���� ���� ���Ϳ� �ӵ��� �����Ͽ�, �߷� ������ ���� ���� ���͸� ����.
//    Vector2 normalGravity = vGravity.GetNormalize();
//    float GravityLength = vVelocity.Dot(normalGravity);
//
//    if (bIsGround || bIsPlatform)
//    {
//        if(vVelocity.y >= 0.f) bIsJumped = false;
//        vVelocity.y = 0.f;
//        GravityLength = 0.f;
//    }
//    else if (bOnStair)
//    {
//        bIsJumped = false;
//        vVelocity -= normalGravity * GravityLength;
//    }
//    else
//    {
//        // �߷� ���ӵ� ����
//        vAcceleration += vGravity;
//    }
//
//    if (_components.GetComponent<InputComponent>()->GetPressedDown()) vAcceleration.y += 10000.f;
//
//    // �ְ��� ��ó���� ü�� ȿ���� ���� �������� �ùķ��̼�
//    if (vVelocity.y < 0) // ��� ���� ��
//    {
//        // �ӵ��� ���������� �������׵� ���� (�ڿ������� ����)
//        float airResistance = fAirResistance * (vVelocity.y * vVelocity.y) / (fJumpInitialVelocity * fJumpInitialVelocity);
//        vAcceleration.y += airResistance;
//    }
//
//    // ���ӵ��� �ӵ�(velocity)�� ��ȭ��Ų��.
//    vVelocity += vAcceleration * deltaTime;
//
//    float upFactor = 400.f;
//    float sideFactor = 400.f;
//    if (GetCurrentState() == EPlayerState::PLAYER_ROLL) sideFactor = 800.f;
//
//    Vector2 gravityVector = normalGravity * GravityLength;
//    Vector2 sideVec = vVelocity - gravityVector;
//    float sideLength = sideVec.Length();
//
//    if (GravityLength > upFactor) gravityVector = normalGravity * upFactor;
//
//    if (sideLength > sideFactor) sideVec = sideVec.GetNormalize() * sideFactor;
//    else if (sideLength < -sideFactor) sideVec = sideVec.GetNormalize() * sideFactor;
//
//    float friction = 0.8f;
//    if (GetCurrentState() == EPlayerState::PLAYER_ATTACK || GetCurrentState() == EPlayerState::PLAYER_ROLL)
//    {
//        friction = 0.98f;
//        gravityVector *= friction;
//    }
//
//    sideVec *= friction;
//    if (sideVec.Length() < 1.0f)
//        sideVec = Vector2(0.f, 0.f);
//
//    // �������� velocity
//    // v : (3,4), a :(3,0), b : (0,4)
//    // v = a + b
//    vVelocity = gravityVector + sideVec;
//
//    // �ӵ�(velocity) ��ġ�� ��ȭ��Ų��.
//    vNewPos = GetPos() + (vVelocity * deltaTime);
//}

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
    if ((_movementComp->GetIsGround() || _movementComp->GetIsPlatform() || _movementComp->GetOnStair()) && _movementComp->GetAcceleration().x != 0.f)
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
            //vVelocity.x += 100000.f;
            _components.GetComponent<Animator>()->SetFlipped(false);
            vFrontDir = { 1, 0 };
        }
    }
    else
    {
        if (vHitNormal.x < 1.f)
        {
            _movementComp->AddVelocity({ -100000.f, 0 });
            //vVelocity.x -= 100000.f;
            _components.GetComponent<Animator>()->SetFlipped(true);
            vFrontDir = { -1, 0 };
        }
    }
    bIsCrouch = false;
    _stateMachine->ChangeState(EPlayerState::PLAYER_ROLL);
}

void Player::Attack()
{
    Vector2 mousePos = InputManager::GetInstance()->GetMousePos();
    Vector2 worldPos = _components.GetComponent<CameraComponent>()->ConvertWorldPos(mousePos);
    Vector2 pos = GetPos();
    Vector2 dir = worldPos - pos;
    dir.Normalize();

    float rad = atan2(dir.y, dir.x);

    // �÷��̾��� �ĸ��� ������ ���, ������ ��������Ʈ�� ����ϹǷ�
    // ���� ������ ��Ī�� �Ǵ� ������ ��ȯ�ϵ��� -180�� ����
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
    bIsAttack = true;
    fAttackRadian = rad;
}

int32 Player::GetCurrentState()
{
    return _stateMachine->GetCurrentStateType();
}

void Player::ChangeState(EPlayerState stateType)
{
    _stateMachine->ChangeState(stateType);
}

void Player::ProcessGroundCollision(const CollisionInfo& collisionInfo)
{
    switch (collisionInfo.hitCorner)
    {
    case 0: // �ٴ� �浹
        ProcessGroundFloor(collisionInfo);
        break;
    case 3: // õ�� �浹
        ProcessGroundCeiling(collisionInfo);
        break;
    case 1: // ���� �� �浹
    case 2: // ������ �� �浹
        ProcessGroundWall(collisionInfo);
        break;
    }
}

void Player::ProcessGroundFloor(const CollisionInfo& collisionInfo)
{
    _movementComp->SetNewPosY(collisionInfo.collisionPoint.y - GetCollider()->GetHeight() * 0.5f);
    //newPos.y = collisionInfo.collisionPoint.y - GetCollider()->GetHeight() * 0.5f;
    _movementComp->SetVelocityY(0.f);
    _movementComp->SetIsGround(true);
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
    if (collisionInfo.vHitNormal.x > 0) // ���� ��
    {
        _movementComp->SetNewPosX(collisionInfo.collisionPoint.x + halfWidth);
        //newPos.x = collisionInfo.collisionPoint.x + halfWidth;
    }
    else if(collisionInfo.vHitNormal.x < 0)// ������ ��
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

    // �÷����� �Ʒ����� ���� �ö� ���� �浹 ó�� (������ �Ʒ��δ� ���)
    if (velocity.y < 0) // ���� �ö󰡰ų� ���� ���¸� �÷��� ����
        return;

    // �÷����� ���� �鸸 �浹 ó�� (������ ������ ���ϴ� ��츸)
    if (collisionInfo.vHitNormal.y < -0.5f)
    {
        // �÷��� ���� ����
        _movementComp->SetNewPosY(collisionInfo.collisionPoint.y - GetCollider()->GetHeight() * 0.5f);
        //newPos.y = collisionInfo.collisionPoint.y - GetCollider()->GetHeight() * 0.5f;
        velocity.y = 0; // �Ʒ��� �������� �ӵ� ����
        _movementComp->SetVelocity(velocity);
        _movementComp->SetIsPlatform(true);

        vHitNormal = collisionInfo.vHitNormal;

        // ���� ���� ���� �ʱ�ȭ
        _movementComp->SetIsJumped(false);
        bIsMaxJump = false;
        fJumpPressedTime = 0.f;
    }
}

void Player::ProcessWallCollision(const CollisionInfo& collisionInfo)
{
    float halfWidth = GetCollider()->GetWidth() * 0.5f;
    // �� ���⿡ ���� ��ġ ����
    if (collisionInfo.vHitNormal.x < 0) // ������ ��
    {
        _movementComp->SetNewPosX(collisionInfo.collisionPoint.x - halfWidth);
        //newPos.x = collisionInfo.collisionPoint.x - halfWidth;
    }
    else if(collisionInfo.vHitNormal.x > 0)// ���� ��
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

    // ��� ���� �Ǵ�
    Collider* stairCollider = collisionInfo.groundActor->GetCollider();
    Vector2 stairStart = stairCollider->GetStartPoint();
    Vector2 stairEnd = stairCollider->GetEndPoint();
    Vector2 stairDir = stairEnd - stairStart;

    // ����� ��������� �»������� �Ǵ�
    bool isRightUpStair = (stairDir.x > 0 && stairDir.y < 0) || (stairDir.x < 0 && stairDir.y > 0);

    // �ùٸ� �𼭸� ����
    Vector2 targetCorner;
    Vector2 newPos = _movementComp->GetNewPos();
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
        _movementComp->SetOnStair(false);
        return;
    }

    // �÷��̾ ��ܿ� ���� (���õ� �𼭸��� ��ܿ� ��Ȯ�� �굵��)
    float deltaY = stairYAtCorner - targetCorner.y;
    _movementComp->AddNewPos({0, deltaY });
    //newPos.y += deltaY;

    if (_movementComp->GetAcceleration().x != 0.f) // ������ �̵� ���� ����
    {
        Vector2 stairDirection = stairDir.GetNormalize(); // ��� ���� ��������
        Vector2 velocity = _movementComp->GetVelocity();

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
        _movementComp->SetVelocity(velocity);
    }

    // ��� ���� ����
    _movementComp->SetOnStair(true);
    _movementComp->SetIsJumped(false);
    bIsMaxJump = false;

    Vector2 stairNormal = Vector2(-stairDir.y, stairDir.x).GetNormalize();
    if (stairNormal.y > 0) stairNormal *= -1; // ������ ���ϵ���
    vHitNormal = stairNormal;

}

void Player::OnCollisionBeginOverlap(const CollisionInfo& info)
{
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
    default:
        break;
    }
}

void Player::OnCollisionStayOverlap(const CollisionInfo& info)
{
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
        _movementComp->SetIsGround(false);
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

void Player::RenderHitbox(HDC hdc, Vector2 pos, float radian, float scale)
{
    POINT corners[4];
    GetRotatedCorners(corners, pos.x, pos.y, radian, 106 * scale, 32 * scale);

    // ������ ���� ó�� ���� �����ؾ� �ϹǷ� ����
    POINT polygon[5];
    memcpy(polygon, corners, sizeof(POINT) * 4);
    polygon[4] = corners[0];

    // ������ ������ OBB ���
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    HGDIOBJ oldPen = SelectObject(hdc, hPen);
    HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));

    Polyline(hdc, polygon, 5);

    // ���� ��/�귯�� ����
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(hPen);
}