#include "GameScene.h"
#include "MeshManager/MeshManager.h"
#include "TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Engine/FrameInfo/FrameInfo.h"
#include <numbers>

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game),
	model_(),
	rotateSpd_(std::numbers::pi_v<float> * 0.5f),
	isOutLine_(true),
	isRotateModel_(false),
	isDrawModel_(true)
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
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	if (isRotateModel_) {
		model_.rotate.y += rotateSpd_ * frameInfo_->GetDelta();
	}
	
	model_.Update();

	if (input_->GetKey()->Pushed(DIK_1)) {
		isOutLine_ = !isOutLine_;
	}
	if (input_->GetKey()->Pushed(DIK_2)) {
		isDrawModel_ = !isDrawModel_;
	}
	if (input_->GetKey()->Pushed(DIK_3)) {
		isRotateModel_= !isRotateModel_;
	}
}

void GameScene::Draw() {
	camera_.Update();
	
	if (isDrawModel_) {
		model_.Draw(camera_.GetViewProjection(), camera_.GetPos());
	}
	if (isOutLine_) {
		pera_.PreDraw();
		model_.Draw(camera_.GetViewProjection(), camera_.GetPos());
		pera_.Draw(Pipeline::Mul);
	}
}