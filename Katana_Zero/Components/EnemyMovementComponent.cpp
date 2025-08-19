#include "pch.h"
#include "EnemyMovementComponent.h"

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

    if (bOnGround)
    {
        vVelocity.y = 0.f;
        GravityLength = 0.f;
    }
    else
    {
        // �߷� ���ӵ� ����
        vAcceleration += vGravity;
    }

    //if (bIsGround || bIsPlatform)
    //{
    //    if (vVelocity.y >= 0.f) bIsJumped = false;
    //    vVelocity.y = 0.f;
    //    GravityLength = 0.f;
    //}
    //else if (bOnStair)
    //{
    //    bIsJumped = false;
    //    vVelocity -= normalGravity * GravityLength;
    //}
    //else
    //{
    //    // �߷� ���ӵ� ����
    //    vAcceleration += vGravity;
    //}

    //// �ְ��� ��ó���� ü�� ȿ���� ���� �������� �ùķ��̼�
    //if (vVelocity.y < 0) // ��� ���� ��
    //{
    //    // �ӵ��� ���������� �������׵� ���� (�ڿ������� ����)
    //    float airResistance = fAirResistance * (vVelocity.y * vVelocity.y) / (fJumpInitialVelocity * fJumpInitialVelocity);
    //    vAcceleration.y += airResistance;
    //}


    // ���ӵ��� �ӵ�(velocity)�� ��ȭ��Ų��.
    vVelocity += vAcceleration * deltaTime;

    float upFactor = 400.f;
    float sideFactor = 400.f;

    Vector2 gravityVector = normalGravity * GravityLength;
    Vector2 sideVec = vVelocity - gravityVector;
    float sideLength = sideVec.Length();

    if (GravityLength > upFactor) gravityVector = normalGravity * upFactor;

    if (sideLength > sideFactor) sideVec = sideVec.GetNormalize() * sideFactor;
    else if (sideLength < -sideFactor) sideVec = sideVec.GetNormalize() * sideFactor;

    float friction = 0.8f;

    sideVec *= friction;
    if (sideVec.Length() < 1.0f)
        sideVec = Vector2(0.f, 0.f);

    // �������� velocity
    // v : (3,4), a :(3,0), b : (0,4)
    // v = a + b
    vVelocity = gravityVector + sideVec;
}