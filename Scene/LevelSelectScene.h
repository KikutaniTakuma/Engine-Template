#pragma once
#include "SceneManager.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Utils/Camera/Camera.h"

class Input;
class AudioManager;
class LevelSelect;
class Audio;

class LevelSelectScene : public IScene {
public:
	LevelSelectScene();
	~LevelSelectScene();

	void OnEnter() override;
	void OnExit() override;

	void Update(const float deltaTime) override;
	void Draw() override;
public:

private:
	Camera camera_{ Camera::Type::Othographic };
	Texture2D background_;
	LevelSelect *levelSelecter_ = nullptr;

	Audio *bgm_ = nullptr;
	Audio *selectSE_ = nullptr;


	// 移動HUD
	/*Texture2D moveButton_;
	Texture2D moveKey_;*/

	enum class KeyTexture {
		W,
		A,
		S,
		D,
		kCount
	};

	std::array<Texture2D, static_cast<uint32_t>(KeyTexture::kCount)> keySprite_;
	std::array<Texture2D, static_cast<uint32_t>(KeyTexture::kCount)> dpadSprite_;

	Texture2D upButton_;
	Texture2D downButton_;
	Texture2D leftButton_;
	Texture2D rightButton_;

	// 戻るHUD
	Texture2D backKey_;
	Texture2D backButton_;

	// 決定HUD
	Texture2D buttonSprite_;
	Texture2D sapceSprite_;
};