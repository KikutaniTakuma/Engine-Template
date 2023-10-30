#include "GameScene.h"
#include "MeshManager/MeshManager.h"
#include "TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Engine/FrameInfo/FrameInfo.h"

GameScene::GameScene():
	BaseScene(BaseScene::ID::Game),
	model_()
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
	grayScale_.Initialize(
		"./Resources/Shaders/PostShader/Post.VS.hlsl",
		"./Resources/Shaders/PostShader/PostGrayScale.PS.hlsl"
	);
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	model_.Update();
}

void GameScene::Draw() {
	camera_.Update();
	
	pera_.PreDraw();
	model_.Draw(camera_.GetViewProjection(), camera_.GetPos());
	pera_.Draw(Pipeline::None, &grayScale_);
	grayScale_.Draw(Pipeline::None);
}