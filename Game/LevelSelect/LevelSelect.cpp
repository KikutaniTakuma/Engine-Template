#include "LevelSelect.h"

#include <algorithm>

#include "GameObject2D/Fade/Fade.h"

#include "Input/Gamepad/Gamepad.h"
#include "Input/KeyInput/KeyInput.h"
#include "Input/Mouse/Mouse.h"

#include "Utils/SoLib/SoLib_ImGui.h"

#include "AudioManager/AudioManager.h"

#include "Game/MapManager/LevelManager.h"

void LevelSelect::Init() {

	levelTitle_.Initialize();
	levelTitle_.LoadTexture("Resources/UI/stageLabel.png");
	levelTitle_.pos = Vector2{ 0.f,260.f };
	levelTitle_.scale = Vector2{ 256.f,128 } *1.5f;

	levelTitle_.uvSize.x = 1.f / kLevelCount_;


	for (uint32_t i = 0u; i < kLevelCount_; ++i) {
		levelIcon_[i].Initialize();
		levelIcon_[i].LoadTexture("Resources/UI/stageIcon.png");
		levelIcon_[i].uvPibot.x = 1.f / kLevelCount_ * i;
		levelIcon_[i].uvSize = Vector2{ 1.f / kLevelCount_, 0.5f };
		levelIcon_[i].scale = Vector2::identity * defaultIconScale_;
	}

	auto *const audioManager = AudioManager::GetInstance();
	moveSE_ = audioManager->LoadWav("Resources/Sound/SE/walk.wav", false);

	texMaxSize_ = Vector2::identity * defaultIconScale_ * 1.05f;
	texMinSize_ = Vector2::identity * defaultIconScale_ * 1.0f;

	isMinas_ = false;
	easeT = 0.0f;
	easeSpd = 5.0f;

	for (size_t i = 0; i < clearIcon_.size(); i++) {
		clearIcon_[i].Initialize();
		clearIcon_[i].LoadTexture("Resources/UI/heart.png");
		clearIcon_[i].scale = Vector2::identity * defaultIconScale_ * 0.6f;
	}
}

void LevelSelect::Update() {

	SoLib::ImGuiWidget("iconOrigin", &iconOrigin_);
	SoLib::ImGuiWidget("iconRange", &iconRange_);
	SoLib::ImGuiWidget("levelTitle", &levelTitle_.pos);

	// フェードが動いていない場合は実行
	if (!Fade::GetInstance()->GetTimer()->IsActive()) {

#pragma region カーソル移動

		if (-0.5f <= Gamepad::GetStick(Gamepad::Stick::LEFT_X) && Gamepad::GetStick(Gamepad::Stick::LEFT_X) <= 0.5f
			&& -0.5f <= Gamepad::GetStick(Gamepad::Stick::LEFT_Y) && Gamepad::GetStick(Gamepad::Stick::LEFT_Y) <= 0.5f) {
			isStick_ = false;
		}

		bool isMoveing = false;

		if (Gamepad::Pushed(Gamepad::Button::UP)
			|| KeyInput::Pushed(DIK_UP) || KeyInput::Pushed(DIK_W)
			|| (Gamepad::GetStick(Gamepad::Stick::LEFT_Y) > 0.5f && !isStick_)
			) {
			selectTarget_ -= kWidthCount_;
			isMoveing = true;
			isStick_ = true;
		}
		if (Gamepad::Pushed(Gamepad::Button::DOWN)
			|| KeyInput::Pushed(DIK_DOWN) || KeyInput::Pushed(DIK_S)
			|| (Gamepad::GetStick(Gamepad::Stick::LEFT_Y) < -0.5f && !isStick_)
			) {
			selectTarget_ += kWidthCount_;
			isMoveing = true;
			isStick_ = true;
		}

		if (Gamepad::Pushed(Gamepad::Button::LEFT)
			|| KeyInput::Pushed(DIK_LEFT) || KeyInput::Pushed(DIK_A)
			|| (Gamepad::GetStick(Gamepad::Stick::LEFT_X) < -0.5f && !isStick_)
			) {
			selectTarget_ += -1;
			isMoveing = true;
			isStick_ = true;
		}
		if (Gamepad::Pushed(Gamepad::Button::RIGHT)
			|| KeyInput::Pushed(DIK_RIGHT) || KeyInput::Pushed(DIK_D)
			|| (Gamepad::GetStick(Gamepad::Stick::LEFT_X) > 0.5f && !isStick_)
			) {
			selectTarget_ += +1;
			isMoveing = true;
			isStick_ = true;
		}

		selectTarget_ = std::clamp(selectTarget_, 0, kLevelCount_ - 1);

		if (isMoveing) {
			moveSE_->Start(1.f);
		}

#pragma endregion

	}

	// 座標の計算
	const float centorX = (kWidthCount_ - 1.f) / 2.f;
	const float centorY = (std::ceil(1.f * kLevelCount_ / kWidthCount_) - 1.f) / 2.f;
	for (uint32_t i = 0u; i < kLevelCount_; ++i) {
		levelAnchor_[i].x = ((i % kWidthCount_) - centorX) * iconRange_.x;
		levelAnchor_[i].y = (std::floor(i * 1.f / kWidthCount_) - centorY) * -iconRange_.y;
	}

	for (int32_t i = 0; i < kLevelCount_; ++i) {
		bool isSelected = (i == selectTarget_);
		levelIcon_[i].uvPibot.y = isSelected * 0.5f;
	}

	levelTitle_.uvPibot.x = selectTarget_ * 1.f / kLevelCount_;
	levelTitle_.Update();

	for (uint32_t i = 0u; i < kLevelCount_; ++i) {
		levelIcon_[i].pos = levelAnchor_[i] + iconOrigin_;
		levelIcon_[i].Update();
	}


	// 演出
	currentTexSize = Vector2::Lerp(texMinSize_, texMaxSize_, SoLib::easeInExpo(easeT));
	if (!isMinas_) {
		easeT += easeSpd * ImGui::GetIO().DeltaTime;
	}
	else {
		easeT -= easeSpd * ImGui::GetIO().DeltaTime;
	}

	if (easeT >= 1.0f) {
		easeT = 1.0f;
		isMinas_ = true;
	}
	else if(easeT <= 0.0f) {
		easeT = 0.0f;
		isMinas_ = false;
	}

	for (auto& i : levelIcon_) {
		i.scale = currentTexSize;
	}

	for (size_t i = 0; i < clearIcon_.size(); i++) {
		clearIcon_[i].pos = levelAnchor_[i] + (texMinSize_ * 0.4f) + iconOrigin_;
		clearIcon_[i].scale = currentTexSize * 0.6f;
		clearIcon_[i].Update();
	}
}

void LevelSelect::Draw(const Mat4x4 &viewProjection) {

	for (auto &icon : levelIcon_) {
		icon.Draw(viewProjection, Pipeline::Blend::Normal);
	}
	levelTitle_.Draw(viewProjection, Pipeline::Blend::Normal);

	for (size_t i = 0; i < clearIcon_.size(); i++) {
		if (LevelManager::GetInstance()->GetLevelList()[i]->isClear_) {
			clearIcon_[i].Draw(viewProjection, Pipeline::Blend::Normal);
		}
	}
}

void LevelSelect::AddTargetLevel(int32_t value) {
	selectTarget_ = std::clamp(selectTarget_ + value, 0, kLevelCount_ - 1);
}
