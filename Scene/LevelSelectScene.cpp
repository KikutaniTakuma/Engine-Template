#include "LevelSelectScene.h"
#include "AudioManager/AudioManager.h"
#include "externals/imgui/imgui.h"

#include "Input/Gamepad/Gamepad.h"
#include "Input/KeyInput/KeyInput.h"
#include "Input/Mouse/Mouse.h"

#include "GameObject2D/Fade/Fade.h"
#include "Engine/WinApp/WinApp.h"

#include "Game/MapManager/LevelManager.h"
#include "Game/LevelSelect/LevelSelect.h"

#include "GameScene.h"
#include "TitleScene.h"

#include "AudioManager/AudioManager.h"

LevelSelectScene::LevelSelectScene() {
	levelSelecter_ = LevelSelect::GetInstance();
}

LevelSelectScene::~LevelSelectScene() {
}

void LevelSelectScene::OnEnter() {
	// 背景
	background_.Initialize();
	background_.LoadTexture("Resources/background.png");
	background_.scale = WinApp::GetInstance()->GetWindowSize();

	background_.Update();

	Fade::GetInstance()->Start(Vector2::zero, 0x00000000, 10);


	selectSE_ = audioManager_->LoadWav("Resources/Sound/SE/choice.wav", false);

	bgm_ = audioManager_->LoadWav("./Resources/Sound/BGM/title_stageSelect.wav", true);
	if (bgm_) {
		bgm_->Start(0.3f);
	}

	// HUD読み込み
	// 移動
	/*moveButton_.Initialize();
	moveButton_.LoadTexture("Resources/UI/moveKey.png");
	moveButton_.scale = Vector2::identity * 128.f;
	moveButton_.pos = Vector2{ 551.f, 285.f };*/

	/*moveKey_.Initialize();
	moveKey_.LoadTexture("Resources/UI/wasd.png");
	moveKey_.scale = Vector2::identity * 128.f;
	moveKey_.pos = Vector2{ 551.f, 285.f };*/

	// キー移動
	for (int32_t i = 0; i < static_cast<int32_t>(KeyTexture::kCount); ++i) {
		keySprite_[i].Initialize();
		keySprite_[i].LoadTexture("Resources/UI/wasd.png");
		keySprite_[i].scale = Vector2::identity * 128.f;
		keySprite_[i].pos = Vector2{ 551.f, 285.f };
		keySprite_[i].uvSize = Vector2{ 1.f / 4.f, 1.f / 2.f };
		keySprite_[i].uvPibot = Vector2{ i / 4.f,0.f, };
	}

	for (int32_t i = 0; i < static_cast<int32_t>(KeyTexture::kCount); ++i) {
		dpadSprite_[i].Initialize();
		dpadSprite_[i].LoadTexture("Resources/UI/moveKey.png");
		dpadSprite_[i].scale = Vector2::identity * 128.f;
		dpadSprite_[i].pos = Vector2{ 551.f, 285.f };
		dpadSprite_[i].uvSize = Vector2{ 1.f / 4.f, 1.f / 2.f };
		dpadSprite_[i].uvPibot = Vector2{ i / 4.f,0.f, };
	}

	// 戻る
	backButton_.Initialize();
	backButton_.LoadTexture("Resources/UI/goTitle_Button.png");
	backButton_.scale = Vector2::identity * 128.f;
	backButton_.pos = Vector2{ -547.f, 285.f };
	backButton_.uvSize.y = 0.5f;

	backKey_.Initialize();
	backKey_.LoadTexture("Resources/UI/goTitleESC.png");
	backKey_.scale = Vector2::identity * 128.f;
	backKey_.pos = Vector2{ -547.f, 285.f };
	backKey_.uvSize.y = 0.5f;

	// 決定
	buttonSprite_.Initialize();
	buttonSprite_.LoadTexture("Resources/UI/A.png");
	buttonSprite_.scale = Vector2::identity * 128.f;
	buttonSprite_.pos = Vector2{ 384.f, 277.f };
	buttonSprite_.uvSize.y = 0.5f;

	sapceSprite_.Initialize();
	sapceSprite_.LoadTexture("Resources/UI/space.png");
	sapceSprite_.scale = Vector2::identity * 160.f;
	sapceSprite_.pos = Vector2{ 384.f, 297.f };
	sapceSprite_.uvSize.y = 0.5f;
}

void LevelSelectScene::OnExit() {
	if (bgm_) {
		bgm_->Stop();
	}
}

