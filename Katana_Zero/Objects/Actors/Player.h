#pragma once
#include "Actor.h"
#include "../../Utils/ComponentContainer.h"

class Animator;
class EffectorComponent;
class InputComponent;
class StateMachine;

class Player : public Actor
{
	using Super = Actor;
private:
	Vector2 vFrontDir = {1, 0};

	Vector2 vGravity = Vector2(0, 1000.f);
	Vector2 vVelocity = {};
	Vector2 vAcceleration = {};

	Vector2 vNewPos;

	float fMoveForce = 10000;
	float fMass = 1;

	float fJumpPressedTime = 0.f;
	float fAttackDelayTime = 0.f;

	bool bIsGround = false;
	bool bIsWall = false;
	bool bIsJumped = false;
	bool bIsMaxJump = false;
	bool bIsPlatform = false;
	bool bIsAir = false;
	
	bool bOnStair = false;
	bool bIsCrouch = false;

	//////////////////
	float fJumpInitialVelocity = 300.f;
	float fMaxJumpHoldTime = 0.4f;
	float fJumpHoldForce = 1000.f;
	float fAirResistance = 50.f;
	//////////////////

	Vector2 vHitNormal;

	ComponentContainer _components;
	StateMachine* _stateMachine;

public:
	virtual void Init(Vector2 pos) override;
	virtual void Update(float deltaTime) override;
	virtual void PostUpdate(float deltaTime) override;
	virtual void Render(HDC hdc) override;

	virtual void OnCollisionHit(Collider* other) override;

	void ApplyPhysics(float deltaTime);
	void UpdatePosition(float deltaTime);

	Vector2 GetAcceleration() { return vAcceleration; }

	bool GetIsGround() { return bIsGround; }
	void SetIsGround(bool isGround) { bIsGround = isGround; }
	bool GetOnStair() { return bOnStair; }
	void SetOnStair(bool onStair) { bOnStair = onStair; }
	bool GetIsJumped() { return bIsJumped; }
	void SetIsJumped(bool isJumped) { bIsJumped = isJumped; }
	bool GetIsMaxJump() { return bIsMaxJump; }
	void SetIsMaxJump(bool maxJump) { bIsMaxJump = maxJump; }
	bool GetIsWall() { return bIsWall; }
	void SetIsWall(bool isWall) { bIsWall = isWall; }
	bool GetIsPlatform() { return bIsPlatform; }
	void SetIsPlatform(bool isPlatform) { bIsPlatform = isPlatform; }

	bool GetIsCrouch() { return bIsCrouch; }
	//void SetIsCrouch(bool isCrouch) { bIsCrouch = isCrouch; }

	Vector2 GetNewPos() const { return vNewPos; }

	float GetAttackDelayTime() { return fAttackDelayTime; }

	bool IsFlipped() { return vFrontDir.x < 0; }

	void Jump();
	void IncreaseJump(float deltaTime);
	void ReleaseJump();
	void Move(bool dir);
	void Crouch(bool active);
	void Roll(bool dir);
	void Landing();
	void Attack();

	template <typename T>
	T* GetComponent()
	{
		return _components.GetComponent<T>();
	}

	void UpdateAttachedComponents(float deltaTime)
	{
		_components.UpdateComponents(deltaTime);
	}

	virtual int32 GetCurrentState() override;

	void ChangeState(EPlayerState stateType);

	void ProcessGroundCollision(const CollisionInfo& collisionInfo, Vector2& newPos);
	void ProcessGroundFloor(const CollisionInfo& collisionInfo, Vector2& newPos);
	void ProcessGroundCeiling(const CollisionInfo& collisionInfo, Vector2& newPos);
	void ProcessGroundWall(const CollisionInfo& collisionInfo, Vector2& newPos);

	void ProcessPlatformCollision(const CollisionInfo& collisionInfo, Vector2& newPos);
	void ProcessWallCollision(const CollisionInfo& collisionInfo, Vector2& newPos);
	void ProcessStairCollision(const CollisionInfo& collisionInfo, Vector2 oldPos, Vector2& newPos);

	virtual void OnCollisionBeginOverlap(const CollisionInfo& info) override;
	virtual void OnCollisionStayOverlap(const CollisionInfo& info) override;
	virtual void OnCollisionEndOverlap(const CollisionInfo& info) override;

	Vector2 GetVelocity() const { return vVelocity; }
};