#include "pch.h"
#include "EffectorComponent.h"
#include "../Managers/ResourceManager.h"
#include "../Resources/Texture.h"
#include "../Objects/Actors/Actor.h"
#include "../Components/CameraComponent.h"

void EffectorComponent::InitComponent()
{
	SetSprite();
}

void EffectorComponent::UpdateComponent(float deltaTime)
{
	for (auto it = _spawnList.begin(); it != _spawnList.end();)
	{
		(*it).fElapsedTime += deltaTime;

		if ((*it).fElapsedTime >= _effectSprite->GetDuration())
		{
			(*it).fElapsedTime = 0.f;
			if ((*it).iCurFrame >= _effectSprite->GetMaxFrameCount())
			{
				if (_effectSprite->IsLoop())
				{
					(*it).iCurFrame = 0;
				}
				else
				{
					it = _spawnList.erase(it);
					continue;
				}
			}
			else
			{
				(*it).iCurFrame += 1;
			}
		}

		++it;
	}
}

void EffectorComponent::RenderComponent(HDC hdc)
{
	CameraComponent* cameraComp = GetOwner()->GetComponent<CameraComponent>();
	for (const SpawnInfo& info : _spawnList)
	{
		Vector2 screenPos = cameraComp->ConvertScreenPos(info.bAttached ? GetPos() : info.vPos);
		_effectSprite->RenderRotatedSprite(hdc, screenPos, info.fRadAngle, info.fScale, info.iCurFrame, info.bIsFlipped);
	}
}

void EffectorComponent::Destroy()
{

}

void EffectorComponent::SetSprite()
{
	_effectSprite = ResourceManager::GetInstance()->GetTexture("zero_slash");
}

void EffectorComponent::PlayEffect(bool flipped, float radian, float scale, bool attached, Vector2 pos)
{
	SpawnInfo info;
	info.bIsFlipped = flipped;
	info.iCurFrame = 0;
	info.fRadAngle = radian;
	info.fScale = scale;
	info.fElapsedTime = 0.f;
	info.bAttached = attached;
	info.vPos = pos;

	_spawnList.push_back(info);
}