#pragma once
#include "../Utils/Singleton.h"

class AudioSource;
class IXAudio2;
class IXAudio2MasteringVoice;

struct SFXVoice {
	IXAudio2SourceVoice* voice;
	bool inUse;
	std::chrono::steady_clock::time_point startTime;
};

class SoundManager : public Singleton<SoundManager>
{
	friend Singleton<SoundManager>;
private:
	SoundManager() {}

	IXAudio2* _xaudio2 = nullptr;
	IXAudio2MasteringVoice* _masterVoice = nullptr;
	unordered_map<ESFXType, AudioSource*> _sfxMap;
	unordered_map<EBGMType, AudioSource*> _bgmMap;

	IXAudio2SourceVoice* _currentBGM = nullptr;

	// SFX 보이스 풀 (동시 재생을 위해)
	static constexpr int MAX_SFX_VOICES = 16;
	std::vector<SFXVoice> _sfxVoicePool;

	// 볼륨 설정
	float _masterVolume = 1.0f;
	float _bgmVolume = 1.0f;
	float _sfxVolume = 1.0f;

	float _globalSpeed = 1.f;
	float _bgmBaseSpeed = 1.0f; // BGM 기본 속도
	float _targetSpeed = 1.0f;   // 목표 속도 (부드러운 전환용)
	float _speedTransitionRate = 4.0f; // 속도 전환 속도

public:
	void Init(fs::path directory);
	void Update();

	void PlayBGM(EBGMType bgmType, float volume = 1.0f, float speed = 1.0f, bool loop = false);
	void StopBGM();
	void PauseBGM();
	void ResumeBGM();
	void PlaySFX(ESFXType sfxType, float volume, float speed);
	void StopAllSFX();

	void SetGlobalSpeed(float speed, bool immediate = false);
	void SetBGMSpeed(float speed);
	float GetGlobalSpeed() const { return _globalSpeed; }

	// 볼륨 제어
	void SetMasterVolume(float volume);
	void SetBGMVolume(float volume);
	void SetSFXVolume(float volume);

private:
	IXAudio2SourceVoice* GetAvailableSFXVoice();
	void ReleaseSFXVoice(IXAudio2SourceVoice* voice);
};


