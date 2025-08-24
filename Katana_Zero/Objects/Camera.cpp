#include "pch.h"
#include "Camera.h"
#include "../Managers/TimeManager.h"

void Camera::Init()
{
	Super::Init();

	fCameraWidth = 1280.f;
	fCameraHeight = 720.f;
}

void Camera::Update(float deltaTime)
{
	if (bCameraShake)
	{
		//OutputDebugString(L"shake\n");
		TimeManager* timeManager = TimeManager::GetInstance();
		fShakeDuration += timeManager->GetConstDeltaTime();
		if (fShakeDuration >= 0.03f)
		{
			fShakeDuration = 0;

			vPos.y += vShakePos.y * 2.f;
			vShakePos = { -vShakePos.x, -vShakePos.y };
		}
		fShakeTime += timeManager->GetConstDeltaTime();
		if (fShakeTime >= timeManager->GetHitStopTime()) bCameraShake = false;
	}
	else
	{
		float halfSizeX = fCameraWidth / 2;
		float halfSizeY = fCameraHeight / 2;
		vPos.x = clamp(vPos.x, halfSizeX, iWorldSizeX - halfSizeX);
		vPos.y = clamp(vPos.y, halfSizeY, iWorldSizeY - halfSizeY);
	}
}

void Camera::Render(HDC hdc)
{
	wstring strs = std::format(L"cameraPos({0}, {1})", vPos.x, vPos.y);
	::TextOut(hdc, 450, 100, strs.c_str(), static_cast<int32>(strs.size()));
}

Vector2 Camera::ConvertScreenPos(Vector2 worldPos)
{
	Vector2 convertPos;
	convertPos.x = worldPos.x - (vPos.x - (fCameraWidth / 2));
	convertPos.y = worldPos.y - (vPos.y - (fCameraHeight / 2));
	return convertPos;
}

Vector2 Camera::ConvertWorldPos(Vector2 screenPos)
{
	Vector2 convertPos;
	convertPos.x = screenPos.x + (vPos.x - (fCameraWidth / 2));
	convertPos.y = screenPos.y + (vPos.y - (fCameraHeight / 2));
	return convertPos;
}

POINT Camera::ConvertScreenPos(POINT worldPos)
{
	POINT convertPos;
	convertPos.x = worldPos.x - (vPos.x - (fCameraWidth / 2));
	convertPos.y = worldPos.y - (vPos.y - (fCameraHeight / 2));
	return convertPos;

}

POINT Camera::ConvertWorldPos(POINT screenPos)
{
	POINT convertPos;
	convertPos.x = screenPos.x + (vPos.x - (fCameraWidth / 2));
	convertPos.y = screenPos.y + (vPos.y - (fCameraHeight / 2));
	return convertPos;
}
