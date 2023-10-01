#include "LevelResult.h"
#include "Game/LevelSelect/LevelSelect.h"
#include <Utils/Camera/Camera.h>
#include "Scene/SceneManager.h"

#include <Scene/GameScene.h>
#include <Scene/LevelSelectScene.h>
#include "Game/MapManager/LevelManager.h"

#include "Input/Gamepad/Gamepad.h"
#include "Input/KeyInput/KeyInput.h"
#include "Input/Mouse/Mouse.h"
#include <Utils/SoLib/SoLib_ImGui.h>

#include "GameObject2D/Fade/Fade.h"

#include "AudioManager/AudioManager.h"

LevelResult::LevelResult() {
	levelSelecter_ = LevelSelect::GetInstance();
}

void LevelResult::Init() {

	Camera camera{ Camera::Type::Othographic };
	camera.Update();

	selectButton_ = static_cast<int32_t>(ButtonID::NextLevel);

	// ステージセレクト
	buttonFunc_[static_cast<int32_t>(ButtonID::LevelSelect)] = []() {SceneManager::GetInstance()->ChangeScene(Vector2::zero, 0xFF, new LevelSelectScene, 30); };

	// リトライ
	buttonFunc_[static_cast<int32_t>(ButtonID::Retry)] = []() {SceneManager::GetInstance()->ChangeScene(Vector2::zero, 0xFF, new GameScene{ LevelManager::GetInstance()->GetLevelName(LevelSelect::GetInstance()->GetTargetLevel()).c_str() }, 15); };
	// 次のステージへ
	buttonFunc_[static_cast<int32_t>(ButtonID::NextLevel)] = []()
		{
			LevelSelect::GetInstance()->AddTargetLevel(1u);
			SceneManager::GetInstance()->ChangeScene(Vector2::zero, 0xFF, new GameScene{ LevelManager::GetInstance()->GetLevelName(LevelSelect::GetInstance()->GetTargetLevel()).c_str() }, 15);
		};

	cameraMat_ = camera.GetViewOthographics();

	background_.Initialize();
	background_.LoadTexture("Resources/UI/resultBuckground.png");
	background_.scale = Vector2{ 896.f,256.f };
	background_.pos = origin_;

	for (auto &button : buttonSprite_) {
		button = std::make_unique<Texture2D>();
		button->Initialize();
	}
	buttonSprite_[static_cast<int32_t>(ButtonID::LevelSelect)]->LoadTexture("Resources/UI/resultStageSelect.png");
	buttonSprite_[static_cast<int32_t>(ButtonID::Retry)]->LoadTexture("Resources/UI/resultRetry.png");
	buttonSprite_[static_cast<int32_t>(ButtonID::NextLevel)]->LoadTexture("Resources/UI/resultNext.png");

	for (int32_t i = 0; i < kButtonCount_; ++i) {
		buttonSprite_[i]->scale = buttonSprite_[i]->GetTexSize();
		buttonSprite_[i]->scale.x *= 0.5f;
		buttonSprite_[i]->uvSize.x = 0.5f;

		float offsetX = i - (kButtonCount_ - 1.f) / 2.f;
		buttonSprite_[i]->pos = origin_ + Vector2{ offsetX * posRadius_ ,0.f };
	}

	cursorMoveSe_ = AudioManager::GetInstance()->LoadWav("./Resources/Sound/SE/walk.wav", false);
	choiseSe_ = AudioManager::GetInstance()->LoadWav("./Resources/Sound/SE/choice.wav", false);
}

void LevelResult::Update() {
	background_.Update();

	SoLib::ImGuiWidget("origin", &origin_);
	SoLib::ImGuiWidget("buttonPos", &posRadius_);

	if (-0.5f <= Gamepad::GetStick(Gamepad::Stick::LEFT_X) && Gamepad::GetStick(Gamepad::Stick::LEFT_X) <= 0.5f
		&& -0.5f <= Gamepad::GetStick(Gamepad::Stick::LEFT_Y) && Gamepad::GetStick(Gamepad::Stick::LEFT_Y) <= 0.5f) {
		isStick_ = false;
	}

	if (Gamepad::Pushed(Gamepad::Button::LEFT) || (Gamepad::GetStick(Gamepad::Stick::LEFT_X) < -0.5f && !isStick_)
		|| KeyInput::Pushed(DIK_A) || KeyInput::Pushed(DIK_LEFT)) {
		--selectButton_;
		cursorMoveSe_->Start(0.4f);
		isStick_ = true;
	}
	if (Gamepad::Pushed(Gamepad::Button::RIGHT) || (Gamepad::GetStick(Gamepad::Stick::LEFT_X) > 0.5f && !isStick_)
		|| KeyInput::Pushed(DIK_D) || KeyInput::Pushed(DIK_RIGHT)) {
		++selectButton_;
		cursorMoveSe_->Start(0.4f);
		isStick_ = true;
	}
	selectButton_ = std::clamp(selectButton_, 0, drawCount_ - 1);


	background_.pos = origin_;

	for (int32_t i = 0; i < drawCount_; ++i) {

		float offsetX = (i - (drawCount_ - 1.f) / 2.f) * (static_cast<float>(kButtonCount_) / drawCount_);
		buttonSprite_[i]->pos = origin_ + Vector2{ offsetX * posRadius_ ,0.f };
		bool isSelect = selectButton_ == i;
		buttonSprite_[i]->uvPibot.x = 0.5f * isSelect;

		buttonSprite_[i]->Update();
	}

	if (!isChoice_ &&
		(Gamepad::Pushed(Gamepad::Button::A)
		|| KeyInput::Pushed(DIK_SPACE) || KeyInput::Pushed(DIK_RETURN))) {
		Fade::GetInstance()->SetEaseFunc(SoLib::easeOutQuad);
		buttonFunc_[selectButton_]();
		choiseSe_->Start(1.0f);
		isChoice_ = true;
	}

}

void LevelResult::Draw() {
	background_.Draw(cameraMat_, Pipeline::Blend::Normal);
	for (int32_t i = 0; i < drawCount_; ++i) {
		buttonSprite_[i]->Draw(cameraMat_, Pipeline::Blend::Normal);
	}
}