void LevelSelectScene::Update(const float) {
	if (Gamepad::PushAnyKey()) {
		isPad_ = true;
	}
	else if (KeyInput::PushAnyKey() || Mouse::PushAnyKey()) {
		isPad_ = false;
	}
#ifdef  _DEBUG
	/*moveButton_.Debug("moveHud");
	moveKey_.Debug("movehud_Key");
	backButton_.Debug("backHud");
	backKey_.Debug("buckHud_Key");
	buttonSprite_.Debug("choiceHud");
	sapceSprite_.Debug("choiceHud_key");
	ImGui::Begin("isPad");
	ImGui::Checkbox("flg", &isPad_);
	ImGui::End();*/
#endif //  _DEBUG


	sapceSprite_.uvPibot.y = 0.5f * (KeyInput::GetKey(DIK_SPACE) || KeyInput::GetKey(DIK_RETURN));
	buttonSprite_.uvPibot.y = 0.5f * Gamepad::GetButton(Gamepad::Button::A);
	backKey_.uvPibot.y = 0.5f * KeyInput::GetKey(DIK_ESCAPE);
	backButton_.uvPibot.y = 0.5f * Gamepad::GetButton(Gamepad::Button::BACK);

	keySprite_[static_cast<int32_t>(KeyTexture::W)].uvPibot.y = 0.5f * (KeyInput::GetKey(DIK_W) || KeyInput::GetKey(DIK_UP));
	keySprite_[static_cast<int32_t>(KeyTexture::S)].uvPibot.y = 0.5f * (KeyInput::GetKey(DIK_S) || KeyInput::GetKey(DIK_DOWN));
	keySprite_[static_cast<int32_t>(KeyTexture::A)].uvPibot.y = 0.5f * (KeyInput::GetKey(DIK_A) || KeyInput::GetKey(DIK_LEFT));
	keySprite_[static_cast<int32_t>(KeyTexture::D)].uvPibot.y = 0.5f * (KeyInput::GetKey(DIK_D) || KeyInput::GetKey(DIK_RIGHT));

	dpadSprite_[static_cast<int32_t>(KeyTexture::W)].uvPibot.y = 0.5f * (Gamepad::GetButton(Gamepad::Button::UP)
		|| KeyInput::GetKey(DIK_UP) || KeyInput::GetKey(DIK_W)
		|| (Gamepad::GetStick(Gamepad::Stick::LEFT_Y) > 0.5f));
	dpadSprite_[static_cast<int32_t>(KeyTexture::S)].uvPibot.y = 0.5f * (Gamepad::GetButton(Gamepad::Button::DOWN)
		|| KeyInput::GetKey(DIK_DOWN) || KeyInput::GetKey(DIK_S)
		|| (Gamepad::GetStick(Gamepad::Stick::LEFT_Y) < -0.5f));
	dpadSprite_[static_cast<int32_t>(KeyTexture::D)].uvPibot.y = 0.5f * (Gamepad::GetButton(Gamepad::Button::RIGHT)
		|| KeyInput::GetKey(DIK_RIGHT) || KeyInput::GetKey(DIK_D)
		|| (Gamepad::GetStick(Gamepad::Stick::LEFT_X) > 0.5f));
	dpadSprite_[static_cast<int32_t>(KeyTexture::A)].uvPibot.y = 0.5f * (Gamepad::GetButton(Gamepad::Button::LEFT)
		|| KeyInput::GetKey(DIK_LEFT) || KeyInput::GetKey(DIK_A)
		|| (Gamepad::GetStick(Gamepad::Stick::LEFT_X) < -0.5f));

	for (auto &key : keySprite_) {
		key.Update();
	}

	for (auto& dpad : dpadSprite_) {
		dpad.Update();
	}

	backButton_.Update();
	backKey_.Update();
	buttonSprite_.Update();
	sapceSprite_.Update();



	if (Gamepad::Pushed(Gamepad::Button::BACK) || KeyInput::Pushed(DIK_ESCAPE)) {
		sceneManager_->ChangeScene(Vector2::zero, 0x000000FF, new TitleScene, 20);
	}

	auto *const levelManager = LevelManager::GetInstance();
	camera_.Update();

	levelSelecter_->Update();

	if (!Fade::GetInstance()->GetTimer()->IsActive()) {
		if (Gamepad::Pushed(Gamepad::Button::A) ||
			KeyInput::Pushed(DIK_SPACE) || KeyInput::Pushed(DIK_RETURN)) {
			selectSE_->Start(1.f);
			const int32_t targetLevel = levelSelecter_->GetTargetLevel();
			sceneManager_->ChangeScene(Vector2::zero, 0x000000FF, new GameScene{ levelManager->GetLevelName(targetLevel).c_str() }, 20);
		}
	}
}

void LevelSelectScene::Draw() {
	const Mat4x4 &cameraMat = camera_.GetViewOthographics();

	background_.Draw(cameraMat);
	levelSelecter_->Draw(cameraMat);

	if (isPad_) {
		backButton_.Draw(cameraMat, Pipeline::Normal);
		buttonSprite_.Draw(cameraMat, Pipeline::Normal);

		for (int32_t i = 0; i < static_cast<int32_t>(KeyTexture::kCount); ++i) {
			dpadSprite_[i].Draw(cameraMat, Pipeline::Normal);
		}
	}
	else {
		backKey_.Draw(cameraMat, Pipeline::Normal);
		sapceSprite_.Draw(cameraMat, Pipeline::Normal);


		for (int32_t i = 0; i < static_cast<int32_t>(KeyTexture::kCount); ++i) {
			keySprite_[i].Draw(cameraMat, Pipeline::Normal);
		}
	}

	Fade::GetInstance()->Draw();
}
