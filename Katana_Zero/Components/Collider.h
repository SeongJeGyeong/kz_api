#pragma once
#include "Component.h"

class Collider : public Component
{
public:
	Collider();
	virtual ~Collider(){}

	static size_t StaticClass()
	{
		static char id = 0;
		return (size_t)&id;
	}

private:
	const uint64 iColliderId;

	bool bCollideActive;
	Vector2 vHittedNormal;
	Vector2 vHitPos;
	Vector2 vHitMTV;

protected:
	ECollisionLayer eCollisionLayer;
	EColliderType eColliderType;
	bool bOverlapped = false;
	bool bBlocked = false;

private:
	uint64 GenerateID()
	{
		static uint64 id = 0;
		return id++;
	}

public:
	virtual void InitComponent() {};
	virtual void InitOBB(float width, float height, float rotation) {};
	virtual void InitAABB(float width, float height) {}
	virtual void InitLine(Vector2 start, Vector2 end) {}
	virtual void InitLine(float length, float radian) {}
	virtual void UpdateComponent(float deltaTime) override;
	virtual void RenderComponent(HDC hdc) override;

	ECollisionLayer GetCollisionLayer() { return eCollisionLayer; }
	void SetCollisionLayer(ECollisionLayer layer) { eCollisionLayer = layer; }
	EColliderType GetColliderType() { return eColliderType; }
	void SetColliderType(EColliderType type) { eColliderType = type; }

	virtual float GetWidth() { return 0; }
	virtual float GetHeight() { return 0; }
	virtual Vector2 GetStartPoint() { return Vector2(); }
	virtual Vector2 GetEndPoint() { return Vector2(); }
	virtual float GetLength() { return 0; }
	virtual float GetRadian() { return 0; }

	bool GetOverlapped() { return bOverlapped; }
	void SetOverlapped(bool overlap) { bOverlapped = overlap; }
	bool GetBlocked() { return bBlocked; }
	void SetBlocked(bool block) { bBlocked = block; }

	Vector2 GetHittedNormal() { return vHittedNormal; }
	void SetHittedNormal(Vector2 normal) { vHittedNormal = normal; }
	Vector2 GetHitPos() { return vHitPos; }
	void SetHitPos(Vector2 pos) { vHitPos = pos; }
	Vector2 GetHitMTV() { return vHitMTV; }
	void SetHitMTV(Vector2 mtv) { vHitMTV = mtv; }

	virtual void SetHitInfo(Vector2 pos, Vector2 normal) {}
	Vector2 newPos;

	uint64 GetColliderId() { return iColliderId; }

	virtual void ChangeSize(float width, float height) {}
};

