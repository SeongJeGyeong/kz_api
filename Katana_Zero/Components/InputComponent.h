#pragma once
#include "Component.h"

class Player;

class InputComponent : public Component
{
public:
	static size_t StaticClass()
	{
		static int id = 0;
		return (size_t)&id;
	}
private:
	Player* _player = nullptr;

public:
	virtual void InitComponent(Actor* owner);
	virtual void UpdateComponent(float deltaTime) override;
	virtual void RenderComponent(HDC hdc) override;
};

