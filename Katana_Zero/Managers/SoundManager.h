#pragma once
#include "../Utils/Singleton.h"

class AudioSource;
class IXAudio2;
class IXAudio2MasteringVoice;

struct SFXVoice {
	IXAudio2SourceVoice* voice;
	ESFXType type;
	bool inUse;
	bool isLooping;
	std::chrono::steady_clock::time_point startTime;
};

class SoundManager : public Singleton<SoundManager>
{
	friend Singleton<SoundManager>;
private:
	SoundManager() {}

	IXAudio2* _xaudio2 = nullptr;
	IXAudio2MasteringVoice* _masterVoice = nullptr;
	unordered_map<string, AudioSource*> _sfxMap;

	unordered_map<int32, AudioSource*> _playerRunningSfx;
	unordered_map<int32, AudioSource*> _playerSlashSfx;
	unordered_map<int32, AudioSource*> _enemyWalkSfx;
	unordered_map<int32, AudioSource*> _enemyRunSfx;
	unordered_map<int32, AudioSource*> _bloodSfx;

	unordered_map<EBGMType, AudioSource*> _bgmMap;

	IXAudio2SourceVoice* _currentBGM = nullptr;
	EBGMType _currentBGMType = EBGMType::MAIN_MENU;

	// SFX Ǯ (���� ����� ����)
	static constexpr int MAX_SFX = 32;
	vector<SFXVoice> _sfxPool;
	SFXVoice _slowMotionSfx;

	// ���� ����
	float _masterVolume = 0.5f;
	float _bgmVolume = 0.5f;
	float _sfxVolume = 0.5f;

	float _globalSpeed = 1.f;
	float _bgmBaseSpeed = 1.0f; // BGM �⺻ �ӵ�
	float _targetSpeed = 1.0f;   // ��ǥ �ӵ� (�ε巯�� ��ȯ��)
	float _speedTransitionRate = 4.0f; // �ӵ� ��ȯ �ӵ�

public:
	void Init(fs::path directory);
	void Update();

	void PlayBGM(EBGMType bgmType, bool loop = false, float volume = 1.0f, float speed = 1.0f);
	void StopBGM();
	void PauseBGM();
	void ResumeBGM();
	void PlaySFX(string sfxName, bool loop = false, float volume = 1.f, float speed = 1.f, ESFXType type = ESFXType::SFX_DEFAULT);
	void StopAllSFX();

	void SetGlobalSpeed(float speed, bool immediate = false);
	void SetBGMSpeed(float speed);
	float GetGlobalSpeed() const { return _globalSpeed; }

	// ���� ����
	void SetMasterVolume(float volume);
	void SetBGMVolume(float volume);
	void SetSFXVolume(float volume);

	void PlayPlayerSlash();
	void PlayPlayerRun();
	void PlayEnemyWalk();
	void PlayEnemyRun();
	void PlayEnemySlashKill();
	void PlayBloodSquirt();
	void PlaySlowMotion(bool engage);

	void SetSFX(AudioSource* audio, float volume, float speed, bool loop, ESFXType type = ESFXType::SFX_DEFAULT);
	void StopSFX(ESFXType type);

private:
	IXAudio2SourceVoice* GetAvailableSFXVoice();
	void ReleaseSFXVoice(IXAudio2SourceVoice* voice);
};


