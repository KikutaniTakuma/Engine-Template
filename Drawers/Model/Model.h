#pragma once
#include <string>
#include "Engine/ConstBuffer/ConstBuffer.h"
#include "Engine/RootSignature/RootSignature.h"
#include "TextureManager/TextureManager.h"
#include "Engine/ShaderManager/ShaderManager.h"
#include <unordered_map>
#include "MeshManager/Mesh/Mesh.h"

class Pipeline;

#include <wrl.h>

class Model {
public:
	Model();
	Model(const std::string& fileName);
	Model(const Model& right);
	Model(Model&& right) noexcept;
	~Model();

	Model& operator=(const Model& right);
	Model& operator=(Model&& right) noexcept;

public:
	void LoadObj(const std::string& fileName);

	void ThreadLoadObj(const std::string& fileName);

public:
	static void Initialize(
		const std::string& vertex = "./Resources/Shaders/ModelShader/Model.VS.hlsl",
		const std::string& pixel = "./Resources/Shaders/ModelShader/Model.PS.hlsl",
		const std::string& geometory = {},
		const std::string& hull = {},
		const std::string& domain = {}
	);

private:
	static void LoadShader(
		const std::string& vertex,
		const std::string& pixel,
		const std::string& geometory,
		const std::string& hull,
		const std::string& domain
	);
	static void CreateGraphicsPipeline();

private:
	static Shader shader;

	static Pipeline* pipeline;
	static bool loadShaderFlg;
	static bool createGPFlg;


public:
	void Update();

	void Draw(const Mat4x4& viewProjectionMat, const Vector3& cameraPos);

	void InstancingDraw(const Mat4x4& viewProjectionMat, const Vector3& cameraPos);


	void Debug(const std::string& guiName);

	void SetParent(Model* parent_) {
		parent = parent_;
	}
	
	void ChangeTexture(const std::string& useMtlName, const std::string& texName);
	void ChangeTexture(const std::string& useMtlName, Texture* tex);

	void MeshChangeTexture(const std::string& useMtlName, const std::string& texName);
	void MeshChangeTexture(const std::string& useMtlName, Texture* tex);

public:
	Vector3 pos;
	Vector3 rotate;
	Vector3 scale;

	uint32_t color;
	Model* parent;

	Mesh::DirectionLight light;

private:
	Mesh* mesh;

	std::unordered_map<std::string, Mesh::CopyData> data;

	bool isLoadObj;

	ConstBuffer<Mesh::MatrixData> wvpData;

	ConstBuffer<Mesh::DirectionLight> dirLig;

	ConstBuffer<Vector4> colorBuf;
};