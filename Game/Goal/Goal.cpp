#include "Goal.h"
#include "externals/imgui/imgui.h"

Goal::Goal() {
	model_ = std::make_unique<Model>();
	model_->LoadObj("./Resources/Cube.obj");
}

void Goal::Update() {
	UpdateCollision();

	model_->pos = collisionPos_;
	model_->scale = scale_;
}

void Goal::Draw(const Mat4x4& viewProjection, const Vector3& cameraPos) {
	model_->Draw(viewProjection, cameraPos);

	DebugDraw(viewProjection);
}

void Goal::Debug(const std::string& debugName) {
	ImGui::Begin(debugName.c_str());
	ImGui::DragFloat("pos", &collisionPos_.x);
	ImGui::DragFloat("scale", &scale_.x);
	ImGui::End();
}