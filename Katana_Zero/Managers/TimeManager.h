#pragma once
#include "../Utils/Singleton.h"

class TimeManager : public Singleton<TimeManager>
{
	friend Singleton<TimeManager>;

private:
	TimeManager() {}

	uint64 iFrequency = 0;	// CPU 클럭
	uint64 iPrevCount = 0;

	// 이전 프레임과 현재 프레임 사이의 시간
	float fDeltaTime = 0.f;

	// 초당 몇 프레임인지
	uint32 iFrameCount = 0;
	float fFrameTime = 0.f;
	uint32 iFps = 0;

public:
	void Init();
	void Update();

	uint64 GetFps() { return iFps; }
	float GetDeltaTime() { return fDeltaTime; }
};

