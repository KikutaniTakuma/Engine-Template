#pragma once
#include "SceneManager/SceneManager.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Game/Player/Player.h"

class GameScene : public BaseScene {
public:
	GameScene();
	GameScene(const GameScene&) = delete;
	GameScene(GameScene&&) = delete;
	~GameScene() = default;

	GameScene& operator=(const GameScene&) = delete;
	GameScene& operator=(GameScene&&) = delete;

public:
	void Initialize(SceneManager* const sceneManager) override;

	void Update() override;

	void Draw() override;

public:
	std::vector<Model> models_;
	std::vector<Texture2D> texs_;
	std::unique_ptr<Player> player_;

	GlobalVariables globalVariables_;
};