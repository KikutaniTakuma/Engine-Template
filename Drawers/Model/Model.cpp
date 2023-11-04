#include "Model.h"
#include <cassert>
#include <climits>
#include "Utils/ConvertString/ConvertString.h"
#include "Utils/UtilsLib/UtilsLib.h"
#include "Engine/ShaderResource/ShaderResourceHeap.h"
#include "externals/imgui/imgui.h"
#include "Engine/ErrorCheck/ErrorCheck.h"
#include "Engine/PipelineManager/PipelineManager.h"
#include "MeshManager/MeshManager.h"
#undef max
#undef min

Shader Model::shader = {};

Pipeline* Model::pipeline = {};
bool Model::loadShaderFlg = false;
bool Model::createGPFlg = false;

void Model::Initialize(
	const std::string& vertex,
	const std::string& pixel,
	const std::string& geometory,
	const std::string& hull,
	const std::string& domain
) {
	LoadShader(vertex, pixel, geometory, hull, domain);
	CreateGraphicsPipeline();
}

void Model::LoadShader(
	const std::string& vertex,
	const std::string& pixel,
	const std::string& geometory,
	const std::string& hull,
	const std::string& domain
) {
	if (!loadShaderFlg) {
		shader.vertex = ShaderManager::LoadVertexShader(vertex);
		shader.pixel = ShaderManager::LoadPixelShader(pixel);
		if (geometory.size() != 0LLU) {
			shader.geometory = ShaderManager::LoadGeometoryShader(geometory);
		}
		if (hull.size() != 0LLU && geometory.size() != 0LLU) {
			shader.hull = ShaderManager::LoadHullShader(hull);
			shader.domain = ShaderManager::LoadHullShader(domain);
		}
		loadShaderFlg = true;
	}
}

