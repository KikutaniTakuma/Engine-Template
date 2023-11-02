#include "GameScene.h"
#include "MeshManager/MeshManager.h"
#include "TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Engine/FrameInfo/FrameInfo.h"
#include <numbers>
#include "externals/imgui/imgui.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game),
	obb_()
{}

void GameScene::Initialize() {
	camera_.farClip = 3000.0f;
	camera_.pos.z = -20.0f;
	camera_.pos.y = 1.1f;
	
	// objファイル読み込み
	model_.LoadObj("./Resources/Rabbit/Rabbit.obj");
	model_.rotate.y = std::numbers::pi_v<float>;

	// テクスチャ読み込み
	tex2D_.LoadTexture("./Resources/uvChecker.png");
	tex2D_.pos = Vector2{ 380.0f,  -80.0f };
	tex2D_.isSameTexSize = true;
	tex2D_.texScalar = 0.5f;

	// テクスチャ単体でも読み込み出来る
	texture_ =
		textureManager_->LoadTexture("./Resources/Rabbit/Rabbit_face.png");
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