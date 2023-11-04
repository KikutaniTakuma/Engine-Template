#pragma once
#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include "Mesh/Mesh.h"
#include "Utils/UtilsLib/UtilsLib.h"


class MeshManager final {
private:
	MeshManager() = default;
	MeshManager(const MeshManager&) = delete;
	MeshManager(MeshManager&&) = delete;
	~MeshManager() = default;

	MeshManager& operator=(const MeshManager&) = delete;
	MeshManager& operator=(MeshManager&&) = delete;

public:
	static MeshManager* const GetInstance();

public:
	Mesh* LoadObj(const std::string& objFileName);
	void LoadObj(const std::string& objFileName, Mesh** mesh_);

	void ResetDrawCount();

	void Draw();


public:
	void ThreadLoad();

	void CheckLoadFinish();

	void JoinThread();

	bool IsNowThreadLoading();

private:
	std::unordered_map<std::string, std::unique_ptr<Mesh>> meshs_;

	std::queue<std::pair<std::string, Mesh**>> threadMeshBuff_;
	std::thread load_;
	std::mutex mtx_;
	bool isThreadFinish_;
};