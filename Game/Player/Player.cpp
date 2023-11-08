#include "Player.h"
#include "Input/Input.h"
#include "Engine/FrameInfo/FrameInfo.h"
#include <numbers>
#include <cmath>
#include "externals/nlohmann/json.hpp"
#include "GlobalVariables/GlobalVariables.h"
#include "externals/imgui/imgui.h"

void Player::Initialize(GlobalVariables* globalVariables) {
	const std::string groupName = "Player";
	const std::string groupName2 = "PlayerParts";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName2, "Body Translation", Vector3::zero);
	globalVariables->AddItem(groupName2, "Head Translation", Vector3::zero);
	globalVariables->AddItem(groupName2, "LeftArm Translation", Vector3::zero);
	globalVariables->AddItem(groupName2, "RightArm Translation", Vector3::zero);
	globalVariables->AddItem(groupName, "Speed", 10.0f);
	globalVariables->AddItem(groupName, "freqSpd", std::numbers::pi_v<float>);
	globalVariables->AddItem(groupName, "armFreqSpd", std::numbers::pi_v<float> / 2.0f);
	globalVariables->AddItem(groupName, "attackSpd", std::numbers::pi_v<float> / 2.0f);
	globalVariables->AddItem(groupName, "cmaeraRotateSpd_", std::numbers::pi_v<float>);
	globalVariables->AddItem(groupName, "dashCoolTime_", 500);
	globalVariables->AddItem(groupName, "dashScale_", 2.0f);
	globalVariables->AddItem(groupName, "delayCaemraSpeed_", 0.2f);
}

Player::Player(GlobalVariables* globalVariables):
	spd(10.0f),
	moveVec_(),
	camera(nullptr),
	freqSpd(std::numbers::pi_v<float>),
	freq(0.0f),
	armFreqSpd(std::numbers::pi_v<float> / 2.0f),
	armFreq(0.0f),
	behavior(Behavior::Normal),
	attack(0.0f),
	attackSpd(std::numbers::pi_v<float> / 2.0f),
	cmaeraRotateSpd_(std::numbers::pi_v<float>),
	cameraRotate_(),
	dashStartTime_{},
	dashCoolTime_{500},
	isDash_{false},
	dashScale_{2.0f},
	cameraEaseing_{},
	preCameraPos_{},
	delayCaemraSpeed_{0.2f}
{
	globalVariables_ = globalVariables;

	model_.push_back(std::make_unique<Model>());
	auto itr = model_.rbegin();
	(*itr)->LoadObj("AL_Resouce/PlayerDivision/PlayerBody.obj");

	model_.push_back(std::make_unique<Model>());
	itr = model_.rbegin();
	(*itr)->LoadObj("AL_Resouce/PlayerDivision/PlayerHead.obj");
	(*itr)->SetParent(model_.begin()->get());
	

	model_.push_back(std::make_unique<Model>());
	itr = model_.rbegin();
	(*itr)->LoadObj("AL_Resouce/PlayerDivision/PlayerLeftArm.obj");
	(*itr)->SetParent(model_.begin()->get());

	model_.push_back(std::make_unique<Model>());
	itr = model_.rbegin();
	(*itr)->LoadObj("AL_Resouce/PlayerDivision/PlayerRightArm.obj");
	(*itr)->SetParent(model_.begin()->get());

	weapon_ = std::make_unique<Model>();
	weapon_->LoadObj("AL_Resouce/Weapon/Hammer.obj");
	weapon_->SetParent(model_.begin()->get());
	weapon_->scale_ *= 0.5f;
	weapon_->pos_.y = 10.45f;
	weaponColliser_.scale_ = { 5.0f, 10.0f, 5.0f };

	collisionPos_.y = 1.85f;
	scale_.y = 5.0f;
	scale_.x = 2.09f;
	scale_.z = 2.09f;

	pos_.y = 8.0f;
	pos_.x = 0.0f;
}

