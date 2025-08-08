#include "pch.h"
#include "TimeManager.h"

void TimeManager::Init()
{
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&iFrequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&iPrevCount)); // CPU 클럭
}

void TimeManager::Update()
{
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	fDeltaTime = (currentCount - iPrevCount) / static_cast<float>(iFrequency);
	iPrevCount = currentCount;

	// 프레임 계산
	iFrameCount++;
	fFrameTime += fDeltaTime;

	// 1초당 프레임
	if (fFrameTime >= 1.0f)
	{
		iFps = iFrameCount;
		fFrameTime -= 1.0f;
		iFrameCount = 0;
	}
}