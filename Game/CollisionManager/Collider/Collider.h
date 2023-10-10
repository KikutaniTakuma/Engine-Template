#pragma once
#include "Utils/Math/Vector3.h"
#include "Utils/Math/Mat4x4.h"
#include "Drawers/Line/Line.h"

class Collider {
public:
	Collider();
	Collider(const Collider&) = default;
	Collider(Collider&&) noexcept = default;
	~Collider() = default;

	Collider& operator=(const Collider&) = default;
	Collider& operator=(Collider&&) noexcept = default;

public:
	void Update();

	bool IsCollision(const Vector3& pos);
	bool IsCollision(const Collider& other);

	void DebugDraw(const Mat4x4& viewProjection);

public:
	Vector3 scale_;
	Vector3 pos_;

private:
	Vector3 max_;
	Vector3 min_;

	uint32_t color_;

	std::array<Line, 12> lines_;
};