#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "Utils/Camera/Camera.h"
#include <vector>

class BaseScene {
public:
	BaseScene() = default;
	BaseScene(const BaseScene&) = delete;
	BaseScene(BaseScene&&) = delete;
	virtual ~BaseScene() = default;

public:
	virtual void Initialize(class SceneManager* sceneManager) = 0;
	//virtual void Finalize() = 0;

	virtual void Update() = 0;
	virtual void Draw() = 0;

protected:
	class SceneManager* sceneManager_;

	class MeshManager* meshManager_;

	class AudioManager* audioManager_;

	class TextureManager* textureManager_;

	static bool isPad_;

protected:
	std::vector<Camera> cmaeras_;
};

class SceneManager final {
private:
	SceneManager();
	SceneManager(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	~SceneManager() = default;

	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager& operator=(SceneManager&&) = delete;

public:
	static SceneManager* const GetInstace();

public:
	void SceneChange(BaseScene* next);

	void Update();

	void Draw();

private:
	std::unique_ptr<BaseScene> scene_;
};