#include "pch.h"
#include "MovingLineCollider.h"
#include "../Game/Game.h"

void MovingLineCollider::InitLine(float length, float radian)
{
	fLength = length;
	fRadian = radian;
}

void MovingLineCollider::UpdateComponent(float deltaTime)
{

}

void MovingLineCollider::RenderComponent(HDC hdc)
{
	if (!Game::GetInstance()->GetDebugMode()) return;
	Super::RenderComponent(hdc);

	float halfLength = fLength * 0.5f;
	float dx = cos(fRadian) * halfLength;
	float dy = sin(fRadian) * halfLength;
	Vector2 start = { GetPos().x - dx, GetPos().y - dy };
	Vector2 end = { GetPos().x + dx, GetPos().y + dy };
	Vector2 screenStart = Game::GetInstance()->ConvertCurSceneScreenPos(start);
	Vector2 screenEnd = Game::GetInstance()->ConvertCurSceneScreenPos(end);

	HPEN pen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
	HPEN oldPen = (HPEN)SelectObject(hdc, pen);

	MoveToEx(hdc, screenStart.x, screenStart.y, nullptr);
	LineTo(hdc, screenEnd.x, screenEnd.y);

	SelectObject(hdc, oldPen);
	DeleteObject(pen);
}

Vector2 MovingLineCollider::GetStartPoint()
{
	Vector2 pos = GetPos();
	float halfLength = fLength * 0.5f;
	float dx = cos(fRadian) * halfLength;
	float dy = sin(fRadian) * halfLength;
	
	return { pos.x - dx, pos.y - dy };
}

Vector2 MovingLineCollider::GetEndPoint()
{
	Vector2 pos = GetPos();
	float halfLength = fLength * 0.5f;
	float dx = cos(fRadian) * halfLength;
	float dy = sin(fRadian) * halfLength;

	return { pos.x + dx, pos.y + dy };
}