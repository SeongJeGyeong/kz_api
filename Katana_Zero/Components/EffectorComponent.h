#pragma once
#include "Component.h"

class Sprite;

struct SpawnInfo
{
	string effectName;
	Vector2 vPos;

	bool bLoop;
	int32 iFrameCount;
	int32 iCurrentFrame;
	float fDuration;

	float fElapsedTime;
	float fRadAngle;
	float fScale;
	bool bAttached;
	bool bIsFlipped;
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
	vector<SpawnInfo> _spawnList;

public:
	virtual void InitComponent() override;
	virtual void UpdateComponent(float deltaTime) override;
	virtual void RenderComponent(HDC hdc) override;

	void Destroy();

	void PlayEffect(string name, bool flipped, float radian, float scale, bool attached, Vector2 pos = Vector2(0, 0));

	virtual uint32 GetPriority() override { return EComponentPriority::EFFECT; }
};

