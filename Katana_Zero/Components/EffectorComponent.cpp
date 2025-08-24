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
	for (const SpawnInfo& constInfo : _constSpawnList)
	{
		Vector2 Pos = constInfo.bAttached ? GetPos() + constInfo.vPos : constInfo.vPos;
		Vector2 screenPos = Game::GetInstance()->ConvertCurSceneScreenPos(Pos);
		ResourceManager::GetInstance()->GetSprite(constInfo.effectName)->RenderRotatedSprite(hdc, screenPos, constInfo.fRadAngle, constInfo.fScale, constInfo.iCurrentFrame, constInfo.bIsFlipped);
	}

	for (const SpawnInfo& info : _spawnList)
	{
		Vector2 Pos = info.bAttached ? GetPos() + info.vPos : info.vPos;
		Vector2 screenPos = Game::GetInstance()->ConvertCurSceneScreenPos(Pos);
		ResourceManager::GetInstance()->GetSprite(info.effectName)->RenderRotatedSprite(hdc, screenPos, info.fRadAngle, info.fScale, info.iCurrentFrame, info.bIsFlipped);
	}
}

void EffectorComponent::UpdateConstantEffect(float deltaTime)
{
	for (auto it = _constSpawnList.begin(); it != _constSpawnList.end();)
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
					it = _constSpawnList.erase(it);
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

void EffectorComponent::PlayEffectForDuration(string name, bool flipped, float radian, float scale, bool attached, float duration, Vector2 pos)
{
}

void EffectorComponent::PlayConstantEffect(string name, bool flipped, float radian, float scale, bool attached, Vector2 pos)
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

	_constSpawnList.push_back(info);
}
