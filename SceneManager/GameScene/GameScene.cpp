#include "GameScene.h"
#include "MeshManager/MeshManager.h"
#include "TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Engine/FrameInfo/FrameInfo.h"
#include <numbers>

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game),
	model_(),
	tex2D_(),
	texture_(nullptr)
{}

void GameScene::Initialize() {
	camera_.farClip = 3000.0f;
	camera_.pos.z = -20.0f;
	camera_.pos.y = 1.1f;
	
	// obj�t�@�C���ǂݍ���
	model_.LoadObj("./Resources/Rabbit/Rabbit.obj");
	model_.rotate.y = std::numbers::pi_v<float>;

	// �e�N�X�`���ǂݍ���
	tex2D_.LoadTexture("./Resources/uvChecker.png");
	tex2D_.pos = Vector2{ 380.0f,  -80.0f };
	tex2D_.isSameTexSize = true;
	tex2D_.texScalar = 0.5f;

	// �e�N�X�`���P�̂ł�ǂݍ��ݏo����
	texture_ =
		textureManager_->LoadTexture("./Resources/Rabbit/Rabbit_face.png");
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	model_.Debug("model");
	model_.Update();

	if (input_->GetKey()->Pushed(DIK_1)) {
		// ロード済みのテクスチャに変更することも可能
		model_.ChangeTexture("face", texture_);
	}
	if (input_->GetKey()->Pushed(DIK_2)) {
		// ロードしてなくてもここでパスを指定すれば読み込んで変更できる
		model_.ChangeTexture("face", "./Resources/Rabbit/Rabbit_face_happy.png");
	}
	
	tex2D_.Debug("tex");
	tex2D_.Update();
}

void GameScene::Draw() {
	camera_.Update(Vector3::zero);
	
	// 投資投影で描画
	model_.Draw(camera_.GetViewProjection(), camera_.GetPos());
	
	// 平行投影で描画
	tex2D_.Draw(camera_.GetOthographics(), Pipeline::Normal, true);
}