#include "pch.h"
#include "BossMovementComponent.h"
#include "../Objects/Actors/Actor.h"

void BossMovementComponent::InitComponent(Actor* owner)
{
    SetOwner(owner);
}

void BossMovementComponent::UpdateComponent(float deltaTime)
{
    ApplyPhysics(deltaTime);
    vNewPos = GetPos() + (vVelocity * deltaTime);
}

void BossMovementComponent::RenderComponent(HDC hdc)
{
}

void BossMovementComponent::ApplyPhysics(float deltaTime)
{
    if (bIsLunge)
    {
        vVelocity.y += vGravity.y * deltaTime;
        return;
    }

    // �߷��� ���� ���� ���Ϳ� �ӵ��� �����Ͽ�, �߷� ������ ���� ���� ���͸� ����.
    Vector2 normalGravity = vGravity.GetNormalize();
    float GravityLength = vVelocity.Dot(normalGravity);

    if (GetOwner()->GetWasHit())
    {
        vAcceleration += vGravity;
    }
    else if (bOnGround)
    {
        vVelocity.y = 0.f;
        GravityLength = 0.f;
    }
    else
    {
        // �߷� ���ӵ� ����
        vAcceleration += vGravity;
    }

    // ���ӵ��� �ӵ�(velocity)�� ��ȭ��Ų��.
    vVelocity += vAcceleration * deltaTime;

    float upFactor = 1000.f;
    float sideFactor = 1000.f;

    Vector2 gravityVector = normalGravity * GravityLength;
    Vector2 sideVec = vVelocity - gravityVector;
    float sideLength = sideVec.Length();

    if (GravityLength > upFactor) gravityVector = normalGravity * upFactor;

    if (sideLength > sideFactor) sideVec = sideVec.GetNormalize() * sideFactor;
    else if (sideLength < -sideFactor) sideVec = sideVec.GetNormalize() * sideFactor;

    float friction = 0.8f;
    if ((GetOwner()->GetWasHit() && !bOnGround) || GetOwner()->GetCurrentState() == EBossState::BOSS_LUNGE) friction = 0.98f;

    sideVec *= friction;
    if (sideVec.Length() < 1.0f)
        sideVec = Vector2(0.f, 0.f);

    // �������� velocity
    // v : (3,4), a :(3,0), b : (0,4)
    // v = a + b
    vVelocity = gravityVector + sideVec;
}

void BossMovementComponent::ParabolicJump(const Vector2& targetPos)
{
    bIsLunge = true;
    bOnGround = false;

    // �ʱ� �ӵ� ���
    vVelocity = CalculateParabolicVelocity(GetPos(), targetPos);
    vAcceleration = Vector2(0.f, 0.f);
}

Vector2 BossMovementComponent::CalculateParabolicVelocity(const Vector2& startPos, const Vector2& targetPos)
{
    Vector2 initialVelocity;

    float dx = targetPos.x - startPos.x;
    float dy = targetPos.y - startPos.y;
    float direction = (dx >= 0.f) ? 1.f : -1.f;
    dx = fabsf(dx);

    const float g = vGravity.y;

    // ��ǥ���� ������ ���� �ӵ��� �������� �ð� ���
    float time = dx / 1000.f;

    // Ÿ�� ���� Ŭ����
    if (time < 0.3f) time = 0.3f;
    if (time > 0.5f) time = 0.5f;

    // ���� �ӵ�
    initialVelocity.x = direction * (dx / time);

    // ���� �ӵ�
    initialVelocity.y = (dy - 0.5f * g * time * time) / time;
    return initialVelocity;
}