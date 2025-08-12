#include "pch.h"
#include "AABBCollider.h"
#include "../Game/Game.h"

void AABBCollider::InitAABB(float width, float height)
{
	fWidth = width;
	fHeight = height;
}

void AABBCollider::UpdateComponent(float deltaTime)
{
	Super::UpdateComponent(deltaTime);
}

void AABBCollider::RenderComponent(HDC hdc)
{
	if (!Game::GetInstance()->GetDebugMode()) return;
	Super::RenderComponent(hdc);

	Vector2 pos = GetPos();

	int32 left = pos.x - fWidth * 0.5f;
	int32 right = pos.x + fWidth * 0.5f;
	int32 top = pos.y - fHeight * 0.5f;
	int32 bottom = pos.y + fHeight * 0.5f;

	HPEN redPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
	HPEN oldPen = (HPEN)SelectObject(hdc, redPen);
	SelectObject(hdc, GetStockObject(NULL_BRUSH));
	Rectangle(hdc, left, top, right, bottom);
	(HPEN)SelectObject(hdc, oldPen);
	DeleteObject(redPen);
}

Vector2 AABBCollider::GetAABBMin()
{
	return { GetPos().x - fWidth / 2, GetPos().y - fHeight / 2 };
}

Vector2 AABBCollider::GetAABBMax()
{
	return { GetPos().x + fWidth / 2 , GetPos().y + fHeight / 2 };
}

RECT AABBCollider::GetRect()
{
	RECT rect;

	Vector2 pos = GetPos();

	rect.left = pos.x - fWidth * 0.5f;
	rect.right = pos.x + fWidth * 0.5f;
	rect.top = pos.y - fHeight * 0.5f;
	rect.bottom = pos.y + fHeight * 0.5f;

	return rect;
}

void AABBCollider::SetHitInfo(Vector2 pos, Vector2 normal)
{
	if (normal.x == 1)
	{
		float x = pos.x + (GetPos().x + fWidth * 0.5f);
		//if (newPos.x < x)
			newPos.x = x;
	}
	else if (normal.x == -1)
	{
		newPos.x = pos.x - (GetPos().x + fWidth * 0.5f);
	}
	else if (normal.y == 1)
	{
		newPos.y = pos.y + (GetPos().y + fHeight * 0.5f);
	}
	else if (normal.y == -1)
	{
		newPos.y = pos.y - (GetPos().y + fHeight * 0.5f);
	}
}
