#pragma once
#include "SceneManager/SceneManager.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "GlobalVariables/GlobalVariables.h"

class GameScene : public BaseScene {
public:
	GameScene();
	~GameScene();

public:
	void Initialize(SceneManager* const sceneManager) override;

	void Update() override;

	void Draw() override;

public:
	std::vector<Model> models_;
	std::vector<Texture2D> texs_;

	GlobalVariables globalVariables_;
};