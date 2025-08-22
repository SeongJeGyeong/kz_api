#include "pch.h"
#include "EnemyMovementComponent.h"
#include "../Objects/Actors/Actor.h"

void EnemyMovementComponent::InitComponent(Actor* owner)
{
	SetOwner(owner);
}

void EnemyMovementComponent::UpdateComponent(float deltaTime)
{
    ApplyPhysics(deltaTime);
    vNewPos = GetPos() + (vVelocity * deltaTime);
}

void EnemyMovementComponent::RenderComponent(HDC hdc)
{
}

void EnemyMovementComponent::ApplyPhysics(float deltaTime)
{
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
    if ((GetOwner()->GetWasHit() && !bOnGround)) friction = 0.98f;

    sideVec *= friction;
    if (sideVec.Length() < 1.0f)
        sideVec = Vector2(0.f, 0.f);

    // �������� velocity
    // v : (3,4), a :(3,0), b : (0,4)
    // v = a + b
    vVelocity = gravityVector + sideVec;
}