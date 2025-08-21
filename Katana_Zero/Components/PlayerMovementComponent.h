#pragma once
#include "Component.h"

class Player;

class PlayerMovementComponent : public Component
{
	using Super = Component;
public:
	static size_t StaticClass()
	{
		static char id = 0;
		return (size_t)&id;
	}

private:
	Player* _player = nullptr;

	Vector2 vGravity = Vector2(0, 1000.f);
	Vector2 vVelocity = {};
	Vector2 vAcceleration = {};

	Vector2 vNewPos;

	//float fMoveForce = 10000;
	//float fMass = 1;

	float fJumpInitialVelocity = 300.f;
	//float fMaxJumpHoldTime = 0.4f;
	//float fJumpHoldForce = 1000.f;
	float fAirResistance = 50.f;

	bool bOnGround = false;
	bool bIsWall = false;
	bool bIsJumped = false;
	bool bIsPlatform = false;
	bool bIsAir = false;
	bool bOnStair = false;

public:
	virtual void InitComponent(Actor* owner);
	virtual void UpdateComponent(float deltaTime) override;
	virtual void RenderComponent(HDC hdc) override;

	void ApplyPhysics(float deltaTime);
	void UpdatePosition();

	void Jump();
	void AttackForce(Vector2 dir);

	Vector2 GetAcceleration() { return vAcceleration; }
	void SetAcceleration(Vector2 accel) { vAcceleration = accel; }
	void AddAcceleration(Vector2 accel) { vAcceleration += accel; }

	Vector2 GetVelocity() { return vVelocity; }
	void SetVelocity(Vector2 velocity) { vVelocity = velocity; }
	void AddVelocity(Vector2 velocity) { vVelocity += velocity; }
	void SetVelocityX(float xValue) { vVelocity.x = xValue; }
	void SetVelocityY(float yValue) { vVelocity.y = yValue; }

	Vector2 GetNewPos() { return vNewPos; }
	void SetNewPos(Vector2 pos) { vNewPos = pos; }
	void AddNewPos(Vector2 pos) { vNewPos += pos; }
	void SetNewPosX(float xValue) { vNewPos.x = xValue; }
	void SetNewPosY(float yValue) { vNewPos.y = yValue; }

	bool GetOnGround() { return bOnGround; }
	void SetOnGround(bool isGround) { bOnGround = isGround; }
	bool GetOnStair() { return bOnStair; }
	void SetOnStair(bool onStair) { bOnStair = onStair; }
	bool GetIsJumped() { return bIsJumped; }
	void SetIsJumped(bool isJumped) { bIsJumped = isJumped; }
	bool GetIsWall() { return bIsWall; }
	void SetIsWall(bool isWall) { bIsWall = isWall; }
	bool GetIsPlatform() { return bIsPlatform; }
	void SetIsPlatform(bool isPlatform) { bIsPlatform = isPlatform; }

	virtual uint32  GetPriority() override { return EComponentPriority::MOVEMENT; }
};

