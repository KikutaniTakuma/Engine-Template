#include "Model.h"
#include "Engine/Engine.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <numbers>
#include <filesystem>
#include "Utils/ConvertString/ConvertString.h"
#include "Engine/ShaderManager/ShaderManager.h"
#include "externals/imgui/imgui.h"
#include "Engine/ErrorCheck/ErrorCheck.h"
#include "Engine/PipelineManager/PipelineManager.h"
#include "MeshManager/MeshManager.h"

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
		D3D12_DESCRIPTOR_RANGE range{};
		range.NumDescriptors = 1;
		range.BaseShaderRegister = 0;
		range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		std::array<D3D12_ROOT_PARAMETER, 4> paramates = {};
		paramates[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		paramates[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		paramates[0].DescriptorTable.pDescriptorRanges = &range;
		paramates[0].DescriptorTable.NumDescriptorRanges = 1;

		paramates[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		paramates[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		paramates[1].Descriptor.ShaderRegister = 0;
		paramates[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		paramates[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		paramates[2].Descriptor.ShaderRegister = 1;
		paramates[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		paramates[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		paramates[3].Descriptor.ShaderRegister = 2;

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
	color(0xffffffff),
	parent(nullptr),
	mesh(nullptr),
	data(),
	loadObjFlg(false),
	wvpData(),
	dirLig(),
	colorBuf()
{

	wvpData.shaderRegister = 0;
	wvpData->worldMat = MakeMatrixIndentity();
	wvpData->viewProjectoionMat = MakeMatrixIndentity();


	dirLig.shaderRegister = 1;
	dirLig->ligDirection = { 1.0f,-1.0f,-1.0f };
	dirLig->ligDirection = dirLig->ligDirection.Normalize();
	Vector4 colorTmp = UintToVector4(0xffffadff);
	dirLig->ligColor = colorTmp.GetVector3();

	dirLig->ptPos = { 5.0f,5.0f,5.0f };
	dirLig->ptColor = { 15.0f,15.0f,15.0f };
	dirLig->ptRange = 10.0f;

	colorBuf.shaderRegister = 2;
	*colorBuf = UintToVector4(color);
}

void Model::LoadObj(const std::string& fileName) {
	if (!loadObjFlg) {
		mesh = MeshManager::GetInstance()->LoadObj(fileName);

		if (!mesh) {
			ErrorCheck::GetInstance()->ErrorTextBox("LoadObj : mesh is nullptr", "Model");
			return;
		}

		data = mesh->CreateResource();

		loadObjFlg = true;
	}
}

void Model::Update() {
	/*drawIndexNumber = 0;*/
}

void Model::Draw(const Mat4x4& viewProjectionMat, const Vector3& cameraPos) {
	assert(createGPFlg);
	if (loadObjFlg) {
		wvpData->worldMat.HoriAffin(scale, rotate, pos);
		if (parent) {
			wvpData->worldMat *= MakeMatrixTransepose(parent->wvpData->worldMat);
		}
		wvpData->worldMat.Transepose();
		wvpData->viewProjectoionMat = viewProjectionMat;

		*colorBuf = UintToVector4(color);

		dirLig->eyePos = cameraPos;


		auto commandlist = Engine::GetCommandList();

		if (!pipeline) {
			ErrorCheck::GetInstance()->ErrorTextBox("pipeline is nullptr", "Model");
			return;
		}

		[[maybe_unused]] size_t indexVertex = 0;

		for (auto& i : data) {
			pipeline->Use();
			i.second.tex->Use(0);


			commandlist->SetGraphicsRootConstantBufferView(1, wvpData.GetGPUVtlAdrs());
			commandlist->SetGraphicsRootConstantBufferView(2, dirLig.GetGPUVtlAdrs());
			commandlist->SetGraphicsRootConstantBufferView(3, colorBuf.GetGPUVtlAdrs());

			commandlist->IASetVertexBuffers(0, 1, &i.second.resource.second);
			commandlist->DrawInstanced(i.second.vertNum, 1, 0, 0);
		}
	}
}

void Model::Debug(const std::string& guiName) {
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
	ImGui::End();
}

Model::~Model() {
	for (auto& i : data) {
		if (i.second.resource.first) {
			i.second.resource.first->Release();
		}
	}
}