#pragma once
#include "Collider.h"
class MovingLineCollider : public Collider
{
	using Super = Collider;
public:
	static size_t StaticClass()
	{
		static char id = 0;
		return (size_t)&id;
	}

private:
	float fLength;
	float fRadian;

public:
	virtual void InitLine(float length, float radian) override;
	virtual void UpdateComponent(float deltaTime) override;
	virtual void RenderComponent(HDC hdc) override;

	float GetLength() { return fLength; }
	float GetRadian() { return fRadian; }
	virtual Vector2 GetStartPoint() override;
	virtual Vector2 GetEndPoint() override;
};