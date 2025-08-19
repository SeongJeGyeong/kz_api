#pragma once
#include "Actor.h"

template<typename EnumType>
class StateMachine;

class Animator;
class EffectorComponent;
class InputComponent;
class Camera;
class CameraComponent;
class PlayerMovementComponent;

class Player : public Actor
{
	using Super = Actor;

private:
	PlayerMovementComponent* _movementComp = nullptr;

	Vector2 vFrontDir = {1, 0};

	//Vector2 vGravity = Vector2(0, 1000.f);
	//Vector2 vVelocity = {};
	//Vector2 vAcceleration = {};

	//Vector2 vNewPos;

	float fMoveForce = 10000;
	//float fMass = 1;

	float fJumpPressedTime = 0.f;
	float fAttackDelayTime = 0.f;

	//bool bIsGround = false;
	//bool bIsWall = false;
	//bool bIsJumped = false;
	bool bIsMaxJump = false;
	//bool bIsPlatform = false;
	//bool bIsAir = false;
	//bool bOnStair = false;

	bool bIsCrouch = false;

	//////////////////
	//float fJumpInitialVelocity = 300.f;
	float fMaxJumpHoldTime = 0.4f;
	float fJumpHoldForce = 1000.f;
	//float fAirResistance = 50.f;
	//////////////////

	bool bIsAttack = false;
	float fAttackRadian = 0.f;

	Vector2 vHitNormal;

	StateMachine<EPlayerState>* _stateMachine;

public:
	virtual void Init(Vector2 pos) override;
	virtual void Update(float deltaTime) override;
	virtual void PostUpdate(float deltaTime) override;
	virtual void Render(HDC hdc) override;

	//void ApplyPhysics(float deltaTime);

	//Vector2 GetAcceleration() { return vAcceleration; }

	//bool GetIsGround() { return bIsGround; }
	//void SetIsGround(bool isGround) { bIsGround = isGround; }
	//bool GetOnStair() { return bOnStair; }
	//void SetOnStair(bool onStair) { bOnStair = onStair; }
	//bool GetIsJumped() { return bIsJumped; }
	//void SetIsJumped(bool isJumped) { bIsJumped = isJumped; }
	bool GetIsMaxJump() { return bIsMaxJump; }
	void SetIsMaxJump(bool maxJump) { bIsMaxJump = maxJump; }
	//bool GetIsWall() { return bIsWall; }
	//void SetIsWall(bool isWall) { bIsWall = isWall; }
	//bool GetIsPlatform() { return bIsPlatform; }
	//void SetIsPlatform(bool isPlatform) { bIsPlatform = isPlatform; }

	bool GetIsCrouch() { return bIsCrouch; }
	void SetIsCrouch(bool isCrouch) { bIsCrouch = isCrouch; }

	void SetIsAttack(bool isAttack) { bIsAttack = isAttack; }

	//Vector2 GetNewPos() const { return vNewPos; }

	float GetAttackDelayTime() { return fAttackDelayTime; }

	bool IsFlipped() { return vFrontDir.x < 0; }

	void Jump();
	void IncreaseJump(float deltaTime);
	void ReleaseJump();
	void Move(bool dir);
	void Crouch(bool active);
	void Roll(bool dir);
	void Attack();

	virtual int32 GetCurrentState() override;

	void ChangeState(EPlayerState stateType);

	void ProcessGroundCollision(const CollisionInfo& collisionInfo);
	void ProcessGroundFloor(const CollisionInfo& collisionInfo);
	void ProcessGroundCeiling(const CollisionInfo& collisionInfo);
	void ProcessGroundWall(const CollisionInfo& collisionInfo);

	void ProcessPlatformCollision(const CollisionInfo& collisionInfo);
	void ProcessWallCollision(const CollisionInfo& collisionInfo);
	void ProcessCeilingCollision(const CollisionInfo& collisionInfo);
	void ProcessStairCollision(const CollisionInfo& collisionInfo, Vector2 oldPos);

	virtual void OnCollisionBeginOverlap(const CollisionInfo& info) override;
	virtual void OnCollisionStayOverlap(const CollisionInfo& info) override;
	virtual void OnCollisionEndOverlap(const CollisionInfo& info) override;

	//Vector2 GetVelocity() const { return vVelocity; }

	void SetPlayerCamera(Camera* camera);

	void printState(HDC hdc);

	void RenderHitbox(HDC hdc, Vector2 pos, float radian, float scale);
};