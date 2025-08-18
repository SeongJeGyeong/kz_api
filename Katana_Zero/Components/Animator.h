#pragma once
#include "Component.h"

class Sprite;

struct AnimationInfo
{
	Sprite* _sprite = nullptr;
	Vector2 vOffset = Vector2(0, 0);
};

class Animator : public Component
{
public:
	static size_t StaticClass()
	{
		static char id = 0;
		return (size_t)&id;
	}

private:
	vector<AnimationInfo> _animationList;
	AnimationInfo _currentAnimation;
	float fElapsedTime = 0.f;

	bool bPlaying = false;
	bool bIsFlipped = false;

public:
	virtual void InitComponent(int32 endIndex);
	virtual void UpdateComponent(float deltaTime) override;
	virtual void RenderComponent(HDC hdc) override;

	void AddAnimation(int32 Index, Sprite* sprite, Vector2 offset = {0, 0});
	void SetAnimation(int32 Index);

	bool IsAnimationFinished();

	void SetFlipped(bool isFlip) { bIsFlipped = isFlip; }

	virtual uint32 GetPriority() override { return EComponentPriority::ANIMATION; }
};

