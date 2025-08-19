#pragma once
#include "Actor.h"

class Camera;

template<typename EnumType>
class StateMachine;

class Enemy : public Actor
{
	using Super = Actor;
private:
	Camera* _worldCamera = nullptr;

	StateMachine<EEnemyState>* _stateMachine;

	Vector2 vNewPos;
	Vector2 vFrontDir = { 1, 0 };

public:
	virtual void Init(Vector2 pos) override;
	virtual void Update(float deltaTime) override;
	virtual void PostUpdate(float deltaTime) override;
	virtual void Render(HDC hdc) override;

	void SetCamera(Camera* camera) { _worldCamera = camera; }
	Vector2 GetFrontDir() { return vFrontDir; }
	void SetFrontDir(Vector2 dir) { vFrontDir = dir; }
	void TurnFrontDir() { vFrontDir.x = -vFrontDir.x; }

	virtual void OnCollisionBeginOverlap(const CollisionInfo& info) override;
	virtual void OnCollisionStayOverlap(const CollisionInfo& info) override;
	virtual void OnCollisionEndOverlap(const CollisionInfo& info) override;

	void ProcessGroundCollision(const CollisionInfo& collisionInfo);

	void ChangeState(EEnemyState stateType);
};