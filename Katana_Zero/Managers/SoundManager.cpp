#include "pch.h"
#include "SoundManager.h"
#include "../Resources/AudioSource.h"

void SoundManager::Init(fs::path directory)
{
	// COM �ʱ�ȭ
	HRESULT hr;
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr)) {
		std::cerr << "COM �ʱ�ȭ ����" << std::endl;
		return;
	}

	// XAudio2 �ν��Ͻ� ����
	hr = XAudio2Create(&_xaudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(hr)) {
		std::cerr << "XAudio2 ���� ����" << std::endl;
		return;
	}

	// �����͸� ���̽� ����
	hr = _xaudio2->CreateMasteringVoice(&_masterVoice);
	if (FAILED(hr)) {
		std::cerr << "�����͸� ���̽� ���� ����" << std::endl;
		return;
	}

    fs::path path = directory / L"Json/Audios.json";
	std::ifstream file(path.c_str());
	if (!file.is_open())
	{
		MessageBox(nullptr, L"Failed to open JSON file", L"Error", MB_OK);
		return;
	}

	json data = json::parse(file);

	fs::path bgmPath = directory / L"Audios/BGM/";


	for (auto& item : data["BGM"])
	{
		AudioSource* audio = new AudioSource();
		string fileName = item.get<string>();
		fs::path filePath = bgmPath / fileName;
		filePath.replace_extension(".wav");
		OutputDebugStringW((filePath.wstring() + L"\n").c_str());
		if (audio->LoadWavFile(filePath))
		{
			if (fileName == "song_rainonbrick")
				_bgmMap[EBGMType::MAIN_MENU] = audio;
			else if (fileName == "song_katanazero")
				_bgmMap[EBGMType::STAGE] = audio;
			else if (fileName == "song_bossbattle")
				_bgmMap[EBGMType::BOSS_ROOM] = audio;
		}
	}
}

void SoundManager::Update()
{
	// �ε巯�� �ӵ� ��ȯ ó��
	if (_globalSpeed != _targetSpeed) {
		float delta = 0.016f; // 60 FPS ����, �����δ� deltaTime ��� ����
		float speedDiff = _targetSpeed - _globalSpeed;
		float speedChange = speedDiff * _speedTransitionRate * delta;

		if (std::abs(speedDiff) < 0.01f) {
			_globalSpeed = _targetSpeed;
		}
		else {
			_globalSpeed += speedChange;
		}

		// ���� BGM �ӵ� ������Ʈ
		if (_currentBGM) {
			_currentBGM->SetFrequencyRatio(_bgmBaseSpeed * _globalSpeed);
		}

		// ��� ���� ��� SFX �ӵ� ������Ʈ
		for (auto& sfxVoice : _sfxVoicePool) {
			if (sfxVoice.inUse && sfxVoice.voice) {
				XAUDIO2_VOICE_STATE state;
				sfxVoice.voice->GetState(&state);
				if (state.BuffersQueued > 0) {
					sfxVoice.voice->SetFrequencyRatio(_globalSpeed);
				}
			}
		}
	}

	// ����� ���� SFX ���̽� ����
	auto now = std::chrono::steady_clock::now();
	for (auto& sfxVoice : _sfxVoicePool) {
		if (sfxVoice.inUse && sfxVoice.voice) {
			XAUDIO2_VOICE_STATE state;
			sfxVoice.voice->GetState(&state);

			if (state.BuffersQueued == 0) {
				// ����� �������Ƿ� ����
				sfxVoice.voice->Stop();
				sfxVoice.voice->FlushSourceBuffers();
				sfxVoice.inUse = false;
			}
			else {
				// Ÿ�Ӿƿ� üũ (10��)
				auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
					now - sfxVoice.startTime).count();
				if (elapsed > 10) {
					sfxVoice.voice->Stop();
					sfxVoice.voice->FlushSourceBuffers();
					sfxVoice.inUse = false;
				}
			}
		}
	}
}

