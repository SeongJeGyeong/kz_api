#pragma once

// WAV ���� ��� ����ü
struct WAVHeader {
    char riff[4];           // "RIFF"
    uint32_t size;          // ���� ũ�� - 8
    char wave[4];           // "WAVE"
};

struct WAVChunk {
    char id[4];             // ûũ ID
    uint32_t size;          // ûũ ũ��
};

struct WAVFormat {
    uint16_t audioFormat;   // ����� ���� (1 = PCM)
    uint16_t numChannels;   // ä�� ��
    uint32_t sampleRate;    // ���ø� ����Ʈ
    uint32_t byteRate;      // ����Ʈ ����Ʈ
    uint16_t blockAlign;    // ��� ����
    uint16_t bitsPerSample; // ��Ʈ ����
};

class AudioSource
{
public:
    WAVEFORMATEX* _wfx = nullptr;
    uint8_t* _buffer = nullptr;
    uint32_t  _bufferSize = 0;
    XAUDIO2_BUFFER _xaBuffer = {0};

    float _duration = 0.f;
    bool _isLoaded = false;

public:
    bool LoadWavFile(const fs::path& filepath);
    void Cleanup();

    float GetDuration() const { return _duration; }
    bool IsLoaded() const { return _isLoaded; }

    //void PlayAudio(class IXAudio2& xaudio2, float volume, float speed, bool loop);
};

