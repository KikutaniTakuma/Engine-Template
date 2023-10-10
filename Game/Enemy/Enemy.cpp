#include "Enemy.h"
#include "Engine/FrameInfo/FrameInfo.h"
#include <numbers>
#include <cmath>

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
}


void Enemy::Update() {
	UpdatePos();
	moveVec = {};
	float deltaTime = FrameInfo::GetInstance()->GetDelta();

	freq += freqSpd * deltaTime;
	if (freq > 2.0f * std::numbers::pi_v<float>) {
		freq = 0.0f;
	}

	/*moveVec = { -std::sin(freq) * radius * freqSpd * deltaTime,
				0.0f,
				std::cos(freq) * radius * freqSpd* deltaTime
			  };*/

	//pos_ += moveVec;

	model[0]->pos += pos_;

	Vector2 rotate;
	rotate.x = moveVec.x;
	rotate.y = moveVec.z;

	model[0]->rotate.y = rotate.GetRad() + (std::numbers::pi_v<float> *0.5f);
}

void Enemy::Draw() {
	for (auto& i : model) {
		i->Draw(camera->GetViewProjection(), camera->pos);
	}
}