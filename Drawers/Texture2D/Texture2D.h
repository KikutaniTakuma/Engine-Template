#pragma once
#include "TextureManager/TextureManager.h"
#include "Engine/Engine.h"
#include "Engine/ConstBuffer/ConstBuffer.h"
#include "Engine/ShaderResource/ShaderResourceHeap.h"
#include "Engine/PipelineManager/PipelineManager.h"

#include "Utils/Math/Vector3.h"
#include "Utils/Math/Mat4x4.h"
#include "Utils/Math/Vector2.h"
#include "Utils/Math/Vector4.h"

#include <array>

class Texture2D {
public:
	struct MatrixData {
		Mat4x4 wvpMat;
	};

	struct VertexData {
		Vector3 position;
		Vector2 uv;
	};

public:
	Texture2D();
	Texture2D(const Texture2D&);
	Texture2D(Texture2D&&) noexcept;
	~Texture2D();

	Texture2D& operator=(const Texture2D& right);
	Texture2D& operator=(Texture2D&& right) noexcept;


public:
	void Initialize(
		const std::string& vsFileName = "Shaders/Texture2DShader/Texture2D.VS.hlsl",
		const std::string& psFileName = "Shaders/Texture2DShader/Texture2DNone.PS.hlsl"
	);

private:
	void LoadShader(const std::string& vsFileName, const std::string& psFileName);

	void CreateGraphicsPipeline();

public:
	void LoadTexture(const std::string& fileName);
	void ThreadLoadTexture(const std::string& fileName);

public:
	void Update();

	void Draw(
		const Mat4x4& viewProjection,
		Pipeline::Blend blend = Pipeline::Blend::None
	);

	void Debug(const std::string& guiName);

	bool Colision(const Vector2& pos2D) const;
	bool Colision(const Texture2D& tex2D) const;

	Vector2 GetTexSize() const {
		if (tex) {
			return tex->getSize();
		}
		else {
			return Vector2::zero;
		}
	}

	/// <summary>
	/// アニメーションスタート関数
	/// </summary>
	/// <param name="aniUvPibot">アニメーションをスタートさせる場所</param>
	void AnimationStart(float aniUvPibot = 0.0f);

	/// <summary>
	/// アニメーション関数
	/// </summary>
	/// <param name="aniSpd">アニメーション速度(milliseconds)</param>
	/// <param name="isLoop">アニメーションをループさせるか</param>
	/// <param name="aniUvStart">アニメーションをスタートさせる場所</param>
	/// <param name="aniUvEnd">アニメーションを終わらせる場所</param>
	void Animation(size_t aniSpd, bool isLoop, float aniUvStart, float aniUvEnd);

	/// <summary>
	/// アニメーションを一時停止
	/// </summary>
	void AnimationPause();

	/// <summary>
	/// アニメーションを再スタート
	/// </summary>
	void AnimationRestart();

	/// <summary>
	/// アニメーションしているかを取得
	/// </summary>
	/// <returns>アニメーションフラグ</returns>
	bool GetIsAnimation()const {
		return isAnimation_;
	}

public:
	Vector2 scale;
	Vector3 rotate;
	Vector3 pos;

	Vector2 uvPibot;
	Vector2 uvSize;

	std::array<Vector3, 4> worldPos;

	uint32_t color;

private:
	ShaderResourceHeap SRVHeap;

	uint32_t SRVHandle;

	D3D12_VERTEX_BUFFER_VIEW vertexView;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;

	D3D12_INDEX_BUFFER_VIEW indexView;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;

	Shader shader;

	std::array<Pipeline*, size_t(Pipeline::Blend::BlendTypeNum)> graphicsPipelineState;

	ConstBuffer<Mat4x4> wvpMat;
	ConstBuffer<Vector4> colorBuf;

	Texture* tex;
	bool isFirstLoad;
	bool isLoad;


	// アニメーション変数
	std::chrono::steady_clock::time_point aniStartTime_;
	float aniCount_;
	bool isAnimation_;
public:
	float uvPibotSpd_;
};