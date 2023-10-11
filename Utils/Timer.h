#pragma once
#include <chrono>

class Timer {
public:
	Timer();
	Timer(const Timer&) = default;
	Timer(Timer&&) = default;
	~Timer() = default;

	Timer& operator=(const Timer&) = default;
	Timer& operator=(Timer&&) = default;

public:
	void Start();

	void Update();

	/// <summary>
	/// 引数時間ごとにtrueを返す(それ以外はfalse)
	/// </summary>
	/// <param name="millSeconds">ミリ秒</param>
	/// <returns></returns>
	bool GetDuration(size_t millSeconds);
	std::chrono::milliseconds GetStartFromTime();

	
private:
	std::chrono::steady_clock::time_point startTime_;
	std::chrono::steady_clock::time_point nowTime_;
	std::chrono::steady_clock::time_point durationTime_;
};