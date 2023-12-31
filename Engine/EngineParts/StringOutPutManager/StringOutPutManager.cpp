#include "StringOutPutManager.h"
#include "Engine/EngineParts/DirectXDevice/DirectXDevice.h"
#include "Engine/EngineParts/DirectXCommon/DirectXCommon.h"
#include "Engine/ErrorCheck/ErrorCheck.h"
#include "Utils/ConvertString/ConvertString.h"
#include "Engine/Engine.h"
#include "Engine/DescriptorHeap/DescriptorHeap.h"
#include <cassert>
#include <filesystem>

StringOutPutManager* StringOutPutManager::instance_ = nullptr;

void StringOutPutManager::Initialize() {
	assert(!instance_);
	instance_ = new StringOutPutManager{};
	assert(!!instance_);
}

void StringOutPutManager::Finalize() {
	delete instance_;
	instance_ = nullptr;
}

StringOutPutManager* const StringOutPutManager::GetInstance() {
	return instance_;
}


StringOutPutManager::StringOutPutManager():
	gmemory_{},
	spriteFonts_{},
	spriteBatch_{}
{
	// GraphicsMemory初期化
	auto device = DirectXDevice::GetInstance()->GetDevice();
	gmemory_.reset(new DirectX::GraphicsMemory(device));
}

void StringOutPutManager::LoadFont(const std::string& fontName) {
	static ID3D12Device* device = DirectXDevice::GetInstance()->GetDevice();
	if (!std::filesystem::exists(std::filesystem::path(fontName))) {
		ErrorCheck::GetInstance()->ErrorTextBox("Engine::LoadFont() Failed : This file is not exist -> " + fontName, "Engine");
		return;
	}

	// もしロード済みなら早期リターン
	auto isLoaded = spriteBatch_.find(fontName);
	if (isLoaded != spriteBatch_.end()) {
		return;
	}

	DirectX::ResourceUploadBatch resUploadBach(device);
	resUploadBach.Begin();
	DirectX::RenderTargetState rtState(
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		DXGI_FORMAT_D32_FLOAT
	);

	DirectX::SpriteBatchPipelineStateDescription pd(rtState);

	// SpriteFontオブジェクトの初期化
	spriteBatch_.insert(
		std::make_pair(fontName, std::make_unique<DirectX::SpriteBatch>(device, resUploadBach, pd)));
	// ビューポート
	D3D12_VIEWPORT viewport{};
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = static_cast<float>(Engine::GetInstance()->clientWidth);
	viewport.Height = static_cast<float>(Engine::GetInstance()->clientHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	spriteBatch_[fontName]->SetViewport(viewport);

	auto descriptorHeap = DescriptorHeap::GetInstance();
	uint32_t useHandle = descriptorHeap->BookingHeapPos(1u);

	spriteFonts_.insert(
		std::make_pair(
			fontName,
			std::make_unique<DirectX::SpriteFont>(
				device,
				resUploadBach,
				ConvertString(fontName).c_str(),
				descriptorHeap->GetSrvCpuHeapHandle(useHandle),
				descriptorHeap->GetSrvGpuHeapHandle(useHandle)
			)
		)
	);

	descriptorHeap->UseThisPosition(useHandle);

	auto directXCommon = DirectXCommon::GetInstance();
	auto future = resUploadBach.End(directXCommon->GetCommandQueue());

	directXCommon->WaitForFinishCommnadlist();

	future.wait();
}

DirectX::SpriteFont* StringOutPutManager::GetFont(const std::string& fontName) {
	auto spriteFontItr = spriteFonts_.find(fontName);
	if (spriteFontItr != spriteFonts_.end()) {
		return spriteFonts_[fontName].get();
	}
	else {
		return nullptr;
	}
}

DirectX::SpriteBatch* StringOutPutManager::GetBatch(const std::string& fontName) {
	auto spriteBatchItr = spriteBatch_.find(fontName);
	if (spriteBatchItr != spriteBatch_.end()) {
		return spriteBatch_[fontName].get();
	}
	else {
		return nullptr;
	}
}

void StringOutPutManager::GmemoryCommit() {
	static auto directXCommon = DirectXCommon::GetInstance();
	gmemory_->Commit(directXCommon->GetCommandQueue());
}