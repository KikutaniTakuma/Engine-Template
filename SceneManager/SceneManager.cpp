#include "SceneManager.h"

bool BaseScene::isPad_ = false;

SceneManager* const SceneManager::GetInstace() {
	static SceneManager instance;
	return &instance;
}

SceneManager::SceneManager() {

}

void SceneManager::SceneChange(BaseScene* next) {
	if (next_) {
		return;
	}
	next_.reset(next);
}

void SceneManager::Update() {
	if (scene_) {
		scene_->Update();
	}
}

void SceneManager::Draw() {
	if (scene_) {
		scene_->Draw();
	}
}
