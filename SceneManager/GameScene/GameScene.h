#pragma once
#include "SceneManager/SceneManager.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "Drawers/Particle/Particle.h"
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
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

public:
	Model model_;

	//float rotateSpd_;

	Particle particle_;

	PeraRender pera_;
};