#include "Enemy.h"
#include "Engine/FrameInfo/FrameInfo.h"
#include <numbers>
#include <cmath>
#include "Game/Player/Player.h"

Enemy::Enemy() :
	spd(10.0f),
	moveVec(),
	camera(nullptr),
	model(0),
	freqSpd(std::numbers::pi_v<float>),
	freq(0.0f),
	radius(5.0f)
{
	model.push_back(std::make_unique<Model>());
	model[0]->LoadObj("AL_Resouce/Enemy/Enemy.obj");
	model.push_back(std::make_unique<Model>());
	model[1]->LoadObj("./Resources/Cube.obj");
	model[1]->SetParent(model[1].get());
	model[1]->scale *= 0.3f;

	easeDuration.first.y = 1.0f;
	easeDuration.first.y = 1.5f;

	ease.Start(true, 1.0f, Easeing::InOutSine);

	distanceLimit = 1.0f;
	isPlayerCollsion = false;
}


void Enemy::Update() {
	UpdateCollision();
	moveVec = {};
	float deltaTime = FrameInfo::GetInstance()->GetDelta();

	freq += freqSpd * deltaTime;
	if (freq > 2.0f * std::numbers::pi_v<float>) {
		freq = 0.0f;
	}

	if (player_) {
		if ((player_->collisionPos_ + collisionPos_).Length() < distanceLimit) {
			moveVec = (player_->collisionPos_ - collisionPos_).Normalize() * spd;
		}
	}

	model[1]->pos = ease.Get(easeDuration.first, easeDuration.second);

	model[0]->pos += collisionPos_;

	Vector2 rotate;
	rotate.x = moveVec.x;
	rotate.y = moveVec.z;

	model[0]->rotate.y = rotate.GetRad() + (std::numbers::pi_v<float> *0.5f);
}

void Enemy::Draw() {
	for (auto& i : model) {
		i->Draw(camera->GetViewProjection(), camera->pos);
	}

	DebugDraw(camera->GetViewProjection());
}