#pragma once
#include "Collider.h"

class LineCollider : public Collider
{
	using Super = Collider;
public:
	static size_t StaticClass()
	{
		static char id = 0;
		return (size_t)&id;
	}

private:
	Vector2 vStart;
	Vector2 vEnd;

public:
	virtual void InitLine(Vector2 start, Vector2 end) override;
	virtual void UpdateComponent(float deltaTime) override;
	virtual void RenderComponent(HDC hdc) override;

	virtual Vector2 GetStartPoint() override { return vStart; }
	virtual Vector2 GetEndPoint() override { return vEnd; }
};

