#pragma once
#include "Component.h"

class Texture;
//template<typename StateEnum>
class Animator : public Component
{
public:
	static size_t StaticClass()
	{
		static char id = 0;
		return (size_t)&id;
	}

private:
	//static_assert(is_enum_v<StateEnum>, "열거형만 설정할 수 있습니다");

	vector<Texture*> _animationList;
	Texture* _currentAnimation = nullptr;
	float fElapsedTime = 0.f;

	bool bPlaying = false;
	bool bIsFlipped = false;

public:
	virtual void InitComponent(int32 endIndex);
	virtual void UpdateComponent(float deltaTime) override;
	virtual void RenderComponent(HDC hdc) override;

	void AddAnimation(int32 Index, Texture* sprite);
	void SetAnimation(int32 Index);

	bool IsAnimationFinished();

	void SetFlipped(bool isFlip) { bIsFlipped = isFlip; }
};