void Player::ApplyGlobalVariables() {
	const std::string groupName = "Player";
	const std::string groupName2 = "PlayerParts";
	model_[0]->pos_ = globalVariables_->GetVector3Value(groupName2, "Body Translation");
	model_[1]->pos_ = globalVariables_->GetVector3Value(groupName2, "Head Translation");
	model_[2]->pos_ = globalVariables_->GetVector3Value(groupName2, "LeftArm Translation");
	model_[3]->pos_ = globalVariables_->GetVector3Value(groupName2, "RightArm Translation");
	spd = globalVariables_->GetFloatValue(groupName, "Speed");
	freqSpd = globalVariables_->GetFloatValue(groupName, "freqSpd");
	armFreqSpd = globalVariables_->GetFloatValue(groupName, "armFreqSpd");
	attackSpd = globalVariables_->GetFloatValue(groupName, "attackSpd");
	cmaeraRotateSpd_ = globalVariables_->GetFloatValue(groupName, "cmaeraRotateSpd_");
	dashCoolTime_ = std::chrono::milliseconds { globalVariables_->GetIntValue(groupName, "dashCoolTime_") };
	dashScale_ = globalVariables_->GetFloatValue(groupName, "dashScale_");
}

void Player::Animation() {
	float deltaTime = FrameInfo::GetInstance()->GetDelta();
	freq += freqSpd * deltaTime;
	model_[0]->pos_.y = std::sin(freq) + 2.5f;

	if (freq > (std::numbers::pi_v<float> *2.0f)) {
		freq = 0.0f;
	}

	switch (behavior)
	{
	case Player::Behavior::Normal:
	default:
		armFreq += armFreqSpd * deltaTime;

		if (armFreq > std::numbers::pi_v<float> *2.0f) {
			armFreq = 0.0f;
		}

		model_[2]->rotate_.y = armFreq;
		model_[3]->rotate_.y = armFreq;
		break;
	case Player::Behavior::Attack:
		armFreq += attackSpd * deltaTime;

		if (armFreq > (std::numbers::pi_v<float> *0.5f)) {
			armFreq = 0.0f;
			behavior = Behavior::Normal;

			model_[2]->rotate_.x = 0.0f;
			model_[3]->rotate_.x = 0.0f;
			break;
		}

		model_[2]->rotate_.x = armFreq + std::numbers::pi_v<float>;
		model_[3]->rotate_.x = armFreq + std::numbers::pi_v<float>;

		weapon_->worldMat_ = MakeMatrixAffin(weapon_->scale_, Vector3::zero, weapon_->pos_);
		weapon_->rotate_.x = armFreq;
		break;
	}
}

void Player::Move() {
	isDash_.Update();
	ApplyGlobalVariables();
	UpdateCollision();
	weaponColliser_.UpdateCollision();
	Animation();

	bool isMove = false;
	moveVec_ = {};
	static Input* input = Input::GetInstance();

	if (!isDash_) {
		if (input->GetKey()->LongPush(DIK_W)) {
			moveVec_.z += spd;
			isMove = true;
		}
		if (input->GetKey()->LongPush(DIK_A)) {
			moveVec_.x -= spd;
			isMove = true;
		}
		if (input->GetKey()->LongPush(DIK_S)) {
			moveVec_.z -= spd;
			isMove = true;
		}
		if (input->GetKey()->LongPush(DIK_D)) {
			moveVec_.x += spd;
			isMove = true;
		}

		if (input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_X) > 0.25f || input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_X) < -0.25f) {
			moveVec_.x = 0.0f;
			moveVec_.x += spd * input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_X);
			isMove = true;
		}
		if (input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_Y) > 0.25f || input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_Y) < -0.25f) {
			moveVec_.z = 0.0f;
			moveVec_.z += spd * input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_Y);
			isMove = true;
		}
	}

	if (input->GetGamepad()->GetStick(Gamepad::Stick::RIGHT_X) > 0.25f || input->GetGamepad()->GetStick(Gamepad::Stick::RIGHT_X) < -0.25f) {
		cameraRotate_ += cmaeraRotateSpd_ * input->GetGamepad()->GetStick(Gamepad::Stick::RIGHT_X) * FrameInfo::GetInstance()->GetDelta();

		isMove = true;
	}

	moveVec_ *= MakeMatrixRotateY(cameraRotate_);

	auto nowTime = std::chrono::steady_clock::now();
	if (!isDash_) {
		if (input->GetKey()->LongPush(DIK_RETURN) ||
			input->GetGamepad()->Pushed(Gamepad::Button::B)) {
			isDash_ = true;
			dashStartTime_ = nowTime;
			
		}
	}

	if (isDash_.OnEnter()) {
		spd *= dashScale_;
		moveVec_ = Vector3::zIdy;
		if (preMoveVec_.Normalize() == -Vector3::zIdy) {
			moveVec_ *= DirectionToDirection(-Vector3::zIdy, preMoveVec_.Normalize());
		}
		else {
			moveVec_ *= DirectionToDirection(Vector3::zIdy, preMoveVec_.Normalize());
		}
	}


	if(isDash_){
		if (dashCoolTime_ <= std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - dashStartTime_)) {
			isDash_ = false;
			spd /= dashScale_;
		}
	}

	moveVec_.y = -15.0f;
	collisionPos_ += moveVec_.Normalize() * spd * FrameInfo::GetInstance()->GetDelta();
}

