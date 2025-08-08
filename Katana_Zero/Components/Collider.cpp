#include "pch.h"
#include "Collider.h"

void Collider::UpdateComponent(float deltaTime)
{

}

void Collider::RenderComponent(HDC hdc)
{
}

Vector2 Collider::GetCenterOffset()
{
	return Vector2(GetPos().x + vCenter.x, GetPos().y + vCenter.y);
}

