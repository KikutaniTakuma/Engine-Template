#pragma once
#include "TextureManager/TextureManager.h"
#include "Engine/Engine.h"
#include "Engine/ConstBuffer/ConstBuffer.h"
#include "Engine/PipelineManager/PipelineManager.h"
#include "Engine/StructuredBuffer/StructuredBuffer.h"
#include "Engine/ShaderResource/ShaderResourceHeap.h"

#include "Utils/Math/Vector3.h"
#include "Utils/Math/Mat4x4.h"
#include "Utils/Math/Vector2.h"
#include "Utils/Math/Vector4.h"

#include <array>

class Particle {
public:
	struct MatrixData {
		Mat4x4 wvpMat;
	};

	struct VertexData {
		Vector3 position;
		Vector2 uv;
	};

private:
	struct WorldTransForm {
		Vector2 scale;
		Vector3 rotate;
		Vector3 pos;
		bool isActive;
		
		Vector3 movePos;
	};

public:
	Particle() = delete;
	Particle(uint32_t indexNum);
	Particle(const Particle&);
	Particle(Particle&&) noexcept;
	~Particle();

	Particle& operator=(const Particle& right);
	Particle& operator=(Particle&& right) noexcept;

	/// <summary>
	/// 静的メンバ関数
	/// </summary>
public:
	static void Initialize(
		const std::string& vsFileName = "./Resources/Shaders/ParticleShader/Particle.VS.hlsl",
		const std::string& psFileName = "./Resources/Shaders/ParticleShader/Particle.PS.hlsl"
	);

	static void Finalize();

private:
	static void LoadShader(const std::string& vsFileName, const std::string& psFileName);

	static void CreateGraphicsPipeline();

	/// <summary>
	/// 静的メンバ変数
	/// </summary>
private:
	static std::array<Pipeline*, size_t(Pipeline::Blend::BlendTypeNum)> graphicsPipelineState;
	static Shader shader;

	static D3D12_INDEX_BUFFER_VIEW indexView;
	static Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;


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

	void Resize(uint32_t index) {
		wvpMat.Resize(index);
		srvHeap.CreateStructuredBufferView(wvpMat, 1);
		wtfs.resize(index);
		for (size_t i = 0; i < wtfs.size(); i++) {
			wtfs[i].scale = Vector2::identity * 512.0f;
			wtfs[i].pos.x = 10.0f * i;
			wtfs[i].pos.y = 10.0f * i;
			wtfs[i].pos.z += 0.3f;
		}
	}

public:
	std::vector<WorldTransForm> wtfs;

	Vector2 uvPibot;
	Vector2 uvSize;

	uint32_t color;

private:
	ShaderResourceHeap srvHeap;


	D3D12_VERTEX_BUFFER_VIEW vertexView;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;

	StructuredBuffer<Mat4x4> wvpMat;
	ConstBuffer<Vector4> colorBuf;

	Texture* tex;
	bool isLoad;

	// アニメーション変数
	std::chrono::steady_clock::time_point aniStartTime_;
	float aniCount_;
	bool isAnimation_;
public:
	float uvPibotSpd_;
};