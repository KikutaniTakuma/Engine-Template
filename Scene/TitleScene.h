#pragma once
#include "SceneManager.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Utils/Camera/Camera.h"

class Input;
class AudioManager;
class Audio;

class TitleScene : public IScene {
public:
	TitleScene();
	~TitleScene();

	void OnEnter() override;
	void OnExit() override;

	void Update(const float deltaTime) override;
	void Draw() override;
public:

private:
	Camera camera_{ Camera::Type::Othographic };

	Audio *bgm_ = nullptr;
	Audio *selectSE_ = nullptr;
	float animateValue_ = 0.f;
	const float kAnimateTime_ = 2.f;

	float kButtonAnimateCircle_ = 1.f;

	Texture2D titleSprite_;
	Texture2D buttonSprite_;
	Texture2D sapceSprite_;

	Texture2D escSprite_;
	Texture2D backButtonSprite_;

	Mat4x4 cameraMat_;
};