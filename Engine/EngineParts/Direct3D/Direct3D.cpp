#include "Direct3D.h"
#include "Utils/Log/Log.h"
#include "Engine/ErrorCheck/ErrorCheck.h"
#include "Utils/ConvertString/ConvertString.h"
#include <cassert>
#include <format>

Direct3D* Direct3D::instance_ = nullptr;

Direct3D* Direct3D::GetInstance() {
	return instance_;
}

void Direct3D::Initialize() {
	instance_ = new Direct3D{};
}
void Direct3D::Finalize() {
	delete instance_;
	instance_ = nullptr;
}


Direct3D::Direct3D():
	incrementSRVCBVUAVHeap_(0u),
	incrementRTVHeap_(0u),
	incrementDSVHeap_(0u),
	incrementSAMPLER_(0u),
	device_{},
	dxgiFactory_{},
	useAdapter_{}
{
	// IDXGIFactory生成
	auto hr = CreateDXGIFactory(IID_PPV_ARGS(dxgiFactory_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("InitializeDirect3D() : CreateDXGIFactory() Failed", "Engine");
		return;
	}

	// 使用するグラボの設定
	useAdapter_ = nullptr;
	for (UINT i = 0;
		dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(useAdapter_.GetAddressOf())) != DXGI_ERROR_NOT_FOUND;
		++i) {

		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter_->GetDesc3(&adapterDesc);
		if (hr != S_OK) {
			ErrorCheck::GetInstance()->ErrorTextBox("InitializeDirect3D() : GetDesc3() Failed", "Engine");
			return;
		}

		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			Log::AddLog(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter_.Reset();
	}
	if (useAdapter_ == nullptr) {
		ErrorCheck::GetInstance()->ErrorTextBox("InitializeDirect3D() : GPU not Found", "Engine");
		return;
	}


	// Deviceの初期化
	// 使用しているデバイスによってD3D_FEATURE_LEVELの対応バージョンが違うので成功するまでバージョンを変えて繰り返す
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
	};
	const char* featureLevelString[] = {
		"12.2", "12.1", "12.0"
	};

	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		hr = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(device_.GetAddressOf()));

		if (SUCCEEDED(hr)) {
			Log::AddLog(std::format("FeatureLevel:{}\n", featureLevelString[i]));
			break;
		}
	}

	if (device_ == nullptr) {
		return;
	}
	Log::AddLog("Complete create D3D12Device!!!\n");

#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		// やばいエラーの予期に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// エラーの時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		// 抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		// 指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);

		// 解放
		infoQueue->Release();
	}
#endif

	incrementSRVCBVUAVHeap_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	incrementRTVHeap_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	incrementDSVHeap_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	incrementSAMPLER_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
}
