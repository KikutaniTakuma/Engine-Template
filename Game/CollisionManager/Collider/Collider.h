#pragma once
#include "Utils/Math/Vector3.h"
#include "Utils/Math/Mat4x4.h"
#include "Drawers/Line/Line.h"
#include "Utils/UtilsLib/UtilsLib.h"
#include <bitset>

class Collider {
public:
	Collider();
	Collider(const Collider&) = default;
	Collider(Collider&&) noexcept = default;
	~Collider() = default;

	Collider& operator=(const Collider&) = default;
	Collider& operator=(Collider&&) noexcept = default;

public:
	void UpdateCollision();

	bool IsCollision(const Vector3& pos);
	void IsCollision(const Collider& other);

	void DebugDraw(const Mat4x4& viewProjection);

	void SetType(uint32_t type);

	bool Filter(const Collider& other) const;

	bool OnEnter() const{
		return flg_.OnEnter();
	}

	bool OnStay() const {
		return flg_.OnStay();
	}

	bool OnExit() const {
		return flg_.OnExit();
	}

	//void Adjusment(Collider& other, const Vector3& moveVec);

public:
	Vector3 scale_;
	Vector3 collisionPos_;

protected:
	Vector3 max_;
	Vector3 min_;

	uint32_t color_;

	std::array<Line, 12> lines_;

	std::bitset<32> types_;

	UtilsLib::Flg flg_;
};