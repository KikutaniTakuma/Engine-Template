#include "Obb.h"
#include <numbers>
#include "externals/imgui/imgui.h"
#include "Game/CollisionManager/Plane/Plane.h"

Obb::Obb():
	center_(),
	scale_(Vector3::identity),
	rotate_(),
	worldMatrix_(),
	color_(std::numeric_limits<uint32_t>::max()),
	orientations_{
		Vector3::xIdy,
		Vector3::yIdy,
		Vector3::zIdy
	},
	lines_{},
	orientationLines_{}
{}

bool Obb::IsCollision(Vector3 pos, float radius) {
	std::array<Vector3, 3> orientations = {
		orientations_[0] * MakeMatrixRotate(rotate_),
		orientations_[1] * MakeMatrixRotate(rotate_),
		orientations_[2] * MakeMatrixRotate(rotate_)
	};

	std::array<Vector3, 2> positions = {
		Vector3(-size_), // 左下手前
		
		Vector3(+size_) // 右上奥
	};

	for (auto& position : positions) {
		position *= MakeMatrixScalar(scale_);
	}

	pos *= MakeMatrixInverse(MakeMatrixAffin(Vector3::identity, rotate_, center_));

	Vector3 closestPoint = {
		std::clamp(pos.x, positions[0].x,positions[1].x),
		std::clamp(pos.y, positions[0].y,positions[1].y),
		std::clamp(pos.z, positions[0].z,positions[1].z)
	};

	float distance = (closestPoint - pos).Length();

	if (distance <= radius) {
		isCollision_ = true;
		color_ = 0xff0000ff;
		return true;
	}
	else {
		isCollision_ = false;
		color_ = std::numeric_limits<uint32_t>::max();
		return false;
	}
}

bool Obb::IsCollision(Obb& other) {
	std::array<Vector3, 8> positions = {
		Vector3(-size_), // 左下手前
		Vector3(-size_.x, -size_.y, +size_.z), // 左下奥
		Vector3(+size_.x, -size_.y, -size_.z), // 右下手前
		Vector3(+size_.x, -size_.y, +size_.z), // 右下奥

		Vector3(-size_.x, +size_.y, -size_.z), // 左上手前
		Vector3(-size_.x, +size_.y, +size_.z), // 左上奥
		Vector3(+size_.x, +size_.y, -size_.z), // 右上手前
		Vector3(+size_) // 右上奥
	};

	Mat4x4 worldmat = MakeMatrixAffin(scale_, rotate_, center_);
	for (auto& pos : positions) {
		pos *= worldmat;
	}

	std::array<Vector3, 8> otherPositions = {
		Vector3(-size_), // 左下手前
		Vector3(-size_.x, -size_.y, +size_.z), // 左下奥
		Vector3(+size_.x, -size_.y, -size_.z), // 右下手前
		Vector3(+size_.x, -size_.y, +size_.z), // 右下奥

		Vector3(-size_.x, +size_.y, -size_.z), // 左上手前
		Vector3(-size_.x, +size_.y, +size_.z), // 左上奥
		Vector3(+size_.x, +size_.y, -size_.z), // 右上手前
		Vector3(+size_) // 右上奥
	};

	Mat4x4 otherWorldMat = MakeMatrixAffin(other.scale_, other.rotate_, other.center_)
		* MakeMatrixInverse(MakeMatrixAffin(Vector3::identity, rotate_, center_));
	for (auto& pos : otherPositions) {
		pos *= otherWorldMat;
	}


	// 分離軸(面法線)
	std::array<Vector3, 3> orientations = {
		orientations_[0] * MakeMatrixRotate(rotate_),
		orientations_[1] * MakeMatrixRotate(rotate_),
		orientations_[2] * MakeMatrixRotate(rotate_)
	};
	Mat4x4 inverceObbMat = MakeMatrixInverse(MakeMatrixRotate(rotate_));
	std::array<Vector3, 3> otherOrientations = {
		other.orientations_[0] * MakeMatrixRotate(other.rotate_) * inverceObbMat,
		other.orientations_[1] * MakeMatrixRotate(other.rotate_) * inverceObbMat,
		other.orientations_[2] * MakeMatrixRotate(other.rotate_) * inverceObbMat
	};

	// 分離軸に射影
	std::array<Vector3, 8> projPos;
	for (size_t i = 0; i < projPos.size(); i++) {
		projPos[i] = Project(positions[i], orientations[0]);
	}
	



	// 分離軸(各辺の組み合わせ)
	std::array<Vector3, 9> line;
	{
		auto lineItr = line.begin();
		for (size_t i = 0; i < orientations.size(); i++) {
			for (size_t j = 0; j < otherOrientations.size(); j++) {
				*lineItr = orientations[i].Cross(otherOrientations[j]);
				lineItr++;
			}
		}
	}



	return false;
}

void Obb::Update() {
	isCollision_.Update();

	worldMatrix_.Affin(scale_, rotate_, center_);

	size_ = scale_ * 0.5f;

	std::array<Vector3, 8> positions = {
		Vector3(-size_), // 左下手前
		Vector3(-size_.x, -size_.y, +size_.z), // 左下奥
		Vector3(+size_.x, -size_.y, -size_.z), // 右下手前
		Vector3(+size_.x, -size_.y, +size_.z), // 右下奥

		Vector3(-size_.x, +size_.y, -size_.z), // 左上手前
		Vector3(-size_.x, +size_.y, +size_.z), // 左上奥
		Vector3(+size_.x, +size_.y, -size_.z), // 右上手前
		Vector3(+size_) // 右上奥
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

	for (size_t i = 0llu; i < orientationLines_.size(); i++) {
		orientationLines_[i].start = center_;
		orientationLines_[i].end = (orientations_[i] * size_[i]) * worldMatrix_;
	}

	for (auto& line : lines_) {
		line.start *= worldMatrix_;
		line.end *= worldMatrix_;
	}
}

void Obb::Draw(const Mat4x4& viewProjection) {
	for (auto& line : lines_) {
		line.Draw(viewProjection, color_);
	}

	for (size_t i = 0llu; i < orientationLines_.size(); i++) {
		orientationLines_[i].Draw(viewProjection, Vector4ToUint(Vector4{ orientations_[i], 1.0f}));
	}
}

void Obb::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat3("center", &center_.x, 0.01f);
	ImGui::DragFloat3("size", &scale_.x, 0.01f);
	ImGui::DragFloat3("rotate", &rotate_.x, 0.01f);
	static Vector4 colorEdit;
	colorEdit = UintToVector4(color_);
	ImGui::ColorEdit4("color", colorEdit.m.data());
	color_ = Vector4ToUint(colorEdit);
	ImGui::End();
#endif // _DEBUG
}