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

	// SFX ���̽� Ǯ �ʱ�ȭ
	//_sfxPool.resize(MAX_SFX);
	//for (auto& sfx : _sfxPool) {
	//	sfx.voice = nullptr;
	//	sfx.inUse = false;
	//	sfx.isLooping = false;
	//}

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

	fs::path sfxPath = directory / L"Audios/SFX/";
	for (auto& item : data["SFX"]["Main"])
	{
		AudioSource* audio = new AudioSource();
		string fileName = item.get<string>();
		fs::path filePath = sfxPath / fileName;
		filePath.replace_extension(".wav");
		if (audio->LoadWavFile(filePath))
		{
			_sfxMap[fileName] = audio;
		}
	}

	int32 idx = 0;
	for (auto& item : data["SFX"]["PlayerRunning"])
	{
		AudioSource* audio = new AudioSource();
		string fileName = item.get<string>();
		fs::path filePath = sfxPath / fileName;
		filePath.replace_extension(".wav");
		if (audio->LoadWavFile(filePath))
		{
			_playerRunningSfx[idx++] = audio;
		}
	}
	idx = 0;
	for (auto& item : data["SFX"]["PlayerSlash"])
	{
		AudioSource* audio = new AudioSource();
		string fileName = item.get<string>();
		fs::path filePath = sfxPath / fileName;
		filePath.replace_extension(".wav");
		if (audio->LoadWavFile(filePath))
		{
			_playerSlashSfx[idx++] = audio;
		}
	}
	idx = 0;
	for (auto& item : data["SFX"]["EnemyWalk"])
	{
		AudioSource* audio = new AudioSource();
		string fileName = item.get<string>();
		fs::path filePath = sfxPath / fileName;
		filePath.replace_extension(".wav");
		if (audio->LoadWavFile(filePath))
		{
			_enemyWalkSfx[idx++] = audio;
		}
	}
	idx = 0;
	for (auto& item : data["SFX"]["EnemyRun"])
	{
		AudioSource* audio = new AudioSource();
		string fileName = item.get<string>();
		fs::path filePath = sfxPath / fileName;
		filePath.replace_extension(".wav");
		if (audio->LoadWavFile(filePath))
		{
			_enemyRunSfx[idx++] = audio;
		}
	}
	idx = 0;
	for (auto& item : data["SFX"]["BloodSquirt"])
	{
		AudioSource* audio = new AudioSource();
		string fileName = item.get<string>();
		fs::path filePath = sfxPath / fileName;
		filePath.replace_extension(".wav");
		if (audio->LoadWavFile(filePath))
		{
			_bloodSfx[idx++] = audio;
		}
	}

	_slowMotionSfx.voice = nullptr;
	_slowMotionSfx.inUse = false;
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
		for (auto& sfx : _sfxPool) {
			if (sfx.inUse && sfx.voice) {
				XAUDIO2_VOICE_STATE state;
				sfx.voice->GetState(&state);
				if (state.BuffersQueued > 0) {
					sfx.voice->SetFrequencyRatio(_globalSpeed);
				}
			}
		}
	}

	// ����� ���� SFX ���̽� ����
	auto now = std::chrono::steady_clock::now();
	for (auto it = _sfxPool.begin(); it != _sfxPool.end();)
	{
		if (it->inUse && it->voice && !it->isLooping)
		{
			XAUDIO2_VOICE_STATE state;
			it->voice->GetState(&state);
			if (state.BuffersQueued == 0) {
				// ����� �������Ƿ� ����
				it->voice->Stop();
				it->voice->FlushSourceBuffers();
				it->inUse = false;
				it->isLooping = false;
				it = _sfxPool.erase(it);
				continue;
			}
			else 
			{
				// Ÿ�Ӿƿ� üũ (10��)
				auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - it->startTime).count();
				if (elapsed > 10) {
					it->voice->Stop();
					it->voice->FlushSourceBuffers();
					it->inUse = false;
					it->isLooping = false;
					it = _sfxPool.erase(it);
					continue;
				}
			}

			++it;
		}
	}

	//for (auto& sfx : _sfxPool) {
	//	if (sfx.inUse && sfx.voice && !sfx.isLooping) {
	//		XAUDIO2_VOICE_STATE state;
	//		sfx.voice->GetState(&state);

	//		if (state.BuffersQueued == 0) {
	//			// ����� �������Ƿ� ����
	//			sfx.voice->Stop();
	//			sfx.voice->FlushSourceBuffers();
	//			sfx.inUse = false;
	//			sfx.isLooping = false;
	//		}
	//		else {
	//			// Ÿ�Ӿƿ� üũ (10��)
	//			auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - sfx.startTime).count();
	//			if (elapsed > 10) {
	//				sfx.voice->Stop();
	//				sfx.voice->FlushSourceBuffers();
	//				sfx.inUse = false;
	//				sfx.isLooping = false;
	//			}
	//		}
	//	}
	//}
}

