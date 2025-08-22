#include "pch.h"
#include "Animator.h"
#include "../Objects/Actors/Actor.h"
#include "../Resources/Sprite.h"
#include "../Game/Game.h"
#include "../Objects/Actors/Player.h"

void Animator::InitComponent(int32 endIndex)
{
	_animationList.resize(endIndex);
}

void Animator::UpdateComponent(float deltaTime)
{
	if (!bPlaying) return;

	fElapsedTime += deltaTime;
	if (fElapsedTime >= _currentAnimation._sprite->GetDuration())
	{
		fElapsedTime = 0.f;
		if (_currentAnimation._sprite->IsSpriteEnd(_currentAnimation.iCurrentFrame))
		{
			if (!_currentAnimation._sprite->GetIsLoop())
			{
				bPlaying = false;
			}
			else
			{
				_currentAnimation.iCurrentFrame = 0;
			}
		}
		else
		{
			_currentAnimation.iCurrentFrame += 1;
		}
	}
}

void Animator::RenderComponent(HDC hdc)
{
	Vector2 screenPos = Game::GetInstance()->ConvertCurSceneScreenPos(GetPos());

	if (bIsFlipped)
	{
		screenPos.x -= _currentAnimation.vOffset.x;
	}
	else
	{
		screenPos.x += _currentAnimation.vOffset.x;
	}
	screenPos.y += _currentAnimation.vOffset.y;

	_currentAnimation._sprite->RenderStretchedSprite(hdc, screenPos, _currentAnimation.iCurrentFrame, bIsFlipped);
}

void Animator::AddAnimation(int32 Index, Sprite* sprite, Vector2 offset)
{
	AnimationInfo info;
	info._sprite = sprite;
	info.vOffset = offset;
	_animationList[Index] = info;
}

void Animator::SetAnimation(int32 Index)
{
	// 이전에 재생중인 애니메이션이 있을 경우 프레임 카운트 초기화
	if (_currentAnimation._sprite != nullptr) _currentAnimation.iCurrentFrame = 0;
	_currentAnimation = _animationList[Index];
	bPlaying = true;
}

bool Animator::IsAnimationFinished()
{
	if (_currentAnimation._sprite != nullptr)
	{
		return (_currentAnimation._sprite->IsSpriteEnd(_currentAnimation.iCurrentFrame) && !bPlaying);
	}

	return false;
}