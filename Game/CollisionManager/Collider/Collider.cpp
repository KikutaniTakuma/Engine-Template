#include "Collider.h"

Collider::Collider() :
	scale_(Vector3::identity),
	collisionPos_(Vector3::zero),
	max_(Vector3::identity * 0.5f),
	min_(Vector3::identity * -0.5f)
{
	color_ = Vector4ToUint(Vector4::identity);
}

void Collider::UpdateCollision() {
	flg_.Update();
	max_ = Vector3::identity * 0.5f;
	min_ = Vector3::identity * -0.5f;

	max_ *= HoriMakeMatrixAffin(scale_, Vector3::zero, collisionPos_);
	min_ *= HoriMakeMatrixAffin(scale_, Vector3::zero, collisionPos_);
}


bool Collider::IsCollision(const Vector3& pos) {
	if (min_.x < pos.x && pos.x < max_.x) {
		if (min_.y < pos.y && pos.y < max_.y) {
			if (min_.z < pos.z && pos.z < max_.z) {
				color_ = Vector4ToUint(Vector4::xIdy);
				return true;
			}
		}
	}
	color_ = Vector4ToUint(Vector4::identity);
	return false;
}

void Collider::IsCollision(const Collider* other) {
	std::array<Vector3, 8> positions = {
		Vector3(other->min_), // 左下手前
		Vector3(other->min_.x, other->min_.y, other->max_.z), // 左下奥
		Vector3(other->max_.x, other->min_.y, other->min_.z), // 右下手前
		Vector3(other->max_.x, other->min_.y, other->max_.z), // 右下奥

		Vector3(other->min_.x, other->max_.y, other->min_.z), // 左上手前
		Vector3(other->min_.x, other->max_.y, other->max_.z), // 左上奥
		Vector3(other->max_.x, other->max_.y, other->min_.z), // 右上手前
		Vector3(other->max_) // 右上奥
	};

	for (auto& pos : positions) {
		if (IsCollision(pos)) {
			flg_ = true;
			return;
		}
		else {
			flg_ = false;
		}
	}

	color_ = Vector4ToUint(Vector4::identity);
}

void Collider::DebugDraw(const Mat4x4& viewProjection) {
	std::array<Vector3, 8> positions = {
		Vector3(min_), // 左下手前
		Vector3(min_.x, min_.y, max_.z), // 左下奥
		Vector3(max_.x, min_.y, min_.z), // 右下手前
		Vector3(max_.x, min_.y, max_.z), // 右下奥

		Vector3(min_.x, max_.y, min_.z), // 左上手前
		Vector3(min_.x, max_.y, max_.z), // 左上奥
		Vector3(max_.x, max_.y, min_.z), // 右上手前
		Vector3(max_) // 右上奥
	};

	lines_[0].start = positions[0];
	lines_[0].end = positions[1];

	lines_[1].start = positions[0];
	lines_[1].end = positions[2];

	lines_[2].start = positions[0];
	lines_[2].end = positions[4];


	lines_[3].start = positions[3];
	lines_[3].end = positions[1];

	lines_[4].start = positions[3];
	lines_[4].end = positions[2];

	lines_[5].start = positions[3];
	lines_[5].end = positions[7];


	lines_[6].start = positions[5];
	lines_[6].end = positions[4];

	lines_[7].start = positions[5];
	lines_[7].end = positions[7];

	lines_[8].start = positions[5];
	lines_[8].end = positions[1];


	lines_[9].start = positions[6];
	lines_[9].end = positions[4];

	lines_[10].start = positions[6];
	lines_[10].end = positions[7];

	lines_[11].start = positions[6];
	lines_[11].end = positions[2];

	for (auto& line : lines_) {
		line.Draw(viewProjection, color_);
	}
}

void Collider::SetType(uint32_t type) {
	types_ = std::bitset<32>(type);
}

bool Collider::Filter(const Collider& other) const {
	return types_ != (types_ & ~other.types_);
}

//void Collider::Adjusment(Collider& other, const Vector3& moveVec) {
//	std::array<Vector3, 8> otherPositions = {
//		Vector3(other.min_), // 左下手前
//		Vector3(other.min_.x, other.min_.y, other.max_.z), // 左下奥
//		Vector3(other.max_.x, other.min_.y, other.min_.z), // 右下手前
//		Vector3(other.max_.x, other.min_.y, other.max_.z), // 右下奥
//
//		Vector3(other.min_.x, other.max_.y, other.min_.z), // 左上手前
//		Vector3(other.min_.x, other.max_.y, other.max_.z), // 左上奥
//		Vector3(other.max_.x, other.max_.y, other.min_.z), // 右上手前
//		Vector3(other.max_) // 右上奥
//	};
//
//	std::array<Vector3, 8> positions = {
//		Vector3(min_), // 左下手前
//		Vector3(min_.x, min_.y, max_.z), // 左下奥
//		Vector3(max_.x, min_.y, min_.z), // 右下手前
//		Vector3(max_.x, min_.y, max_.z), // 右下奥
//
//		Vector3(min_.x, max_.y, min_.z), // 左上手前
//		Vector3(min_.x, max_.y, max_.z), // 左上奥
//		Vector3(max_.x, max_.y, min_.z), // 右上手前
//		Vector3(max_) // 右上奥
//	};
//
//	Vector3 collisionPos;
//
//	for (auto& pos : otherPositions) {
//		if (IsCollision(pos)) {
//			flg_.flg_ = true;
//			collisionPos = pos;
//			break;
//		}
//	}
//
//	Vector3 mostNear;
//
//	if (flg_.OnEnter()) {
//		for (auto& pos : positions) {
//			if ((collisionPos - pos).Length() < (collisionPos - mostNear).Length()) {
//				mostNear = pos;
//			}
//			else if(mostNear == Vector3::zero) {
//				mostNear = pos;
//			}
//		}
//	}
//
//	if (moveVec.x < 0.0f) {
//		other.pos_.x += std::abs((mostNear - collisionPos).x);
//	}
//	else if (moveVec.x > 0.0f) {
//		other.pos_.x -= std::abs((mostNear - collisionPos).x);
//	}
//
//	if (moveVec.y < 0.0f) {
//		other.pos_.y += std::abs((mostNear - collisionPos).y);
//	}
//	else if (moveVec.y > 0.0f) {
//		other.pos_.y -= std::abs((mostNear - collisionPos).y);
//	}
//
//	if (moveVec.z < 0.0f) {
//		other.pos_.z += std::abs((mostNear - collisionPos).z);
//	}
//	else if (moveVec.z > 0.0f) {
//		other.pos_.z -= std::abs((mostNear - collisionPos).z);
//	}
//}