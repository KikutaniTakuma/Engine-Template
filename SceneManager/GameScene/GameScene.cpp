#include "GameScene.h"
#include "MeshManager/MeshManager.h"
#include "TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Engine/FrameInfo/FrameInfo.h"
#include <numbers>

GameScene::GameScene():
	BaseScene(BaseScene::ID::Game),
	model_()/*,
	rotateSpd_(std::numbers::pi_v<float> * 0.5f)*/
{}

void GameScene::Initialize() {
	camera_.farClip = 3000.0f;
	camera_.pos.z = -5.0f;
	camera_.pos.y = 1.1f;
	
	model_.LoadObj("./Resources/Watame/Watame.obj");

	pera_.Initialize(
		"./Resources/Shaders/PostShader/Post.VS.hlsl",
		"./Resources/Shaders/PostShader/PostEdge.PS .hlsl"
	);

	particle_.LoadSettingDirectory("enemy-generation-delete");

	peraCamera_.Update();
	pera_.scale_ = { 1280.0f, 720.0f };
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	//model_.rotate.y += rotateSpd_ * frameInfo_->GetDelta();
	model_.Update();
	camera_.Debug("camera");

	particle_.Debug("particle_");
	particle_.Update();

	pera_.Update();
}

void GameScene::Draw() {
	camera_.Update(Vector3::zero);
	
	model_.Draw(camera_.GetViewProjection(), camera_.GetPos());
	//particle_.Draw(camera_.rotate, camera_.GetViewProjection());
	pera_.PreDraw();
	model_.Draw(camera_.GetViewProjection(), camera_.GetPos());
	pera_.Draw(peraCamera_.GetOthographics(), Pipeline::Mul);
}