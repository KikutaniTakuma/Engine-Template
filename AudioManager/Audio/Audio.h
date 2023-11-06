#pragma once
#include <array>
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

#include <string>

/// <summary>
/// 音の再生や停止、ロードを担う
/// </summary>
class Audio {
	friend class AudioManager;

private:
	struct ChunkHeader {
		std::array<char,4> id;
		int32_t size;
	};
	struct RiffHeader {
		ChunkHeader chunk;
		std::array<char, 4> type;
	};
	struct FormatChunk
	{
		ChunkHeader chunk;
		WAVEFORMATEX fmt;
	};

public:
	Audio();
	Audio(const Audio&) = delete;
	Audio(Audio&&) = delete;
	~Audio();

	Audio& operator=(const Audio&) = delete;
	Audio& operator=(Audio&&) = delete;

public:
	void Start(float volume);

	void Pause();

	void ReStart();

	void Stop();

	bool IsStart() const {
		return isStart_;
	}

	void SetAudio(float volume);

	void Debug(const std::string& guiName);

private:
	void Load(const std::string& fileName, bool loopFlg_);

private:
	WAVEFORMATEX wfet_;
	BYTE* pBuffer_;
	uint32_t bufferSize_;
	IXAudio2SourceVoice* pSourceVoice_;
	bool loopFlg_;
	bool isStart_;
	float volume_;

	bool isLoad_;
	std::string fileName_;
};