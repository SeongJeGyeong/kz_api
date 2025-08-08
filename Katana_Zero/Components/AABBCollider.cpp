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

	int32 left = GetCenterOffset().x - fWidth * 0.5f;
	int32 right = GetCenterOffset().x + fWidth * 0.5f;
	int32 top = GetCenterOffset().y - fHeight * 0.5f;
	int32 bottom = GetCenterOffset().y + fHeight * 0.5f;

	HPEN redPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
	HPEN oldPen = (HPEN)SelectObject(hdc, redPen);
	SelectObject(hdc, GetStockObject(NULL_BRUSH));
	Rectangle(hdc, left, top, right, bottom);
	(HPEN)SelectObject(hdc, oldPen);
	DeleteObject(redPen);
}

Vector2 AABBCollider::GetAABBMin()
{
	return { GetCenterOffset().x - fWidth / 2, GetCenterOffset().y - fHeight / 2 };
}

Vector2 AABBCollider::GetAABBMax()
{
	return { GetCenterOffset().x + fWidth / 2 , GetCenterOffset().y + fHeight / 2 };
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
