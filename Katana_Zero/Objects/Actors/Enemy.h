#pragma once
#include "Actor.h"

class Camera;
class Player;

template<typename EnumType>
class StateMachine;

class Enemy : public Actor
{
	using Super = Actor;
public:
	function<void(Vector2, Vector2, float, float)> OnCreateBullet;
private:
	Camera* _worldCamera = nullptr;

	StateMachine<EEnemyState>* _stateMachine;

	Vector2 vNewPos;
	Vector2 vFrontDir = { 1, 0 };

	Actor* _targetPlayer = nullptr;

	float fDetectRange = 600.f;
	float fShotRnage = 500.f;
	bool bDetected = false;

public:
	virtual void Init(Vector2 pos) override;
	virtual void Update(float deltaTime) override;
	virtual void PostUpdate(float deltaTime) override;
	virtual void Render(HDC hdc) override;

	void SetCamera(Camera* camera) { _worldCamera = camera; }
	Vector2 GetFrontDir() { return vFrontDir; }
	void SetFrontDir(Vector2 dir) { vFrontDir = dir; }
	void TurnFrontDir() { vFrontDir.x = -vFrontDir.x; }
	bool GetDetected() { return bDetected; }

	virtual void OnCollisionBeginOverlap(const CollisionInfo& info) override;
	virtual void OnCollisionStayOverlap(const CollisionInfo& info) override;
	virtual void OnCollisionEndOverlap(const CollisionInfo& info) override;

	void ProcessGroundCollision(const CollisionInfo& collisionInfo);
	void ProcessStairCollision(const CollisionInfo& collisionInfo, Vector2 oldPos);

	void ChangeState(EEnemyState stateType);

	void SetPlayer(Actor* player) { _targetPlayer = player; }
	bool PlayerIsBack();
	bool DetectPlayer(float radius, float angleStart, float angleEnd);
	bool InShotRange();
	bool IsBlockedObstacles();

	void DrawSight(HDC hdc, int centerX, int centerY, int radius, float startAngle, float endAngle);

	void ShotBullet();

	virtual void TakeDamage(const Vector2& hitDir) override;
};