#include "Node.h"
#include "Utils/SoLib/SoLib.h"
#include <algorithm>

void Node::Initialize(
	const Vector2& mapNum,
	size_t isPort,
	const Vector2& pos,
	const std::string& fileName,
	const std::string& portFileName,
	const std::string& heartFileName,
	float size,
	Node::Type type
) {
	mapPos_ = mapNum;

	type_ = type;

	isPort = std::clamp(isPort, size_t(0x0), size_t(0xf));
	isPort_ = decltype(isPort_)(isPort);

	pos_ = pos;

	tex_.Initialize();
	tex_.LoadTexture(fileName);
	tex_.pos = pos_;
	tex_.scale *= size;
	tex_.uvPibotSpd_ = tex_.GetTexSize().y / tex_.GetTexSize().x;
	tex_.uvSize.x = tex_.uvPibotSpd_;



	if (type_ == Type::kMove || type_ == Type::kGoal) {
		heart_.Initialize();
		heart_.LoadTexture(heartFileName);
		heart_.pos = pos_;
		heart_.scale *= size;
		heart_.uvPibotSpd_ = heart_.GetTexSize().y / heart_.GetTexSize().x;
		heart_.uvSize.x = heart_.uvPibotSpd_;
	}

	size_ = size;


	for (auto& i : ports_) {
		i.Initialize();
		i.LoadTexture(portFileName);
		i.scale = tex_.scale * 0.3f;
		i.pos = pos_;
		//i.color = 0xffa500ff;
	}

	ports_[0].pos.y += size * 0.5f;
	ports_[1].pos.x += size * 0.5f;
	ports_[2].pos.y -= size * 0.5f;
	ports_[3].pos.x -= size * 0.5f;

	type_ = type;
	isGoalAni_ = false;
	isAni_ = false;

	aniDuration_ = std::chrono::milliseconds(500);

	isAni_ = false;
	easeT_ = 0.0f;
	easeSpd_ = 2.0f;

	conectHeartMaxScale_ = heart_.scale * 1.2f;
	conectHeartScale_ = heart_.scale;

	heartColor_ = 0xffffffff;
	heartMaxColor_ = 0xff0000ff;

	isPreConect_ = false;
	isConect_ = false;


	easeMoveT_ = 0.0f;
	easeMoveSpd_ = 10.0f;

	startPos_ = pos_;
	moveToPos_ = pos_;
	isEase_ = false;
	prePos_ = pos_;
	preMapPos_ = mapPos_;

	tex_.AnimationStart();
	if (type_ == Type::kGoal) {
		heart_.AnimationStart();
	}

	isCanGoal_ = false;
}

void Node::Update() {
	switch (type_){
	case Node::Type::kMove:
		for (auto& i : ports_) {
			i.Update();
		}

		for (auto& i : ports_) {
			i.pos += moveVec_;
		}

		if (moveVec_ != Vector2::zero && !isEase_) {
			moveToPos_ = pos_ + moveVec_;
			startPos_ = pos_;
			easeMoveT_ = 0.0f;
			isEase_ = true;

			preMapPos_ = mapPos_;
			prePos_ = pos_;
			mapPos_.x += moveVec_.Normalize().x;
			mapPos_.y -= moveVec_.Normalize().y;
		}

		if (isEase_) {
			//if (easeMoveT_ == 0.0f) {
			//	preMapPos_ = mapPos_;
			//	prePos_ = pos_;
			//	/*mapPos_.x += moveVec_.Normalize().x;
			//	mapPos_.y -= moveVec_.Normalize().y;*/
			//}
			pos_ = Vector2::Lerp(startPos_, moveToPos_, SoLib::easeOutExpo(easeMoveT_));
			easeMoveT_ += easeMoveSpd_ * ImGui::GetIO().DeltaTime;
			if (easeMoveT_ >= 1.0f) {
				easeMoveT_ = 0.0f;
				pos_ = moveToPos_;
				isEase_ = false;
				startPos_ = moveToPos_;
			}
		}

		tex_.pos = pos_;

		tex_.Update();

		heart_.pos = pos_;

		heart_.Update();

		for (auto& i : ports_) {
			i.pos = pos_;
		}

		ports_[0].pos.y += size_ * 0.5f;
		ports_[1].pos.x += size_ * 0.5f;
		ports_[2].pos.y -= size_ * 0.5f;
		ports_[3].pos.x -= size_ * 0.5f;

		break;

	case Node::Type::kHole:
	case Node::Type::kWall:
		for (auto& i : ports_) {
			i.Update();
		}
		tex_.pos = pos_;
		tex_.Update();


		heart_.pos = pos_;
		break;
	case Node::Type::kGoal:
		for (auto& i : ports_) {
			i.Update();
		}
		tex_.pos = pos_;
		tex_.Update();


		heart_.pos = pos_;
		heart_.Update();

		break;
	}
}

