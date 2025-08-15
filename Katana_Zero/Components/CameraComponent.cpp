#include "pch.h"
#include "CameraComponent.h"
#include "../Game/Game.h"
#include "../Objects/Camera.h"

void CameraComponent::InitComponent(Actor* owner)
{
	SetOwner(owner);
}

void CameraComponent::UpdateComponent(float deltaTime)
{
	_camera->SetPos(GetPos());
	//_camera->Update(deltaTime);
}

void CameraComponent::RenderComponent(HDC hdc)
{
}

Vector2 CameraComponent::ConvertScreenPos(Vector2 worldPos)
{
	return _camera->ConvertScreenPos(worldPos);
}

Vector2 CameraComponent::ConvertWorldPos(Vector2 screenPos)
{
	return _camera->ConvertWorldPos(screenPos);
}

POINT CameraComponent::ConvertScreenPos(POINT worldPos)
{
	return _camera->ConvertScreenPos(worldPos);
}

POINT CameraComponent::ConvertWorldPos(POINT screenPos)
{
	return _camera->ConvertWorldPos(screenPos);
}
