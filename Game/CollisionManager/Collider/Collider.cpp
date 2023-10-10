#include "Collider.h"

Collider::Collider() :
	scale_(Vector3::identity),
	pos_(Vector3::zero),
	max_(Vector3::identity * 0.5f),
	min_(Vector3::identity * -0.5f)
{
	color_ = Vector4ToUint(Vector4::identity);
}

void Collider::Update() {
	max_ = Vector3::identity * 0.5f;
	min_ = Vector3::identity * -0.5f;

	max_ *= HoriMakeMatrixAffin(scale_, Vector3::zero, pos_);
	min_ *= HoriMakeMatrixAffin(scale_, Vector3::zero, pos_);
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
	
	return false;
}

bool Collider::IsCollision(const Collider& other) {
	std::array<Vector3, 8> postions = {
		Vector3(other.min_), // 左下手前
		Vector3(other.min_.x, other.min_.y, other.max_.z), // 左下奥
		Vector3(other.max_.x, other.min_.y, other.min_.z), // 右下手前
		Vector3(other.max_.x, other.min_.y, other.max_.z), // 右下奥

		Vector3(other.min_.x, other.max_.y, other.min_.z), // 左上手前
		Vector3(other.min_.x, other.max_.y, other.max_.z), // 左上奥
		Vector3(other.max_.x, other.max_.y, other.min_.z), // 右上手前
		Vector3(other.max_) // 右上奥
	};

	for (auto& pos : postions) {
		if (IsCollision(pos)) {
			return true;
		}
	}

	return false;
}

void Collider::DebugDraw(const Mat4x4& viewProjection) {
	std::array<Vector3, 8> postions = {
		Vector3(min_), // 左下手前
		Vector3(min_.x, min_.y, max_.z), // 左下奥
		Vector3(max_.x, min_.y, min_.z), // 右下手前
		Vector3(max_.x, min_.y, max_.z), // 右下奥

		Vector3(min_.x, max_.y, min_.z), // 左上手前
		Vector3(min_.x, max_.y, max_.z), // 左上奥
		Vector3(max_.x, max_.y, min_.z), // 右上手前
		Vector3(max_) // 右上奥
	};

	lines_[0].start = postions[0];
	lines_[0].end = postions[1];

	lines_[1].start = postions[0];
	lines_[1].end = postions[2];

	lines_[2].start = postions[0];
	lines_[2].end = postions[4];


	lines_[3].start = postions[3];
	lines_[3].end = postions[1];

	lines_[4].start = postions[3];
	lines_[4].end = postions[2];

	lines_[5].start = postions[3];
	lines_[5].end = postions[7];


	lines_[6].start = postions[5];
	lines_[6].end = postions[4];

	lines_[7].start = postions[5];
	lines_[7].end = postions[7];

	lines_[8].start = postions[5];
	lines_[8].end = postions[1];


	lines_[9].start = postions[6];
	lines_[9].end = postions[4];

	lines_[10].start = postions[6];
	lines_[10].end = postions[7];

	lines_[11].start = postions[6];
	lines_[11].end = postions[2];

	for (auto& line : lines_) {
		line.Draw(viewProjection, color_);
	}
}