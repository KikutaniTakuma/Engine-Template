#include "MoveFloor.h"
#include "externals/imgui/imgui.h"

MoveFloor::MoveFloor() {
	model_ = std::make_unique<Model>();
	model_->LoadObj("./Resources/Cube.obj");
	ease_.Start(true, 2.0f, Easeing::InOutSine);
	scale_ = { 14.0f,1.0f,14.0f };
	pos_.y = -0.5f;
}

void MoveFloor::Debug(const std::string& debugName) {
	ImGui::Begin(debugName.c_str());
	ImGui::DragFloat3("first", &moveDuration_.first.x, 0.01f);
	ImGui::DragFloat3("second", &moveDuration_.second.x, 0.01f);
	ImGui::DragFloat3("pos", &pos_.x, 0.01f);
	ImGui::DragFloat3("scale", &scale_.x, 0.01f);
	ImGui::End();
}

void MoveFloor::Update() {
	UpdateCollision();

	Vector3 prePos = pos_;
	pos_ = ease_.Get(moveDuration_.first, moveDuration_.second);
	moveVec_ = pos_ - prePos;
	collisionPos_ = pos_;

	model_->pos_ = pos_;
	model_->scale_ = scale_*0.5f;

	ease_.Update();

	model_->Update();
}

void MoveFloor::Draw(const Mat4x4& viewProjection, const Vector3& cameraPos) {
	model_->Draw(viewProjection, cameraPos);

	DebugDraw(viewProjection);
}