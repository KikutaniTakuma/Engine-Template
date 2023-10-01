#include "TitleScene.h"
#include "AudioManager/AudioManager.h"
#include "externals/imgui/imgui.h"

#include "Input/Gamepad/Gamepad.h"
#include "Input/KeyInput/KeyInput.h"
#include "Input/Mouse/Mouse.h"

#include "GameObject2D/Fade/Fade.h"
#include "Utils/Math/Angle.h"

#include "GameScene.h"
#include "LevelSelectScene.h"

#include "AudioManager/AudioManager.h"
#include <Utils/SoLib/SoLib_ImGui.h>

TitleScene::TitleScene() {
}

TitleScene::~TitleScene() {
}

void TitleScene::OnEnter() {
	titleSprite_.Initialize();
	titleSprite_.LoadTexture("Resources/UI/title.png");
	Vector2 texSize = titleSprite_.GetTexSize();
	titleSprite_.scale = { texSize.x / 4.f,texSize.y };
	titleSprite_.uvSize.x = 1.f / 4.f;


	buttonSprite_.Initialize();
	buttonSprite_.LoadTexture("Resources/UI/A.png");
	buttonSprite_.scale = Vector2::identity * 128.f;
	buttonSprite_.pos = Vector2{ 0.f, -272.f };
	buttonSprite_.uvSize.y = 0.5f;

	sapceSprite_.Initialize();
	sapceSprite_.LoadTexture("Resources/UI/space.png");
	sapceSprite_.scale = Vector2::identity * 160.f;
	sapceSprite_.pos = Vector2{ 0.f, -260.f };
	sapceSprite_.uvSize.y = 0.5f;

	escSprite_.Initialize();
	escSprite_.LoadTexture("Resources/UI/endESC.png");
	escSprite_.scale = Vector2::identity * 128.f;
	escSprite_.pos = Vector2{ -547.f, -266.f };
	escSprite_.Update();

	backButtonSprite_.Initialize();
	backButtonSprite_.LoadTexture("Resources/UI/end.png");
	backButtonSprite_.scale = Vector2::identity * 128.f;
	backButtonSprite_.pos = Vector2{ -547.f, -266.f };
	backButtonSprite_.uvSize.y = 0.5f;
	backButtonSprite_.Update();

	Fade::GetInstance()->Start(Vector2::zero, 0x00000000, 10);



	auto cameraBuff = Camera{ Camera::Type::Othographic };
	cameraBuff.Update();

	cameraMat_ = cameraBuff.GetViewOthographics();

	bgm_ = audioManager_->LoadWav("./Resources/Sound/BGM/title_stageSelect.wav", true);
	if (bgm_) {
		bgm_->Start(0.3f);
	}

	selectSE_ = audioManager_->LoadWav("Resources/Sound/SE/choice.wav", false);
}

void TitleScene::OnExit() {
	if (bgm_) {
		bgm_->Stop();
	}
}

void TitleScene::Update(const float deltaTime) {
	if (Gamepad::PushAnyKey()) {
		isPad_ = true;
	}
	else if (KeyInput::PushAnyKey() || Mouse::PushAnyKey()) {
		isPad_ = false;
	}

#ifdef _DEBUG
	//buttonSprite_.Debug("buttonSprite_");

	////// キーボード
	//ImGui::Begin("isPad");
	//ImGui::Checkbox("flg", &isPad_);
	//ImGui::End();
	
#endif // _DEBUG

	escSprite_.Update();
	backButtonSprite_.Update();

	//if (KeyInput::Pushed(DIK_SPACE) || KeyInput::Pushed(DIK_RETURN)) {
	sapceSprite_.uvPibot.y = 0.5f * (KeyInput::GetKey(DIK_SPACE) || KeyInput::GetKey(DIK_RETURN));
	//}
	buttonSprite_.uvPibot.y = 0.5f * Gamepad::GetButton(Gamepad::Button::A);

	if (Gamepad::Pushed(Gamepad::Button::A)
		|| KeyInput::Pushed(DIK_SPACE) || KeyInput::Pushed(DIK_RETURN)) {
		selectSE_->Start(1.0f);
		sceneManager_->ChangeScene(Vector2::zero, 0x000000FF, new LevelSelectScene, 20);
	}
	animateValue_ += deltaTime;

	SoLib::ImGuiWidget("buttonPos", &buttonSprite_.pos);

	buttonSprite_.color = 0xFFFFFF00 | static_cast<uint32_t>(0xFF * (std::sin(animateValue_ * Angle::PI2) * 0.5f + 0.5f));
	buttonSprite_.Update();

	sapceSprite_.color = 0xFFFFFF00 | static_cast<uint32_t>(0xFF * (std::sin(animateValue_ * Angle::PI2) * 0.5f + 0.5f));
	sapceSprite_.Update();

	titleSprite_.uvPibot.x = std::floor(animateValue_ * 4.f / kAnimateTime_) * 0.25f;
	titleSprite_.Update();
	//camera_.Update();
}

void TitleScene::Draw() {
	//const Mat4x4 &cametaMat = camera_.GetViewOthographics();
	titleSprite_.Draw(cameraMat_, Pipeline::Blend::Normal);
	if (isPad_) {
		buttonSprite_.Draw(cameraMat_, Pipeline::Blend::Normal);
		backButtonSprite_.Draw(cameraMat_, Pipeline::Blend::Normal);

	}
	else {
		escSprite_.Draw(cameraMat_, Pipeline::Blend::Normal);
		sapceSprite_.Draw(cameraMat_, Pipeline::Blend::Normal);
	}

	Fade::GetInstance()->Draw();
}
