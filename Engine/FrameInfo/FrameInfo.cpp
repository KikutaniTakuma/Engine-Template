#include "FrameInfo.h"
#include "Engine/Log/Log.h"

#include <cmath>
#include <thread>
#include <Windows.h>
#undef max
#undef min
#include <fstream>
#include <string>
#include <format>
#include <algorithm>

FrameInfo::FrameInfo() :
	frameStartTime_(),
	deltaTime_(0.0),
	fps_(0.0),
	maxFps_(0.0),
	minFps_(0.0),
	frameCount_(0)
{
	//画面情報構造体
	DEVMODE mode{};

	//現在の画面情報を取得
	EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &mode);

	// リフレッシュレート取得
	fps_ = static_cast<float>(mode.dmDisplayFrequency);
	minFps_ = maxFps_ = fps_;
	deltaTime_ = 1.0f / fps_;

	gameStartTime_ = std::chrono::steady_clock::now();
}

FrameInfo::~FrameInfo() {
	auto end = std::chrono::steady_clock::now();

	//画面情報構造体
	DEVMODE mode{};

	//現在の画面情報を取得
	EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &mode);

	auto playtime =
		std::chrono::duration_cast<std::chrono::milliseconds>(end - gameStartTime_);

	double theoreticalFrameCount = (static_cast<double>(playtime.count()) / 1000.0) * static_cast<double>(mode.dmDisplayFrequency);

	maxFps_ = std::clamp(maxFps_, 0.0, static_cast<double>(mode.dmDisplayFrequency));
	minFps_ = std::clamp(minFps_, 0.0, static_cast<double>(mode.dmDisplayFrequency));

	Log::AddLog(std::format("Average Fps : {:.2f}\n", static_cast<double>(mode.dmDisplayFrequency) * (static_cast<double>(frameCount_) / theoreticalFrameCount)));
	if (std::chrono::duration_cast<std::chrono::seconds>(end - gameStartTime_) > std::chrono::seconds(1)) {
		Log::AddLog(std::format("Max Fps : {:.2f}\n", maxFps_));
		Log::AddLog(std::format("Min Fps : {:.2f}\n", minFps_));
	}
}

FrameInfo* const FrameInfo::GetInstance() {
	static FrameInfo instance;

	return &instance;
}

void FrameInfo::Start() {
	frameStartTime_ = std::chrono::steady_clock::now();
}

void FrameInfo::End() {
	static std::chrono::steady_clock::time_point end{};
	end = std::chrono::steady_clock::now();

	// 10^-6
	static constexpr double unitAdjustment = 0.000001;

	auto frameTime = std::chrono::duration_cast<std::chrono::microseconds>(end - frameStartTime_);
	
	deltaTime_ = static_cast<double>(frameTime.count()) * unitAdjustment;
	fps_ = 1.0f / deltaTime_;

	if (std::chrono::duration_cast<std::chrono::seconds>(end - gameStartTime_) > std::chrono::seconds(1)) {
		maxFps_ = std::max(fps_, maxFps_);
		minFps_ = std::min(fps_, minFps_);
	}

	frameCount_++;
}