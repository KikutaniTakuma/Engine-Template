#include "GameScene.h"
#include "MeshManager/MeshManager.h"
#include "TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Engine/FrameInfo/FrameInfo.h"
#include <numbers>
#include "externals/imgui/imgui.h"

GameScene::GameScene():
	BaseScene(BaseScene::ID::Game),
	obb_(),
	obb2_()
{}

void GameScene::Initialize() {
	camera_.farClip = 3000.0f;
	camera_.pos.z = -5.0f;
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	
	camera_.Debug("camera");

	obb_.Debug("obb");
	obb_.Update();

	obb2_.Debug("obb2");
	obb2_.Update();

	obb_.IsCollision(obb2_);
	obb2_.IsCollision(obb_);
}

void GameScene::Draw() {
	camera_.Update(Vector3::zero);
	
	obb_.Draw(camera_.GetViewProjection());
	obb2_.Draw(camera_.GetViewProjection());
}