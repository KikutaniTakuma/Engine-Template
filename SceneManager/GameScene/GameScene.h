#pragma once
#include "SceneManager/SceneManager.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Game/Player/Player.h"
#include "Game/MoveFloor/MoveFloor.h"
#include "Game/Goal/Goal.h"
#include "Game/Enemy/Enemy.h"

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
	std::unique_ptr<Goal> goal_;
	std::unique_ptr<Enemy> enemy_;
	std::unique_ptr<Model> skyDome_;

	std::vector<MoveFloor> floor_;

	GlobalVariables globalVariables_;
};