#include "Player.h"
#include "Input/Input.h"
#include "Engine/FrameInfo/FrameInfo.h"
#include <numbers>
#include <cmath>
#include "externals/nlohmann/json.hpp"
#include "GlobalVariables/GlobalVariables.h"
#include "externals/imgui/imgui.h"

Player::Player(GlobalVariables* globalVariables):
	spd(10.0f),
	moveVec(),
	camera(nullptr),
	freqSpd(std::numbers::pi_v<float>),
	freq(0.0f),
	armFreqSpd(std::numbers::pi_v<float> / 2.0f),
	armFreq(0.0f),
	behavior(Behavior::Normal),
	attack(0.0f),
	attackSpd(std::numbers::pi_v<float> / 2.0f),
	cmaeraRotateSpd_(std::numbers::pi_v<float>),
	cameraRotate_()
{
	model.push_back(std::make_unique<Model>());
	auto itr = model.rbegin();
	(*itr)->LoadObj("AL_Resouce/PlayerDivision/PlayerBody.obj");

	model.push_back(std::make_unique<Model>());
	itr = model.rbegin();
	(*itr)->LoadObj("AL_Resouce/PlayerDivision/PlayerHead.obj");
	(*itr)->SetParent(model.begin()->get());
	

	model.push_back(std::make_unique<Model>());
	itr = model.rbegin();
	(*itr)->LoadObj("AL_Resouce/PlayerDivision/PlayerLeftArm.obj");
	(*itr)->SetParent(model.begin()->get());

	model.push_back(std::make_unique<Model>());
	itr = model.rbegin();
	(*itr)->LoadObj("AL_Resouce/PlayerDivision/PlayerRightArm.obj");
	(*itr)->SetParent(model.begin()->get());

	const std::string groupName = "Player";
	const std::string groupName2 = "PlayerParts";
	globalVariables_ = globalVariables;
	globalVariables_->CreateGroup(groupName);
	globalVariables_->AddItem(groupName2, "Body Translation", model[0]->pos);
	globalVariables_->AddItem(groupName2, "Head Translation", model[1]->pos);
	globalVariables_->AddItem(groupName2, "LeftArm Translation", model[2]->pos);
	globalVariables_->AddItem(groupName2, "RightArm Translation", model[3]->pos);
	globalVariables_->AddItem(groupName, "Speed", spd);
	globalVariables_->AddItem(groupName, "freqSpd", freqSpd);
	globalVariables_->AddItem(groupName, "armFreqSpd", armFreqSpd);
	globalVariables_->AddItem(groupName, "attackSpd", attackSpd);
	globalVariables_->AddItem(groupName, "cmaeraRotateSpd_", cmaeraRotateSpd_);
}

void Player::ApplyGlobalVariables() {
	const std::string groupName = "Player";
	const std::string groupName2 = "PlayerParts";
	model[0]->pos = globalVariables_->GetVector3Value(groupName2, "Body Translation");
	model[1]->pos = globalVariables_->GetVector3Value(groupName2, "Head Translation");
	model[2]->pos = globalVariables_->GetVector3Value(groupName2, "LeftArm Translation");
	model[3]->pos = globalVariables_->GetVector3Value(groupName2, "RightArm Translation");
	spd = globalVariables_->GetFloatValue(groupName, "Speed");
	freqSpd = globalVariables_->GetFloatValue(groupName, "freqSpd");
	armFreqSpd = globalVariables_->GetFloatValue(groupName, "armFreqSpd");
	attackSpd = globalVariables_->GetFloatValue(groupName, "attackSpd");
	cmaeraRotateSpd_ = globalVariables_->GetFloatValue(groupName, "cmaeraRotateSpd_");
}

void Player::Animation() {
	float deltaTime = FrameInfo::GetInstance()->GetDelta();
	freq += freqSpd * deltaTime;
	model[0]->pos.y = std::sin(freq) + 2.5f;

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

		model[2]->rotate.y = armFreq;
		model[3]->rotate.y = armFreq;
		break;
	}
}

void Player::Update() {
	ApplyGlobalVariables();
	UpdateCollision();
	moveVec = {};
	bool isMove = false;
	Animation();

	static Input* input = Input::GetInstance();

	if (input->GetKey()->LongPush(DIK_W)) {
		moveVec.z += spd;
		isMove = true;
	}
	if (input->GetKey()->LongPush(DIK_A)) {
		moveVec.x -= spd;
		isMove = true;
	}
	if (input->GetKey()->LongPush(DIK_S)) {
		moveVec.z -= spd;
		isMove = true;
	}
	if (input->GetKey()->LongPush(DIK_D)) {
		moveVec.x += spd;
		isMove = true;
	}

	if (input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_X) > 0.15f || input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_X) < -0.15f) {
		moveVec.x += spd * input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_X);
		isMove = true;
	}
	if (input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_Y) > 0.15f || input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_Y) < -0.15f) {
		moveVec.z += spd * input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_Y);
		isMove = true;
	}

	if (input->GetGamepad()->GetStick(Gamepad::Stick::RIGHT_X) > 0.15f || input->GetGamepad()->GetStick(Gamepad::Stick::RIGHT_X) < -0.15f) {
		cameraRotate_ += cmaeraRotateSpd_ * input->GetGamepad()->GetStick(Gamepad::Stick::RIGHT_X) * FrameInfo::GetInstance()->GetDelta();
		
		isMove = true;
	}
	/*if (input->GetGamepad()->GetStick(Gamepad::Stick::RIGHT_Y) > 0.15f || input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_Y) < -0.15f) {
		moveVec.z += spd * input->GetGamepad()->GetStick(Gamepad::Stick::LEFT_Y);
		isMove = true;
	}*/
	
	/*Vector2 moveRotate;
	moveRotate.x = moveVec.x;
	moveRotate.y = moveVec.z;

	moveRotate.Rotate(camera->gazePointRotate.x);
	ImGui::Begin("Hoge");
	ImGui::DragFloat2("Hoge", &camera->gazePointRotate.x);
	ImGui::End();
	moveVec.x = moveRotate.x;
	moveVec.z = moveRotate.y;*/

	moveVec *= HoriMakeMatrixRotateY(cameraRotate_);

	pos_ += moveVec.Normalize() * spd * FrameInfo::GetInstance()->GetDelta();

	model[0]->pos = pos_;

	if (isMove) {
		Vector2 rotate;
		rotate.x = -moveVec.x;
		rotate.y = moveVec.z;
		
		model[0]->rotate.y = rotate.GetRad() - (std::numbers::pi_v<float> *0.5f);
	}

	ImGui::Begin("Player");
	ImGui::DragFloat3("pos_", &collisionPos_.x, 0.01f);
	static auto cameraPos = camera->pos;
	cameraPos = camera->pos;
	ImGui::DragFloat3("scale_", &scale_.x, 0.01f);
	ImGui::DragFloat3("cameraPos", &cameraPos.x, 0.01f);
	ImGui::DragFloat("cameraRotate_", &cameraRotate_, 0.01f);
	//camera->pos = cameraPos;
	ImGui::End();

	if (camera) {
		Vector3 offset = { 0.0f, 2.0f, -20.0f };
		camera->rotate.y = cameraRotate_;
		offset *= HoriMakeMatrixRotateY(cameraRotate_);
		camera->pos = model[0]->pos + offset;
		camera->Update();
	}
}

void Player::Draw() {
	for (auto& i : model) {
		i->Draw(camera->GetViewProjection(), camera->pos);
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