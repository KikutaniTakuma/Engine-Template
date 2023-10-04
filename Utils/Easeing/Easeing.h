#pragma once
#include <functional>
#include "Utils/Math/Vector3.h"
#include <string>

class Easeing {
public:
	Easeing() = default;
	Easeing(const Easeing& right) = default;
	Easeing(Easeing&& right) noexcept = default;
	~Easeing() = default;

public:
	Easeing& operator=(const Easeing& right) = default;
	Easeing& operator=(Easeing&& right) noexcept = default;

public:
	void Update();

	/// <summary>
	/// イージングスタート
	/// </summary>
	/// <param name="start">スタート</param>
	/// <param name="end">エンド</param>
	/// <param name="isLoop">ループするか</param>
	/// <param name="easeTime">何秒でイージングするか</param>
	/// <param name="ease">イージング関数</param>
	void Start(
		const Vector3& start, 
		const Vector3& end, 
		bool isLoop, 
		float easeTime,
		std::function<float(float)> ease = [](float t) {
			return t;
		}
	);
	
	/// <summary>
	/// リスタート
	/// </summary>
	void Restart();

	/// <summary>
	/// 再スタート
	/// </summary>
	void Pause();

	/// <summary>
	/// 強制終了
	/// </summary>
	void Stop();

	/// <summary>
	/// イージングしたVector3を取得
	/// </summary>
	/// <returns>イージングしたVector3</returns>
	const Vector3& Get() const {
		return current_;
	}

	void Debug(const std::string& debugName);

private:
#ifdef _DEBUG
	int32_t easeType_;
	float easeTime_;
#endif // _DEBUG

	std::function<float(float)> ease_;

	Vector3 current_;

	Vector3 start_;
	Vector3 end_;

	bool isActive_;
	bool isLoop_;

	float t_;

	float spdT_;

/// <summary>
///	Easing関数 参照 : https://easings.net/ja
/// </summary>
public:
	static float InSine(float t);
	static float OutSine(float t);
	static float InOutSine(float t);

	static float InQuad(float t);
	static float OutQuad(float t);
	static float InOutQuad(float t);

	static float InCubic(float t);
	static float OutCubic(float t);
	static float InOutCubic(float t);
	
	static float InQuart(float t);
	static float OutQuart(float t);
	static float InOutQuart(float t);

	static float InQuint(float t);
	static float OutQuint(float t);
	static float InOutQuint(float t);

	static float InExpo(float t);
	static float OutExpo(float t);
	static float InOutExpo(float t);

	static float InCirc(float t);
	static float OutCirc(float t);
	static float InOutCirc(float t);

	static float InBack(float t);
	static float OutBack(float t);
	static float InOutBack(float t);

	static float InElastic(float t);
	static float OutElastic(float t);
	static float InOutElastic(float t);

	static float InBounce(float t);
	static float OutBounce(float t);
	static float InOutBounce(float t);
};