void Player::Update() {
	//ApplyGlobalVariables();

	if (moveVec_.y!=0.0f) {
		pos_.y += moveVec_.y * FrameInfo::GetInstance()->GetDelta();
		moveVec_.y = 0.0f;
	}
	pos_ += moveVec_.Normalize() * spd * FrameInfo::GetInstance()->GetDelta();
	collisionPos_ = pos_;
	collisionPos_.y += 1.85f;

	model_[0]->pos_ = pos_;

	if (KeyInput::GetInstance()->Pushed(DIK_SPACE) || Gamepad::GetInstance()->Pushed(Gamepad::Button::A)) {
		behavior = Behavior::Attack;

		armFreq = 0.0f;

		model_[2]->rotate_.y = 0.0f;
		model_[3]->rotate_.y = 0.0f;
	}

	if (camera) {
		Vector3 offset = { 0.0f, 7.0f, -30.0f };
		camera->rotate.y = cameraRotate_;
		camera->rotate.x = 0.2f;
		offset *= MakeMatrixRotateY(cameraRotate_);
		if (isDash_.OnEnter()) {
			cameraEaseing_.Start(
				false,
				delayCaemraSpeed_
			);
			preCameraPos_ = camera->pos;
		}
		if (cameraEaseing_.ActiveEnter() || cameraEaseing_.ActiveStay()) {
			camera->pos = cameraEaseing_.Get(preCameraPos_, model_[0]->pos_ + offset);
		}
		else {
			camera->pos = model_[0]->pos_ + offset;
		}
		camera->Update();
	}

	cameraEaseing_.Update();

	if (moveVec_ != Vector3::zero && !isDash_) {
		preMoveVec_ = moveVec_;
	}

	for (auto& i : model_) {
		i->Update();
	}
	if (preMoveVec_ != Vector3::zero) {
		if (preMoveVec_.Normalize() == -Vector3::zIdy) {
			model_[0]->worldMat_ = DirectionToDirection(-Vector3::zIdy, preMoveVec_.Normalize()) * MakeMatrixRotateY(std::numbers::pi_v<float>) * model_[0]->worldMat_;
		}
		else {
			model_[0]->worldMat_ = DirectionToDirection(Vector3::zIdy, preMoveVec_.Normalize()) * model_[0]->worldMat_;
		}
	}
	weapon_->Debug("weapon_");
	weapon_->Update();
	if (behavior == Behavior::Attack) {
		weapon_->worldMat_.Affin(weapon_->scale_, Vector3::zero, weapon_->pos_);
		weapon_->worldMat_ *= MakeMatrixRotate(weapon_->rotate_);
	}
	weaponColliser_.collisionPos_ = weapon_->GetPos();
}

void Player::Draw() {
	for (auto& i : model_) {
		i->Draw(camera->GetViewProjection(), camera->pos);
	}

	if (behavior == Behavior::Attack) {
		weapon_->Draw(camera->GetViewProjection(), camera->pos);
		weaponColliser_.DebugDraw(camera->GetViewProjection());
	}

	DebugDraw(camera->GetViewProjection());
}

void Player::Debug() {
	/*model[0]->Debug("Player");
	ImGui::Begin("Player");
	ImGui::DragFloat("spd", &spd);
	if (ImGui::TreeNode("Freq")) {
		ImGui::DragFloat("Freq", &freq);
		ImGui::DragFloat("FreqSpd", &freqSpd);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("armFreq")) {
		ImGui::DragFloat("armFreq", &armFreq);
		ImGui::DragFloat("armFreqSpd", &armFreqSpd);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Attack")) {
		ImGui::DragFloat("attack", &attack);
		ImGui::DragFloat("attackSpd", &attackSpd);
		ImGui::TreePop();
	}
	ImGui::End();*/
}