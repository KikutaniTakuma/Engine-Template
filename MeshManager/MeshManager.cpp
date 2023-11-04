#include "MeshManager.h"

MeshManager* const MeshManager::GetInstance() {
	static MeshManager instance;

	return &instance;
}

Mesh* MeshManager::LoadObj(const std::string& objFileName) {
	auto mesh = meshs_.find(objFileName);

	if (mesh == meshs_.end()) {
		meshs_[objFileName];
		meshs_[objFileName] = std::make_unique<Mesh>();
		meshs_[objFileName]->LoadObj(objFileName);
	}

	return meshs_[objFileName].get();
}

void MeshManager::LoadObj(const std::string& objFileName, Mesh** mesh) {
	threadMeshBuff_.push(std::make_pair(objFileName, mesh));
}

void MeshManager::ResetDrawCount() {
	for (auto& i : meshs_) {
		i.second->ResetDrawCount();
	}
}

void MeshManager::Draw() {
	for (auto& i : meshs_) {
		i.second->Draw();
	}
}

void MeshManager::ThreadLoad() {
	if (!threadMeshBuff_.empty() && !load_.joinable()) {
		auto loadProc = [this]() {
			std::lock_guard<std::mutex> lock(mtx_);

			while (!threadMeshBuff_.empty()) {
				// ロードするmeshを取得
				auto& front = threadMeshBuff_.front();
				
				// ロード済みか判定
				auto mesh = meshs_.find(front.first);

				// ロード済みではなかったらロードして代入
				if (mesh == meshs_.end()) {
					meshs_[front.first] = std::make_unique<Mesh>();
					meshs_[front.first]->ThreadLoadObj(front.first);
					(*front.second) = meshs_[front.first].get();
				}
				// ロード済みだったらロード済みのmeshポインタを代入
				else {
					(*front.second) = mesh->second.get();
				}
				threadMeshBuff_.pop();
			}

			isThreadFinish_ = true;
			};

		load_ = std::thread(loadProc);
	}
}

void MeshManager::CheckLoadFinish() {
	if (isThreadFinish_) {
		for (auto& i : meshs_) {
			i.second->CheckModelTextureLoadFinish();
		}

		for (auto& i : meshs_) {
			if (!i.second->isLoad_) {
				return;
			}
		}

		isThreadFinish_ = false;
	}
}
void MeshManager::JoinThread() {
	if (isThreadFinish_ && load_.joinable()) {
		load_.join();
	}
}

bool MeshManager::IsNowThreadLoading() {
	return isThreadFinish_ && !threadMeshBuff_.empty();
}