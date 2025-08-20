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

struct AttackInfo
{
	vector<Collider*> _hitActors;
	Vector2 vAttackDir;
	float fAttackRadian = 0.f;
	bool bIsAttack = false;
};


class Player : public Actor
{
	using Super = Actor;

private:
	PlayerMovementComponent* _movementComp = nullptr;

	Vector2 vFrontDir = {1, 0};
	float fMoveForce = 10000;

	float fJumpPressedTime = 0.f;
	float fAttackDelayTime = 0.f;

	bool bIsMaxJump = false;
	bool bIsCrouch = false;

	float fMaxJumpHoldTime = 0.4f;
	float fJumpHoldForce = 1000.f;

	Vector2 vHitNormal;

	StateMachine<EPlayerState>* _stateMachine;

	AttackInfo _attackInfo;

public:
	virtual void Init(Vector2 pos) override;
	virtual void Update(float deltaTime) override;
	virtual void PostUpdate(float deltaTime) override;
	virtual void Render(HDC hdc) override;

	bool GetIsMaxJump() { return bIsMaxJump; }
	void SetIsMaxJump(bool maxJump) { bIsMaxJump = maxJump; }

	bool GetIsCrouch() { return bIsCrouch; }
	void SetIsCrouch(bool isCrouch) { bIsCrouch = isCrouch; }

	void SetIsAttack(bool isAttack) { _attackInfo.bIsAttack = isAttack; }
	void ClearHitActors() { _attackInfo._hitActors.clear(); }

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

	void SetPlayerCamera(Camera* camera);

	void printState(HDC hdc);

	void RenderHitbox(HDC hdc, Vector2 pos, float radian, float scale, COLORREF color);
};