void SoundManager::PlayBGM(EBGMType bgmType, bool loop, float volume, float speed)
{
	auto it = _bgmMap.find(bgmType);
	if (it == _bgmMap.end()) return;

	if (_currentBGM)
	{
		// ���� BGM�� ���� Ÿ���̸� ����
		if (_currentBGM && _currentBGMType == bgmType) {
			return;
		}

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
		_currentBGMType = bgmType;
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

void SoundManager::PlaySFX(string sfxName, bool loop, float volume, float speed, ESFXType type)
{
	auto it = _sfxMap.find(sfxName);
	if (it == _sfxMap.end()) {
		OutputDebugString(L"SFX not found\n");
		return;
	}

	AudioSource* audio = it->second;
	if (!audio || !audio->IsLoaded()) return;

	SetSFX(audio, volume, speed, loop, type);
}

void SoundManager::StopAllSFX()
{
	for (auto& sfx : _sfxPool) {
		if (sfx.voice) {
			sfx.voice->Stop();
			sfx.voice->FlushSourceBuffers();
			sfx.voice->DestroyVoice();
			sfx.voice = nullptr;
			sfx.inUse = false;
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
		for (auto& sfx : _sfxPool) {
			if (sfx.inUse && sfx.voice) {
				XAUDIO2_VOICE_STATE state;
				sfx.voice->GetState(&state);
				if (state.BuffersQueued > 0) {
					sfx.voice->SetFrequencyRatio(_globalSpeed);
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

void SoundManager::PlayPlayerSlash()
{
	auto it = _playerSlashSfx.find(rand() % 3);
	if (it == _playerSlashSfx.end()) 
	{
		OutputDebugString(L"SFX not found\n");
		return;
	}

	AudioSource* audio = it->second;
	if (!audio || !audio->IsLoaded()) return;

	SetSFX(audio, 1.f, 1.f, false);
}

void SoundManager::PlayPlayerRun()
{
	auto it = _playerRunningSfx.find(rand() % 4);
	if (it == _playerRunningSfx.end())
	{
		OutputDebugString(L"SFX not found\n");
		return;
	}

	AudioSource* audio = it->second;
	if (!audio || !audio->IsLoaded()) return;

	SetSFX(audio, 1.f, 1.f, false);
}

void SoundManager::PlayEnemyWalk()
{
}

void SoundManager::PlayEnemyRun()
{
}

void SoundManager::PlayEnemySlashKill()
{
	auto it = _sfxMap.find((rand() % 2) ? "sound_enemy_death_sword_01" : "sound_enemy_death_sword_02");
	if (it == _sfxMap.end()) {
		OutputDebugString(L"SFX not found\n");
		return;
	}

	AudioSource* audio = it->second;
	if (!audio || !audio->IsLoaded()) return;

	SetSFX(audio, 1.f, 1.f, false);
}

void SoundManager::PlayBloodSquirt()
{

}

void SoundManager::PlaySlowMotion(bool engage)
{
	auto it = _sfxMap.find(engage ? "sound_slomo_engage" : "sound_slomo_disengage");
	if (it == _sfxMap.end()) {
		OutputDebugString(L"SFX not found\n");
		return;
	}

	AudioSource* audio = it->second;
	if (!audio || !audio->IsLoaded()) return;

	if (_slowMotionSfx.voice)
	{
		_slowMotionSfx.voice->Stop();
		_slowMotionSfx.voice->FlushSourceBuffers();
		_slowMotionSfx.voice->DestroyVoice();
		_slowMotionSfx.voice = nullptr;
		_slowMotionSfx.inUse = false;
	}

	HRESULT hr = _xaudio2->CreateSourceVoice(&_slowMotionSfx.voice, audio->_wfx);
	if (FAILED(hr)) {
		OutputDebugString(L"Failed to create source voice for sfx\n");
		return;
	}

	IXAudio2SourceVoice* voice = voice = _slowMotionSfx.voice;
	_slowMotionSfx.inUse = true;
	_slowMotionSfx.startTime = std::chrono::steady_clock::now();

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
	voice->SetVolume(0.5f * _sfxVolume * _masterVolume);
	voice->SetFrequencyRatio(1.f);

	voice->SubmitSourceBuffer(&buffer);
	voice->Start(0);
}

void SoundManager::SetSFX(AudioSource* audio, float volume, float speed, bool loop, ESFXType type)
{
	// ��� ������ ���̽� ã��
	IXAudio2SourceVoice* voice = nullptr;
	SFXVoice sfx;
	HRESULT hr = _xaudio2->CreateSourceVoice(&sfx.voice, audio->_wfx);
	if (FAILED(hr)) return;

	voice = sfx.voice;
	sfx.inUse = true;
	sfx.isLooping = loop;
	sfx.type = type;
	sfx.startTime = std::chrono::steady_clock::now();

	_sfxPool.push_back(sfx);

	//if (!voice) {
	//	// �� ���̽� ����
	//	for (auto& sfx : _sfxPool) {
	//		if (!sfx.voice) {
	//			HRESULT hr = _xaudio2->CreateSourceVoice(&sfx.voice, audio->_wfx);
	//			if (SUCCEEDED(hr)) {
	//				voice = sfx.voice;
	//				sfx.inUse = true;
	//				sfx.isLooping = loop;
	//				sfx.type = type;
	//				sfx.startTime = std::chrono::steady_clock::now();
	//				break;
	//			}
	//		}
	//	}
	//}

	if (!voice) {
		OutputDebugString(L"No available SFX voice\n");
		return;
	}

	// ���� ����
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = audio->_bufferSize;
	buffer.pAudioData = audio->_buffer;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	if (loop) buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

	// ������ �ӵ� ����
	voice->SetVolume(volume * _sfxVolume * _masterVolume);
	voice->SetFrequencyRatio(speed * _globalSpeed);

	// ���� ���� ���� �� �� ���� ����
	voice->Stop();
	voice->FlushSourceBuffers();
	voice->SubmitSourceBuffer(&buffer);
	voice->Start(0);
}

void SoundManager::StopSFX(ESFXType type)
{
	if (_sfxPool.empty()) return;

	for (auto& sfx : _sfxPool) {
		if (sfx.inUse && sfx.type == type && sfx.voice) {
			sfx.voice->Stop();
			sfx.voice->FlushSourceBuffers();
			sfx.inUse = false;
			sfx.isLooping = false;
		}
	}
}

IXAudio2SourceVoice* SoundManager::GetAvailableSFXVoice()
{
	for (auto& sfx : _sfxPool) {
		if (sfx.voice && !sfx.inUse) {
			sfx.inUse = true;
			sfx.startTime = std::chrono::steady_clock::now();
			return sfx.voice;
		}
	}
	return nullptr;
}

void SoundManager::ReleaseSFXVoice(IXAudio2SourceVoice* voice)
{
}
