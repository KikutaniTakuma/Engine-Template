#include "MoveFloor.h"
#include "externals/imgui/imgui.h"

MoveFloor::MoveFloor() {
	model_ = std::make_unique<Model>();
	model_->LoadObj("./Resources/Cube.obj");
	ease_.Start(true, 2.0f, Easeing::InOutSine);
}

void MoveFloor::Debug(const std::string& debugName) {
	ImGui::Begin(debugName.c_str());
	ImGui::DragFloat3("first", &moveDuration_.first.x, 0.01f);
	ImGui::DragFloat3("second", &moveDuration_.second.x, 0.01f);
	ImGui::End();
}

void MoveFloor::Update() {
	UpdateCollision();

	collisionPos_ = ease_.Get(moveDuration_.first, moveDuration_.second);

	model_->pos = collisionPos_;
	model_->scale = scale_;

	ease_.Update();
}

void MoveFloor::Draw(const Mat4x4& viewProjection, const Vector3& cameraPos) {
	model_->Draw(viewProjection, cameraPos);

	DebugDraw(viewProjection);
}