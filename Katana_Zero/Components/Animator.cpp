#include "pch.h"
#include "Animator.h"
#include "../Objects/Actors/Actor.h"
#include "../Resources/Sprite.h"
#include "../Game/Game.h"
#include "../Components/CameraComponent.h"

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
		_currentAnimation._sprite->IncreaseCurrentFrame();
		if (_currentAnimation._sprite->IsSpriteEnd())
		{
			if (!_currentAnimation._sprite->GetIsLoop())
			{
				bPlaying = false;
			}
			else
			{
				_currentAnimation._sprite->SetCurrentFrame(0);
			}
		}
	}
}

void Animator::RenderComponent(HDC hdc)
{
	CameraComponent* cameraComp = GetOwner()->GetComponent<CameraComponent>();
	Vector2 screenPos = cameraComp->ConvertScreenPos(GetPos());

	if (bIsFlipped)
	{
		screenPos.x -= _currentAnimation.vOffset.x;
	}
	else
	{
		screenPos.x += _currentAnimation.vOffset.x;
	}
	screenPos.y += _currentAnimation.vOffset.y;


	_currentAnimation._sprite->RenderStretchedSprite(hdc, screenPos, bIsFlipped);
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
	if(_currentAnimation._sprite != nullptr) _currentAnimation._sprite->SetCurrentFrame(0);
	_currentAnimation = _animationList[Index];
	bPlaying = true;
}

bool Animator::IsAnimationFinished()
{
	if (_currentAnimation._sprite != nullptr)
	{
		return _currentAnimation._sprite->IsSpriteEnd();
	}

	return false;
}