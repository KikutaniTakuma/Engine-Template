#pragma once
#include "Utils/Math/Vector3.h"
#include "Utils/Math/Mat4x4.h"
#include "Utils/Math/Vector2.h"
#include "Utils/Math/Vector4.h"
#include <string>
#include "Engine/ConstBuffer/ConstBuffer.h"
#include "Engine/ShaderResource/ShaderResourceHeap.h"
#include "Engine/RootSignature/RootSignature.h"
#include "TextureManager/TextureManager.h"
#include "Engine/ShaderManager/ShaderManager.h"
#include <unordered_map>
#include "MeshManager/Mesh/Mesh.h"

class Pipeline;

#include <wrl.h>

class Model {
private:
	struct MatrixData {
		Mat4x4 worldMat;
		Mat4x4 viewProjectoionMat;
	};

	struct DirectionLight {
		Vector3 ligDirection;
		float pad0;
		Vector3 ligColor;
		float pad1;
		Vector3 eyePos;
		float pad2;
		Vector3 ptPos;
		float pad3;
		Vector3 ptColor;
		float ptRange;
	};

public:
	Model();
	Model(const Model&) = default;
	~Model();

	Model& operator=(const Model&) = default;

public:
	void LoadObj(const std::string& fileName);

public:
	void LoadShader(const std::string& vertex = "./Resources/Shaders/ModelShader/Model.VS.hlsl",
		const std::string& pixel = "./Resources/Shaders/ModelShader/Model.PS.hlsl",
		const std::string& geometory = "./Resources/Shaders/ModelShader/ModelNone.GS.hlsl",
		const std::string& hull = {},
		const std::string& domain = {}
	);

	void Update();

	void Draw(const Mat4x4& viewProjectionMat, const Vector3& cameraPos);

	void CreateGraphicsPipeline();

	void Debug(const std::string& guiName);

	void SetParent(Model* parent_) {
		parent = parent_;
	}

public:
	Vector3 pos;
	Vector3 rotate;
	Vector3 scale;

	uint32_t color;
	Model* parent;


private:
	Mesh* mesh;

	std::unordered_map<std::string, Mesh::CopyData> data;

	Shader shader;

	Pipeline* pipeline;

	bool loadObjFlg;
	bool loadShaderFlg;
	bool createGPFlg;

	ConstBuffer<MatrixData> wvpData;

	ConstBuffer<DirectionLight> dirLig;

	ConstBuffer<Vector4> colorBuf;
};