#include "pch.h"
#include "Animator.h"
#include "../Objects/Actors/Actor.h"
#include "../Resources/Texture.h"
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
	if (fElapsedTime >= _currentAnimation->GetDuration())
	{
		fElapsedTime = 0.f;
		_currentAnimation->SetCurFrameCount(_currentAnimation->GetCurFrameCount() + 1);
		if (_currentAnimation->IsSpriteEnd())
		{
			if (!_currentAnimation->IsLoop())
			{
				bPlaying = false;
			}
			else
			{
				_currentAnimation->SetCurFrameCount(0);
			}
		}
	}
}

void Animator::RenderComponent(HDC hdc)
{
	CameraComponent* cameraComp = GetOwner()->GetComponent<CameraComponent>();
	Vector2 screenPos = cameraComp->ConvertScreenPos(GetPos());
	_currentAnimation->RenderStretchedSprite(hdc, screenPos, 2, bIsFlipped);
}

void Animator::AddAnimation(int32 Index, Texture* sprite)
{
	_animationList[Index] = sprite;
}

void Animator::SetAnimation(int32 Index)
{
	// 이전에 재생중인 애니메이션이 있을 경우 프레임 카운트 초기화
	if(_currentAnimation) _currentAnimation->SetCurFrameCount(0);
	_currentAnimation = _animationList[Index];
	bPlaying = true;
}

bool Animator::IsAnimationFinished()
{
	if (_currentAnimation)
	{
		return _currentAnimation->IsSpriteEnd();
	}

	return false;
}