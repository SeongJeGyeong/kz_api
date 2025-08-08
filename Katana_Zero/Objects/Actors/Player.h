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
	//EPlayerState EState;

	Vector2 vFrontDir = {1, 0};

	Vector2 vGravity = Vector2(0, 1000.f);
	Vector2 vVelocity = {};
	Vector2 vAcceleration = {};

	float fMoveForce = 3000;
	float fMass = 1;

	float fJumpPressedTime = 0.f;
	float fPrevPressedTime = 0.f;

	bool bIsGround = false;
	bool bIsWall = false;
	bool bIsJumped = false;
	bool bJumpPressed = false;

	//////////////////
	float fJumpInitialVelocity = 300.f;
	float fMaxJumpHoldTime = 0.4f;
	float fJumpHoldForce = 1000.f;
	float fAirResistance = 50.f;
	//////////////////
	Vector2 vHitNormal;
	float halfHeight;

	ComponentContainer _components;
	StateMachine* _stateMachine;

public:
	virtual void Init(Vector2 pos) override;
	virtual void Update(float deltaTime) override;
	virtual void Render(HDC hdc) override;

	virtual void OnCollisionHit(Collider* other) override;

	void ApplyPhysics(float deltaTime);

	Vector2 GetAcceleration() { return vAcceleration; }

	bool GetIsGround() { return bIsGround; }
	void SetIsGround(bool isGround) { bIsGround = isGround; }
	bool GetIsJumped() { return bIsJumped; }
	void SetIsJumped(bool isJumped) { bIsJumped = isJumped; }
	bool GetIsWall() { return bIsWall; }
	void SetIsWall(bool isWall) { bIsWall = isWall; }

	bool IsFlipped() { return vFrontDir.x < 0; }

	void Jump();
	void IncreaseJump(float deltaTime);
	void ReleaseJump();
	void Move(bool dir);
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
};