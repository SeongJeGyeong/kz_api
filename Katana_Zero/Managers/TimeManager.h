#pragma once
#include "../Utils/Singleton.h"

class TimeManager : public Singleton<TimeManager>
{
	friend Singleton<TimeManager>;

private:
	TimeManager() {}

	uint64 iFrequency = 0;	// CPU Ŭ��
	uint64 iPrevCount = 0;

	// ���� �����Ӱ� ���� ������ ������ �ð�
	float fDeltaTime = 0.f;

	// �ʴ� �� ����������
	uint32 iFrameCount = 0;
	float fFrameTime = 0.f;
	uint32 iFps = 0;

public:
	void Init();
	void Update();

	uint64 GetFps() { return iFps; }
	float GetDeltaTime() { return fDeltaTime; }

	void SetPrevCount(uint64 now) { iPrevCount = now; }
};