bool Node::Collision(const Vector2& pos) const{
	return tex_.Colision(pos);
}

void Node::Draw(const Mat4x4& cameraMat) {
	if (!isCanGoal_) {
		for (size_t i = 0; i < ports_.size(); i++) {
			if (isPort_[i]) {
				ports_[i].Draw(cameraMat, Pipeline::Normal);
			}
		}
	}

	if (type_ == Type::kGoal) {
		if (isCanGoal_ && isGoalAni_) {
			heart_.Animation(aniDuration_.count(), false, 2.0f, 4.0f);
		}
		else if(isCanGoal_ && !isGoalAni_) {
			heart_.Animation(aniDuration_.count(), true, 0.0f, 2.0f);
		}
		else {
			tex_.Animation(aniDuration_.count(), true, 0.0f, 2.0f);
		}

		if (isCanGoal_) {
			heart_.Draw(cameraMat, Pipeline::Normal);
		}
		else {
			tex_.Draw(cameraMat, Pipeline::Normal);
		}
		isPreConect_ = isConect_;
		return;
	}
	else if (type_ == Type::kMove) {
		ConectAnimation();
	}
	
	tex_.Draw(cameraMat, Pipeline::Normal);

	if (type_ == Type::kMove) {
		heart_.Draw(cameraMat, Pipeline::Normal);
	}

	isPreConect_ = isConect_;
}

void Node::SetStartPos() {
	while (!posData_.empty())
	{
		if (posData_.size() == 1) {
			pos_ = posData_.top();
			posData_.pop();
		}
		else {
			posData_.pop();
		}
	}

	while (!mapPosData_.empty())
	{
		if (mapPosData_.size() == 1) {
			mapPos_ = mapPosData_.top();
			mapPosData_.pop();
		}
		else {
			mapPosData_.pop();
		}
	}

	for (auto& i : ports_) {
		i.pos = pos_;
	}


	ports_[0].pos.y += size_ * 0.5f;
	ports_[1].pos.x += size_ * 0.5f;
	ports_[2].pos.y -= size_ * 0.5f;
	ports_[3].pos.x -= size_ * 0.5f;

	prePos_ = pos_;
	preMapPos_ = mapPos_;
	startPos_ = pos_;
	moveToPos_ = pos_;
}

void Node::SaveThisFramePos() {
	// stackにデータを格納
	posData_.push(pos_);

	mapPosData_.push(mapPos_);
}

void Node::SetPrePos() {
	prePos_ = pos_;
	preMapPos_ = mapPos_;

	if (!posData_.empty()) {
		// stackの一番上のデータを取得
		pos_ = posData_.top();
		// stackの一番上のデータを削除
		posData_.pop();

		for (auto& i : ports_) {
			i.pos = pos_;
		}

		ports_[0].pos.y += size_ * 0.5f;
		ports_[1].pos.x += size_ * 0.5f;
		ports_[2].pos.y -= size_ * 0.5f;
		ports_[3].pos.x -= size_ * 0.5f;
	}

	if (!mapPosData_.empty()) {
		// stackの一番上のデータを取得
		mapPos_ = mapPosData_.top();
		// stackの一番上のデータを削除
		mapPosData_.pop();
	}

	startPos_ = pos_;
	moveToPos_ = pos_;
}
bool Node::Animation() {
	if (type_ == Type::kGoal) {
		tex_.AnimationRestart();
		return false;
	}
	else {
		return false;
	}
}

void Node::ConectAnimation() {
	// 繋がった瞬間
	if (isConect_ && !isPreConect_) {
		isAni_ = true;
	}
	// 離れた瞬間
	else if (!isConect_/* && isPreConect_*/) {
		heart_.scale = conectHeartScale_;
		heart_.color = heartColor_;
		isAni_ = false;
		easeT_ = 0.0f;
	}

	if (isAni_) {
		heart_.scale = Vector2::Lerp(conectHeartScale_, conectHeartMaxScale_, SoLib::easeOutElastic(easeT_));
		heart_.color = SoLib::ColorLerp(heartColor_, heartMaxColor_, easeT_);
		easeT_ += easeSpd_ * ImGui::GetIO().DeltaTime;
		if (easeT_ > 1.0f) {
			heart_.scale = conectHeartMaxScale_;
			heart_.color = heartMaxColor_;
			isAni_ = false;
			easeT_ = 0.0f;
		}
	}
}