void SoundManager::PlayBGM(EBGMType bgmType, float volume, float speed, bool loop)
{
	auto it = _bgmMap.find(bgmType);
	if (it == _bgmMap.end()) return;

	if (_currentBGM)
	{
		_currentBGM->Stop();
		_currentBGM->DestroyVoice();
		_currentBGM = nullptr;
	}

	AudioSource* audio = it->second;
	if (!audio || !audio->IsLoaded()) return;

	// �� �ҽ� ���̽� ����
	HRESULT hr = _xaudio2->CreateSourceVoice(&_currentBGM, audio->_wfx);
	if (FAILED(hr)) {
		OutputDebugString(L"Failed to create source voice for BGM\n");
		return;
	}

	// ���� ����
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = audio->_bufferSize;
	buffer.pAudioData = audio->_buffer;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	if (loop) {
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	// ������ �ӵ� ����
	_currentBGM->SetVolume(volume * _bgmVolume * _masterVolume);
	_currentBGM->SetFrequencyRatio(speed * _globalSpeed);

	// ���� ���� �� ���
	hr = _currentBGM->SubmitSourceBuffer(&buffer);
	if (SUCCEEDED(hr)) {
		_currentBGM->Start(0);
		//_currentBGMType = bgmType;
		OutputDebugString(L"BGM started playing\n");
	}
}

void SoundManager::StopBGM()
{
	if (_currentBGM) {
		_currentBGM->Stop();
		_currentBGM->FlushSourceBuffers();
		_currentBGM->DestroyVoice();
		_currentBGM = nullptr;
		OutputDebugString(L"BGM stopped\n");
	}
}

void SoundManager::PauseBGM()
{
	if (_currentBGM) {
		_currentBGM->Stop(XAUDIO2_PLAY_TAILS);
	}
}

void SoundManager::ResumeBGM()
{
	if (_currentBGM) {
		_currentBGM->Start();
	}
}

void SoundManager::PlaySFX(ESFXType sfxType, float volume, float speed)
{
	auto it = _sfxMap.find(sfxType);
	if (it == _sfxMap.end()) {
		OutputDebugString(L"SFX not found\n");
		return;
	}

	AudioSource* audio = it->second;
	if (!audio || !audio->IsLoaded()) return;

	// ��� ������ ���̽� ã��
	IXAudio2SourceVoice* voice = GetAvailableSFXVoice();
	if (!voice) {
		// �� ���̽� ����
		for (auto& sfxVoice : _sfxVoicePool) {
			if (!sfxVoice.voice) {
				HRESULT hr = _xaudio2->CreateSourceVoice(&sfxVoice.voice, audio->_wfx);
				if (SUCCEEDED(hr)) {
					voice = sfxVoice.voice;
					sfxVoice.inUse = true;
					sfxVoice.startTime = std::chrono::steady_clock::now();
					break;
				}
			}
		}
	}

	if (!voice) {
		OutputDebugString(L"No available SFX voice\n");
		return;
	}

	// ���� ����
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = audio->_bufferSize;
	buffer.pAudioData = audio->_buffer;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	// ������ �ӵ� ����
	voice->SetVolume(volume * _sfxVolume * _masterVolume);
	voice->SetFrequencyRatio(speed * _globalSpeed);

	// ���� ���� ���� �� �� ���� ����
	voice->Stop();
	voice->FlushSourceBuffers();
	voice->SubmitSourceBuffer(&buffer);
	voice->Start(0);
}

void SoundManager::StopAllSFX()
{
	for (auto& sfxVoice : _sfxVoicePool) {
		if (sfxVoice.voice) {
			sfxVoice.voice->Stop();
			sfxVoice.voice->FlushSourceBuffers();
			sfxVoice.voice->DestroyVoice();
			sfxVoice.voice = nullptr;
			sfxVoice.inUse = false;
		}
	}
}

void SoundManager::SetGlobalSpeed(float speed, bool immediate)
{
	speed = std::clamp(speed, 0.1f, 2.0f); // 0.1x ~ 2x �ӵ� ����

	if (immediate) {
		_globalSpeed = speed;
		_targetSpeed = speed;

		// BGM ��� �ӵ� ����
		if (_currentBGM) {
			_currentBGM->SetFrequencyRatio(_bgmBaseSpeed * _globalSpeed);
		}

		// ��� ��� ���� SFX ��� �ӵ� ����
		for (auto& sfxVoice : _sfxVoicePool) {
			if (sfxVoice.inUse && sfxVoice.voice) {
				XAUDIO2_VOICE_STATE state;
				sfxVoice.voice->GetState(&state);
				if (state.BuffersQueued > 0) {
					sfxVoice.voice->SetFrequencyRatio(_globalSpeed);
				}
			}
		}
	}
	else {
		// �ε巯�� ��ȯ�� ���� ��ǥ �ӵ��� ����
		_targetSpeed = speed;
	}
}

void SoundManager::SetBGMSpeed(float speed)
{
	_bgmBaseSpeed = std::clamp(speed, 0.1f, 2.0f);

	if (_currentBGM) {
		_currentBGM->SetFrequencyRatio(_bgmBaseSpeed * _globalSpeed);
	}
}

void SoundManager::SetMasterVolume(float volume)
{
	_masterVolume = std::clamp(volume, 0.0f, 1.0f);

	// ���� ��� ���� BGM ���� ������Ʈ
	if (_currentBGM) {
		_currentBGM->SetVolume(_bgmVolume * _masterVolume);
	}
}

void SoundManager::SetBGMVolume(float volume)
{
	_bgmVolume = std::clamp(volume, 0.0f, 1.0f);

	if (_currentBGM) {
		_currentBGM->SetVolume(_bgmVolume * _masterVolume);
	}
}

void SoundManager::SetSFXVolume(float volume)
{
	_sfxVolume = std::clamp(volume, 0.0f, 1.0f);
}

IXAudio2SourceVoice* SoundManager::GetAvailableSFXVoice()
{
	for (auto& sfxVoice : _sfxVoicePool) {
		if (sfxVoice.voice && !sfxVoice.inUse) {
			sfxVoice.inUse = true;
			sfxVoice.startTime = std::chrono::steady_clock::now();
			return sfxVoice.voice;
		}
	}
	return nullptr;
}

void SoundManager::ReleaseSFXVoice(IXAudio2SourceVoice* voice)
{
}
