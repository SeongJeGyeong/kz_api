#include "pch.h"
#include "TimeManager.h"

void TimeManager::Init()
{
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&iFrequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&iPrevCount)); // CPU 클럭

	fConstantDeltaTime = 0.0f;
	fTimeScale = 1.0f;
	bSlowMotion = false;
	fTargetTimeScale = 1.0f;
	fTransitionSpeed = 2.0f;
	bIsTransitioning = false;
}

void TimeManager::Update()
{
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	fConstantDeltaTime = (currentCount - iPrevCount) / static_cast<float>(iFrequency);
	iPrevCount = currentCount;

	if (bIsTransitioning)
	{
		float diff = fTargetTimeScale - fTimeScale;
		float moveAmount = fTransitionSpeed * fConstantDeltaTime;
		if (abs(diff) <= moveAmount)
		{
			fTimeScale = fTargetTimeScale;
			bIsTransitioning = false;
			bSlowMotion = (fTimeScale < 1.f);

			fMaskAlpha = bSlowMotion ? 255.f : 0.f;
		}
		else
		{
			fTimeScale += (diff > 0 ? moveAmount : -moveAmount);

			float transitionProgress = 1.0f - (abs(diff) / abs(fTargetTimeScale - fPrevTimeScale));

			// 슬로모션 해제 중
			if (fTargetTimeScale >= 1.0f)
			{
				fMaskAlpha = 255.f * (1.0f - transitionProgress);
			}
			// 슬로모션 진입 중
			else 
			{
				fMaskAlpha = 255.f * transitionProgress;
			}

		}
	}

	if (bSlowMotion)
	{
		fSlowMotionCountGauge += fConstantDeltaTime;
		if (fSlowMotionCountGauge >= 0.5f)
		{
			fSlowMotionCountGauge = 0.f;
			iSlowMotionBatteryCount--;
		}
		if (iSlowMotionBatteryCount <= 0)
		{
			EndSlowMotion(2.f);
		}
	}
	else if(iSlowMotionBatteryCount < 11)
	{
		fSlowMotionCountGauge += fConstantDeltaTime * 0.5f;
		if (fSlowMotionCountGauge >= 0.5f)
		{
			fSlowMotionCountGauge = 0.f;
			iSlowMotionBatteryCount++;
		}
	}

	if (IsHitStop)
	{
		hitStopDuration += fConstantDeltaTime;
		if (hitStopDuration >= hitStopTime)
		{
			hitStopDuration = 0.f;
			IsHitStop = false;
		}
	}

	fDeltaTime = fConstantDeltaTime * fTimeScale;

	// 프레임 계산
	iFrameCount++;
	fFrameTime += fConstantDeltaTime;

	// 1초당 프레임
	if (fFrameTime >= 1.0f)
	{
		iFps = iFrameCount;
		fFrameTime -= 1.0f;
		iFrameCount = 0;
	}
}

void TimeManager::StartSlowMotion(float transitionSpeed)
{
	fPrevTimeScale = fTimeScale;
	fTargetTimeScale = 0.2f;
	fTransitionSpeed = transitionSpeed;
	bIsTransitioning = true;
	bSlowMotion = true;
}

void TimeManager::EndSlowMotion(float transitionSpeed)
{
	fPrevTimeScale = fTimeScale;
	fTargetTimeScale = 1.0f;
	fTransitionSpeed = transitionSpeed;
	bIsTransitioning = true;
}
//
//void TimeManager::ToggleSlowMotion(float scale, float transitionSpeed)
//{
//	if (bSlowMotion || fTargetTimeScale < 1.0f)
//	{
//		EndSlowMotion(transitionSpeed);
//	}
//	else
//	{
//		
// (scale, transitionSpeed);
//	}
//}