void Model::CreateGraphicsPipeline() {
	if (loadShaderFlg) {
		std::array<D3D12_DESCRIPTOR_RANGE, 1> range = {};
		range[0].NumDescriptors = 1;
		range[0].BaseShaderRegister = 0;
		range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		std::array<D3D12_DESCRIPTOR_RANGE, 1> rangeCBV = {};
		rangeCBV[0].NumDescriptors = 3;
		rangeCBV[0].BaseShaderRegister = 0;
		rangeCBV[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		rangeCBV[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;

		std::array<D3D12_ROOT_PARAMETER, 2> paramates = {};
		paramates[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		paramates[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		paramates[0].DescriptorTable.pDescriptorRanges = range.data();
		paramates[0].DescriptorTable.NumDescriptorRanges = UINT(range.size());

		paramates[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		paramates[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		paramates[1].DescriptorTable.pDescriptorRanges = rangeCBV.data();
		paramates[1].DescriptorTable.NumDescriptorRanges = UINT(rangeCBV.size());

		PipelineManager::CreateRootSgnature(paramates.data(), paramates.size(), true);

		PipelineManager::SetShader(shader);

		PipelineManager::SetVertexInput("POSITION", 0u, DXGI_FORMAT_R32G32B32A32_FLOAT);
		PipelineManager::SetVertexInput("NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT);
		PipelineManager::SetVertexInput("TEXCOORD", 0u, DXGI_FORMAT_R32G32_FLOAT);

		PipelineManager::SetState(Pipeline::Blend::None, Pipeline::SolidState::Solid);

		pipeline = PipelineManager::Create();

		PipelineManager::StateReset();

		createGPFlg = true;
	}
}

Model::Model() :
	pos(),
	rotate(),
	scale(Vector3::identity),
	color(std::numeric_limits<uint32_t>::max()),
	parent(nullptr),
	mesh(nullptr),
	data(),
	isLoadObj(false),
	wvpData(),
	dirLig(),
	colorBuf()
{

	wvpData.shaderRegister = 0;
	wvpData->worldMat = Mat4x4::kIdentity_;
	wvpData->viewProjectoionMat = Mat4x4::kIdentity_;


	dirLig.shaderRegister = 1;
	light.ligDirection = { 1.0f,-1.0f,-1.0f };
	light.ligDirection = light.ligDirection.Normalize();
	light.ligColor = UintToVector4(0xffffadff).GetVector3();

	light.ptPos = Vector3::zero;
	light.ptColor = Vector3::zero;
	light.ptRange = std::numeric_limits<float>::max();

	*dirLig = light;

	colorBuf.shaderRegister = 2;
	*colorBuf = UintToVector4(color);

	auto descriptorHeap = ShaderResourceHeap::GetInstance();
	descriptorHeap->BookingHeapPos(3u);
	descriptorHeap->CreateConstBufferView(wvpData);
	descriptorHeap->CreateConstBufferView(dirLig);
	descriptorHeap->CreateConstBufferView(colorBuf);
}

Model::Model(const std::string& fileName) :
	Model()
{
	this->LoadObj(fileName);
}

Model::Model(const Model& right) :
	Model()
{
	*this = right;
}
Model::Model(Model&& right) noexcept :
	Model()
{
	*this = std::move(right);
}

Model& Model::operator=(const Model& right) {
	pos = right.pos;
	rotate = right.rotate;
	scale = right.scale;
	color = right.color;
	parent = right.parent;

	// 自身がロード済みだったらResourceを解放する
	if (isLoadObj) {
		data.clear();
	}

	// rightがロード済みだったら
	if (right.isLoadObj) {
		mesh = right.mesh;

		if (!mesh) {
			ErrorCheck::GetInstance()->ErrorTextBox("operator=() : right mesh is nullptr", "Model");
			return *this;
		}

		data = mesh->CopyBuffer();

		isLoadObj = true;
	}

	light = right.light;

	// 定数バッファの値をコピー
	*wvpData = *right.wvpData;
	*dirLig = *right.dirLig;
	*colorBuf = *right.colorBuf;

	return *this;
}

Model& Model::operator=(Model&& right) noexcept {
	pos = std::move(right.pos);
	rotate = std::move(right.rotate);
	scale = std::move(right.scale);
	color = std::move(right.color);
	parent = std::move(right.parent);

	// 自身がロード済みだったらResourceを解放する
	if (isLoadObj) {
		data.clear();
	}

	// rightがロード済みだったら
	if (right.isLoadObj) {
		mesh = right.mesh;

		if (!mesh) {
			ErrorCheck::GetInstance()->ErrorTextBox("operator=() : right mesh is nullptr", "Model");
			return *this;
		}

		data = mesh->CopyBuffer();

		isLoadObj = true;
	}

	light = std::move(right.light);

	// 定数バッファの値をコピー
	*wvpData = *right.wvpData;
	*dirLig = *right.dirLig;
	*colorBuf = *right.colorBuf;

	return *this;
}

void Model::LoadObj(const std::string& fileName) {
	if (!isLoadObj) {
		mesh = MeshManager::GetInstance()->LoadObj(fileName);

		if (!mesh) {
			ErrorCheck::GetInstance()->ErrorTextBox("LoadObj : mesh is nullptr", "Model");
			return;
		}

		isLoadObj = true;
	}
}

void Model::ChangeTexture(const std::string& useMtlName, const std::string& texName) {
	data[useMtlName].tex = TextureManager::GetInstance()->LoadTexture(texName);
	assert(data[useMtlName].tex->GetFileName() == texName);
}

void Model::ChangeTexture(const std::string& useMtlName, Texture* tex) {
	assert(tex != nullptr);
	data[useMtlName].tex = tex;
}

void Model::MeshChangeTexture(const std::string& useMtlName, const std::string& texName) {
	mesh->ChangeTexture(useMtlName, texName);
}

void Model::MeshChangeTexture(const std::string& useMtlName, Texture* tex) {
	assert(tex != nullptr);
	mesh->ChangeTexture(useMtlName, tex);
}

void Model::Update() {
	*dirLig = light;
}

void Model::Draw(const Mat4x4& viewProjectionMat, const Vector3& cameraPos) {
	assert(createGPFlg);
	if (isLoadObj) {
		if (data.empty()) {
			data = mesh->CopyBuffer();
		}

		wvpData->worldMat.Affin(scale, rotate, pos);
		if (parent) {
			wvpData->worldMat *= parent->wvpData->worldMat;
		}
		wvpData->viewProjectoionMat = viewProjectionMat;

		*colorBuf = UintToVector4(color);

		light.eyePos = cameraPos;
		dirLig->eyePos = cameraPos;

		auto commandlist = Direct12::GetInstance()->GetCommandList();

		if (!pipeline) {
			ErrorCheck::GetInstance()->ErrorTextBox("pipeline is nullptr", "Model");
			return;
		}

		for (auto& i : data) {
			pipeline->Use();
			i.second.tex->Use(0);

			commandlist->SetGraphicsRootDescriptorTable(1, wvpData.GetViewHandle());

			commandlist->IASetVertexBuffers(0, 1, &i.second.view);
			commandlist->DrawInstanced(i.second.vertNum, 1, 0, 0);
		}
	}
}

void Model::InstancingDraw(const Mat4x4& viewProjectionMat, const Vector3& cameraPos) {
	if (isLoadObj) {
		light.eyePos = cameraPos;

		mesh->Use(
			MakeMatrixAffin(scale, rotate, pos),
			viewProjectionMat,
			light,
			UintToVector4(color)
			);
	}
}

void Model::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat3("pos", &pos.x, 0.01f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.01f);
	ImGui::DragFloat3("scale", &scale.x, 0.01f);
	ImGui::ColorEdit4("SphereColor", &colorBuf->color.r);
	ImGui::DragFloat3("ligDirection", &dirLig->ligDirection.x, 0.01f);
	dirLig->ligDirection = dirLig->ligDirection.Normalize();
	ImGui::DragFloat3("ligColor", &dirLig->ligColor.x, 0.01f);
	ImGui::DragFloat3("ptPos", &dirLig->ptPos.x, 0.01f);
	ImGui::DragFloat3("ptColor", &dirLig->ptColor.x, 0.01f);
	ImGui::DragFloat("ptRange", &dirLig->ptRange);


	if (ImGui::TreeNode("LoadObj")) {
		auto filePathes = UtilsLib::GetFilePathFormDir("./Resources/", ".obj");

		for (auto& path : filePathes) {
			if (ImGui::Button(path.string().c_str())) {
				isLoadObj = false;
				LoadObj(path.string());
				break;
			}
		}

		ImGui::TreePop();
	}
	ImGui::End();
#endif // _DEBUG
}

Model::~Model() {
	
}