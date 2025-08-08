#pragma once
#include "Component.h"

class Texture;

struct SpawnInfo
{
	Vector2 vPos;
	int32 iCurFrame;
	float fElapsedTime;
	float fRadAngle;
	float fScale;
	bool bAttached;
	bool bIsFlipped;
	bool bLoop;
};

class EffectorComponent : public Component
{
	using Super = Component;
public:
	static size_t StaticClass()
	{
		static char id = 0;
		return (size_t)&id;
	}

private:
	Texture* _effectSprite = nullptr;
	vector<SpawnInfo> _spawnList;

public:
	virtual void InitComponent() override;
	virtual void UpdateComponent(float deltaTime) override;
	virtual void RenderComponent(HDC hdc) override;

	void Destroy();

	void SetSprite();
	void PlayEffect(bool flipped, float radian, float scale, bool attached, Vector2 pos = Vector2(0, 0));

	virtual void print() override 
	{
		OutputDebugString(L"Effector\n");
	}
};

