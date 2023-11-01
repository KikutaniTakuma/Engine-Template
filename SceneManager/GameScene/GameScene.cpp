#include "GameScene.h"
#include "MeshManager/MeshManager.h"
#include "TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Engine/FrameInfo/FrameInfo.h"
#include <numbers>
#include "externals/imgui/imgui.h"

GameScene::GameScene():
	BaseScene(BaseScene::ID::Game),
	obb_()
{}

void GameScene::Initialize() {
	camera_.farClip = 3000.0f;
	camera_.pos.z = -5.0f;
	radius_ = 1.0f;
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	
	camera_.Debug("camera");

	obb_.Debug("obb");
	obb_.Update();

	ImGui::Begin("radius_");
	ImGui::DragFloat("radius", &radius_, 0.01f);
	ImGui::End();

	sphere_.Debug("sphere");
	sphere_.Update();
	sphere_.scale = Vector3{ radius_ ,radius_ ,radius_ } * 0.5f;

	obb_.IsCollision(sphere_.pos, radius_ * 0.5f);
}

void GameScene::Draw() {
	camera_.Update(Vector3::zero);
	
	obb_.Draw(camera_.GetViewProjection());
	sphere_.Draw(camera_.GetViewProjection(), camera_.pos);
}