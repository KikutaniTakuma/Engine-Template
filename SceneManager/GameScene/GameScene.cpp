#include "GameScene.h"
#include "MeshManager/MeshManager.h"
#include "TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"

GameScene::GameScene():
	models_(),
	texs_(),
	player_(),
	globalVariables_()
{
}

void GameScene::Initialize(SceneManager* const sceneManager) {
	sceneManager_ = sceneManager;

	meshManager_ = MeshManager::GetInstance();

	audioManager_ = AudioManager::GetInstance();

	textureManager_ = TextureManager::GetInstance();


	globalVariables_.LoadFile();
	player_ = std::make_unique<Player>(&globalVariables_);
	//camera_.pos = { 0.0f,5.0f,-20.0f };

	camera_.farClip = 3000.0f;


	player_->SetCamera(&camera_);

	goal_ = std::make_unique<Goal>();

	skyDome_ = std::make_unique<Model>();
	skyDome_->LoadObj("./Resources/skydome/skydome.obj");
	skyDome_->scale *= 1000.0f;
}

void GameScene::Update() {
	for (auto& model : models_) {
		model.Update();
	}

	for (auto& tex : texs_) {
		tex.Update();
	}

	player_->Debug();
	player_->Update();
}

void GameScene::Draw() {
	//camera_.Update();

	for (auto& model : models_) {
		model.Draw(camera_.GetViewProjection(), camera_.GetPos());
	}

	for (auto& tex : texs_) {
		tex.Draw(camera_.GetViewProjection());
	}

	skyDome_->Draw(camera_.GetViewProjection(), camera_.GetPos());

	player_->Draw();
}