#pragma once
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#include <wrl.h>
#include <memory>
#include <unordered_map>
#include <queue>
#include <thread>
#include <mutex>
#include "Audio/Audio.h"

class AudioManager {
	friend Audio;

private:
	struct LoadStatus {
		std::string fileName_;
		bool loopFlg_;
		Audio** audio_;
	};

private:
	AudioManager();
	AudioManager(const AudioManager&) = delete;
	AudioManager(AudioManager&&) noexcept = delete;
	~AudioManager();
	AudioManager& operator=(const AudioManager&) = delete;
	AudioManager& operator=(AudioManager&&) noexcept = delete;

public:
	static void Inititalize();
	static void Finalize();
	static inline AudioManager* GetInstance() {
		return instance;
	}

private:
	static AudioManager* instance;

public:
	Audio* LoadWav(const std::string& fileName, bool loopFlg);
	void LoadWav(const std::string& fileName, bool loopFlg,Audio** audio);

public:
	void ThreadLoad();

	void CheckThreadLoadFinish();
private:
	void JoinThread();

private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_;

	std::unordered_map<std::string, std::unique_ptr<Audio>> audios_;

	std::queue<LoadStatus> threadAudioBuff_;
	std::thread load_;
	std::mutex mtx_;
	bool isThreadLoadFinish_;
};