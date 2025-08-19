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
    // 중력의 방향 단위 벡터와 속도를 내적하여, 중력 방향의 값만 가진 벡터를 얻음.
    Vector2 normalGravity = vGravity.GetNormalize();
    float GravityLength = vVelocity.Dot(normalGravity);

    if (bOnGround)
    {
        vVelocity.y = 0.f;
        GravityLength = 0.f;
    }
    else
    {
        // 중력 가속도 적용
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
    //    // 중력 가속도 적용
    //    vAcceleration += vGravity;
    //}

    //// 최고점 근처에서 체공 효과를 위한 공기저항 시뮬레이션
    //if (vVelocity.y < 0) // 상승 중일 때
    //{
    //    // 속도가 느려질수록 공기저항도 감소 (자연스러운 감속)
    //    float airResistance = fAirResistance * (vVelocity.y * vVelocity.y) / (fJumpInitialVelocity * fJumpInitialVelocity);
    //    vAcceleration.y += airResistance;
    //}


    // 가속도는 속도(velocity)를 변화시킨다.
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

    // 최종적인 velocity
    // v : (3,4), a :(3,0), b : (0,4)
    // v = a + b
    vVelocity = gravityVector + sideVec;
}