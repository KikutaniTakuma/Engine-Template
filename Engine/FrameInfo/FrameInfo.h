#pragma once
#include <chrono>

class FrameInfo {
/// <summary>
/// コンストラクタ
/// </summary>
private:
	FrameInfo();
	FrameInfo(const FrameInfo&) = delete;
	FrameInfo(FrameInfo&&) = delete;
	~FrameInfo();

	FrameInfo& operator=(const FrameInfo&) = delete;
	FrameInfo& operator=(FrameInfo&&) = delete;

/// <summary>
/// シングルトン
/// </summary>
public:
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	static FrameInfo* const GetInstance();

/// <summary>
/// メンバ関数
/// </summary>
public:
	/// <summary>
	/// フレーム開始時関数
	/// </summary>
	void Start();

	/// <summary>
	/// フレーム終了時関数
	/// </summary>
	void End();

/// <summary>
/// getter
/// </summary>
public:
	/// <summary>
	/// デルタタイム取得
	/// </summary>
	/// <returns>デルタタイム</returns>
	inline float GetDelta() const {
		return deltaTime_;
	}

	/// <summary>
	/// fps取得
	/// </summary>
	/// <returns>fps</returns>
	inline float GetFps() const {
		return fps_;
	}

	/// <summary>
	/// フレームカウント取得
	/// </summary>
	/// <returns>フレームカウント</returns>
	inline size_t GetFrameCount() const {
		return frameCount_;
	}

/// <summary>
/// メンバ変数
/// </summary>
private:
	std::chrono::steady_clock::time_point frameStartTime_;
	float deltaTime_;
	float fps_;
	size_t frameCount_;

	std::chrono::steady_clock::time_point gameStartTime_;
};