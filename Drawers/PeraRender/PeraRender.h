#pragma once
#include <string>
#include <array>

#include "Engine/ConstBuffer/ConstBuffer.h"
#include "Engine/RenderTarget/RenderTarget.h"
#include "Engine/ShaderManager/ShaderManager.h"
#include "Engine/PipelineManager/PipelineManager.h"

#include "Utils/Math/Vector3.h"
#include "Utils/Math/Vector2.h"
#include "Utils/Math/Mat4x4.h"

class PeraRender {
public:
	struct PeraVertexData {
		Vector3 position;
		Vector2 uv;
	};

	struct Wipe {
		Vector2 center;
		float wipeSize;
	};

public:
	PeraRender();
	PeraRender(uint32_t width_, uint32_t height_);
	~PeraRender();

	PeraRender(const PeraRender&) = delete;
	PeraRender(PeraRender&&) = delete;
	PeraRender& operator=(const PeraRender&) = delete;
	PeraRender& operator=(PeraRender&&) = delete;

public:
	void Initialize(const std::string& vsFileName, const std::string& psFileName);

private:
	void CreateShader(const std::string& vsFileName, const std::string& psFileName);

	void CreateGraphicsPipeline();

public:
	void Update();

	void PreDraw();

	void Draw(const Mat4x4& viewProjection, Pipeline::Blend blend, PeraRender* pera = nullptr);


public:
	Vector3 pos_;
	Vector3 rotate_;
	Vector3 scale_;

	Vector2 uvPibot_;
	Vector2 uvSize_;

	std::array<Vector3, 4> worldPos_;

	uint32_t color_;

private:
	RenderTarget render_;

	ConstBuffer<Mat4x4> wvpMat_;
	ConstBuffer<Vector4> colorBuf_;

	bool isPreDraw_;

	D3D12_VERTEX_BUFFER_VIEW peraVertexView_;
	Microsoft::WRL::ComPtr<ID3D12Resource> peraVertexResource_ = nullptr;
	Shader shader_;

	D3D12_INDEX_BUFFER_VIEW indexView_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;

	std::array<class Pipeline*, Pipeline::Blend::BlendTypeNum> piplines_;
};