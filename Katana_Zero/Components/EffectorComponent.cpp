#include "pch.h"
#include "EffectorComponent.h"
#include "../Managers/ResourceManager.h"
#include "../Resources/Sprite.h"
#include "../Objects/Actors/Actor.h"
#include "../Game/Game.h"

void EffectorComponent::InitComponent()
{

}

void EffectorComponent::UpdateComponent(float deltaTime)
{
	for (auto it = _spawnList.begin(); it != _spawnList.end();)
	{
		(*it).fElapsedTime += deltaTime;

		if ((*it).fElapsedTime >= (*it).fDuration)
		{
			(*it).fElapsedTime = 0.f;
			if ((*it).iCurrentFrame >= (*it).iFrameCount)
			{
				if ((*it).bLoop)
				{
					(*it).iCurrentFrame = 0;
				}
				else
				{
					it = _spawnList.erase(it);
					continue;
				}
			}
			else
			{
				(*it).iCurrentFrame += 1;
			}
		}

		++it;
	}
}

void EffectorComponent::RenderComponent(HDC hdc)
{
	for (const SpawnInfo& info : _spawnList)
	{
		Vector2 screenPos = Game::GetInstance()->ConvertCurSceneScreenPos(info.bAttached ? GetPos() : info.vPos);
		ResourceManager::GetInstance()->GetSprite(info.effectName)->RenderRotatedSprite(hdc, screenPos, info.fRadAngle, info.fScale, info.iCurrentFrame, info.bIsFlipped);
	}
}

void EffectorComponent::Destroy()
{

}

void EffectorComponent::PlayEffect(string name, bool flipped, float radian, float scale, bool attached, Vector2 pos)
{
	Sprite* sprite = ResourceManager::GetInstance()->GetSprite(name);
	if (sprite == nullptr) return;

	SpawnInfo info;
	info.effectName = name;
	info.iFrameCount = sprite->GetFrameCount();
	info.fDuration = sprite->GetDuration();
	info.bLoop = sprite->GetIsLoop();
	info.iCurrentFrame = 0;
	info.bIsFlipped = flipped;
	info.fRadAngle = radian;
	info.fScale = scale;
	info.fElapsedTime = 0.f;
	info.bAttached = attached;
	info.vPos = pos;

	_spawnList.push_back(info);
}