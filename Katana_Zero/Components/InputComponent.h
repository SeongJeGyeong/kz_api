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

	bool bPressedDown = false;

public:
	virtual void InitComponent(Actor* owner);
	virtual void UpdateComponent(float deltaTime) override;
	virtual void RenderComponent(HDC hdc) override;

	virtual uint32 GetPriority() override { return EComponentPriority::INPUT; }
	bool GetPressedDown() { return bPressedDown; }


	void Input_In_Idle(float deltaTime);
	void Input_In_IdleToRun(float deltaTime);
	void Input_In_Run(float deltaTime);
	void Input_In_RunToIdle(float deltaTime);
	void Input_In_Jump(float deltaTime);
	void Input_In_Fall(float deltaTime);
	void Input_In_PreCrouch(float deltaTime);
	void Input_In_Crouch(float deltaTime);
	void Input_In_PostCrouch(float deltaTime);
	void Input_In_Attack(float deltaTime);
	void Input_In_Roll(float deltaTime);
};

