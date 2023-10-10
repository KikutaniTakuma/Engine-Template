#pragma once
#include "Drawers/Model/Model.h"
#include <memory>
#include "Utils/Camera/Camera.h"
#include <vector>
#include "Game/CollisionManager/Collider/Collider.h"

class Player : public Collider {
public:
	enum class Behavior {
		Normal,
		Attack,
	};

public:
	Player() = delete;
	Player(class GlobalVariables* globalVariables);
	Player(const Player&) = default;
	~Player() = default;
	Player& operator=(const Player&) = default;

public:
	void Update();

	void Draw();

	inline Vector3 GetPos() const {
		return (*model.begin())->pos;
	}

	inline void SetCamera(Camera* camera_) {
		camera = camera_;
	}

	void Debug();

private:
	void Animation();

	void ApplyGlobalVariables();

private:
	class GlobalVariables* globalVariables_ = nullptr;

	std::vector<std::unique_ptr<Model>> model;

	float spd;

	Vector3 moveVec;

	Camera* camera;

	float freqSpd;
	float freq;

	float armFreqSpd;
	float armFreq;

	Behavior behavior;

	float attack;
	float attackSpd;

	float cameraRotate_;
	float cmaeraRotateSpd_;
};