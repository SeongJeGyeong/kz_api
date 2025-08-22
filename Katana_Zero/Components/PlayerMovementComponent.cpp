#include "pch.h"
#include "PlayerMovementComponent.h"
#include "../Objects/Actors/Player.h"
#include "../Managers/InputManager.h"

void PlayerMovementComponent::InitComponent(Actor* owner)
{
	SetOwner(owner);
    _player = static_cast<Player*>(owner);
}

void PlayerMovementComponent::UpdateComponent(float deltaTime)
{
}

void PlayerMovementComponent::RenderComponent(HDC hdc)
{
    SetTextColor(hdc, RGB(255, 255, 255));
    wstring str2 = std::format(L"newPos({0}, {1})", vNewPos.x, vNewPos.y);
    ::TextOut(hdc, 100, 90, str2.c_str(), static_cast<int32>(str2.size()));

    wstring str = std::format(L"IsGround({0}) IsJumped({1}) bIsStair({2}) bIsPlatform({3}) bIsWall({4})", bOnGround, bIsJumped, bOnStair, bIsPlatform, bIsWall);
    ::TextOut(hdc, 450, 30, str.c_str(), static_cast<int32>(str.size()));

    wstring str5 = std::format(L"velocity ( {0}, {1} )", vVelocity.x, vVelocity.y);
    ::TextOut(hdc, 100, 120, str5.c_str(), static_cast<int32>(str5.size()));
}

void PlayerMovementComponent::ApplyPhysics(float deltaTime)
{
    // �߷��� ���� ���� ���Ϳ� �ӵ��� �����Ͽ�, �߷� ������ ���� ���� ���͸� ����.
    Vector2 normalGravity = vGravity.GetNormalize();
    float GravityLength = vVelocity.Dot(normalGravity);

    if (bOnGround || bIsPlatform)
    {
        if (vVelocity.y >= 0.f) bIsJumped = false;
        vVelocity.y = 0.f;
        GravityLength = 0.f;
    }
    else if (bOnStair)
    {
        bIsJumped = false;
        vVelocity -= normalGravity * GravityLength;
    }
    else
    {
        // �߷� ���ӵ� ����
        vAcceleration += vGravity;
    }

    if ((_player->GetCurrentState() == EPlayerState::PLAYER_FALL ||
        _player->GetCurrentState() == EPlayerState::PLAYER_JUMP) &&
        InputManager::GetInstance()->GetButtonPressed(KeyType::S))
    {
        vAcceleration.y += 10000.f;
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
    float sideFactor = 400.f;
    if (_player->GetCurrentState() == EPlayerState::PLAYER_ROLL)
    {
        sideFactor = 1500.f;
    }
    else if (_player->GetCurrentState() == EPlayerState::PLAYER_HURT_BEGIN ||
             _player->GetCurrentState() == EPlayerState::PLAYER_HURT_LOOP)
    {
        sideFactor = 800.f;
    }

    Vector2 gravityVector = normalGravity * GravityLength;
    Vector2 sideVec = vVelocity - gravityVector;
    float sideLength = sideVec.Length();

    if (GravityLength > upFactor) gravityVector = normalGravity * upFactor;

    if (sideLength > sideFactor) sideVec = sideVec.GetNormalize() * sideFactor;
    else if (sideLength < -sideFactor) sideVec = sideVec.GetNormalize() * sideFactor;

    float friction = 0.8f;
    if (_player->GetCurrentState() == EPlayerState::PLAYER_ROLL)
    {
        friction = 0.95f;
    }
    if (_player->GetCurrentState() == EPlayerState::PLAYER_ATTACK || 
        //_player->GetCurrentState() == EPlayerState::PLAYER_ROLL ||
        _player->GetCurrentState() == EPlayerState::PLAYER_HURT_BEGIN ||
        _player->GetCurrentState() == EPlayerState::PLAYER_HURT_LOOP)
    {
        friction = 0.98f;
        gravityVector *= friction;
    }

    sideVec *= friction;
    if (sideVec.Length() < 1.0f)
        sideVec = Vector2(0.f, 0.f);

    // �������� velocity
    // v : (3,4), a :(3,0), b : (0,4)
    // v = a + b
    vVelocity = gravityVector + sideVec;
}

void PlayerMovementComponent::UpdatePosition()
{
    _player->SetPos(vNewPos);
    vAcceleration = { 0, 0 };
}

void PlayerMovementComponent::Jump()
{
    vVelocity.y = -fJumpInitialVelocity; // ������ ���� ����
    bOnGround = false;
    bIsPlatform = false;
    bOnStair = false;
    bIsJumped = true;
}

void PlayerMovementComponent::AttackForce(Vector2 dir)
{
    vVelocity = { 0, 0 };
    vAcceleration.x += dir.x * 100000000.f;
    vAcceleration.y += dir.y * 100000000.f;
}
