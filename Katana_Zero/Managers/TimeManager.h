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
	float fDeltaTime = 0.f;				// 슬로모션에 영향을 받음
	float fConstantDeltaTime = 0.f;		// 슬로모션에 영향받지 않음
	float fTimeScale = 1.0f;

	// 초당 몇 프레임인지
	uint32 iFrameCount = 0;
	float fFrameTime = 0.f;
	uint32 iFps = 0;

	bool bSlowMotion = false;
	float fTargetTimeScale = 1.f;
	float fTransitionSpeed;
	bool bIsTransitioning;
	float fSlowMotionMaxGuage = 5.5f;
	float fSlowMotionCountGauge = 0.f;
	int32 iSlowMotionBatteryCount = 11;

	bool IsHitStop = false;
	float hitStopDuration = 0.f;
	float hitStopTime = 0.2f;

	float fMaskAlpha = 0.f;
	float fPrevTimeScale = 1.f;

public:
	void Init();
	void Update();
	void StartSlowMotion(float transitionSpeed);
	void EndSlowMotion(float transitionSpeed);
	//void ToggleSlowMotion(float scale, float transitionSpeed);

	uint64 GetFps() { return iFps; }
	float GetDeltaTime() { return fDeltaTime; }
	float GetConstDeltaTime() { return fConstantDeltaTime; }

	void SetPrevCount(uint64 now) { iPrevCount = now; }

	bool GetSlowMotion() { return bSlowMotion; }
	void SetSlowMotion(bool slowMotion) { bSlowMotion = slowMotion; }
	bool GetSlowMotionTransitioning() { return bIsTransitioning; }
	float GetMaskAlpha() { return fMaskAlpha; }

	bool GetHitStop() { return IsHitStop; }
	void SetHitStop() { IsHitStop = true; }
	float GetHitStopTime() { return hitStopTime; }

	int32 GetSlowMotionBatteryCount() { return iSlowMotionBatteryCount; }

	int32 GetRandom()
	{
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);
		return counter.LowPart & 1;
	}
};

