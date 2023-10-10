#pragma once
#include "Drawers/Model/Model.h"
#include <memory>
#include "Utils/Camera/Camera.h"
#include <vector>
#include "Game/CollisionManager/Collider/Collider.h"

class Enemy : public Collider {
public:
	Enemy();
	Enemy(const Enemy&) = default;
	~Enemy() = default;
	Enemy& operator=(const Enemy&) = default;

public:
	void Update();

	void Draw();

	inline Vector3 GetPos() const {
		return (*model.begin())->pos;
	}

	inline void SetCamera(Camera* camera_) {
		camera = camera_;
	}

	inline void SetPlayer(class Player* player) {
		player_ = player;
	}

private:
	std::vector<std::unique_ptr<Model>> model;

	float spd;

	Vector3 moveVec;

	Camera* camera;

	float freqSpd;
	float freq;
	float radius;

	class Player* player_=nullptr;
};