#include "Engine.h"
#include <cassert>
#include <format>
#include <filesystem>

#include "WinApp/WinApp.h"
#include "EngineParts/DirectXDevice/DirectXDevice.h"
#include "EngineParts/DirectXCommon/DirectXCommon.h"
#include "Engine/EngineParts/StringOutPutManager/StringOutPutManager.h"

#include "ShaderManager/ShaderManager.h"
#include "TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "PipelineManager/PipelineManager.h"
#include "MeshManager/MeshManager.h"

#include "Input/Input.h"
#include "ErrorCheck/ErrorCheck.h"
#include "FrameInfo/FrameInfo.h"
#include "Utils/ConvertString/ConvertString.h"
#include "Utils/ExecutionLog/ExecutionLog.h"

#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Line/Line.h"
#include "Drawers/Particle/Particle.h"

#include "Utils/Math/Vector3.h"
#include "Utils/Math/Mat4x4.h"
#include "Utils/Math/Vector2.h"
#include "Utils/Math/Vector4.h"

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wPram, LPARAM lPram);


#ifdef _DEBUG
Engine::Debug Engine::debugLayer;

Engine::Debug::Debug() :
	debugController(nullptr)
{}

Engine::Debug::~Debug() {
	debugController.Reset();

	// リソースリークチェック
	Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debug.GetAddressOf())))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
	}
}

///
/// デバッグレイヤー初期化
/// 

void Engine::Debug::InitializeDebugLayer() {
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf())))) {
		// デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		// さらにGPU側でもチェックするようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
}

#endif // _DEBUG

/// 
/// 各種初期化処理
/// 

Engine* Engine::engine = nullptr;

bool Engine::Initialize(const std::string& windowName, const Vector2& windowSize) {
	HRESULT hr =  CoInitializeEx(0, COINIT_MULTITHREADED);
	if (hr != S_OK) {
		ErrorCheck::GetInstance()->ErrorTextBox("CoInitializeEx failed", "Engine");
		return false;
	}

	engine = new Engine();
	assert(engine);
	engine->clientWidth = static_cast<int32_t>(windowSize.x);
	engine->clientHeight = static_cast<int32_t>(windowSize.y);

	const auto&& windowTitle = ConvertString(windowName);

	// Window生成
	WinApp::GetInstance()->Create(windowTitle, engine->clientWidth, engine->clientHeight);

#ifdef _DEBUG
	// DebugLayer有効化
	debugLayer.InitializeDebugLayer();
#endif

	// Direct3D生成
	engine->InitializeDirectXDevice();

	// ディスクリプタヒープ初期化
	DescriptorHeap::Initialize(4096);

	// DirectX12生成
	engine->InitializeDirectXCommon();

	if (!engine->InitializeDraw()) {
		ErrorCheck::GetInstance()->ErrorTextBox("Initialize() : InitializeDraw() Failed", "Engine");
		return false;
	}

	engine->InitializeDirectXTK();

	Input::Initialize();
	ShaderManager::Initialize();
	TextureManager::Initialize();
	AudioManager::Inititalize();
	PipelineManager::Initialize();
	MeshManager::Initialize();

	Texture2D::Initialize();
	Mesh::Initialize();
	Model::Initialize();
	Line::Initialize();
	Particle::Initialize();

	return true;
}

void Engine::Finalize() {
	engine->isFinalize = true;
	Particle::Finalize();
	Texture2D::Finalize();

	MeshManager::Finalize();
	PipelineManager::Finalize();
	AudioManager::Finalize();
	TextureManager::Finalize();
	ShaderManager::Finalize();
	Input::Finalize();

	StringOutPutManager::Finalize();

	DescriptorHeap::Finalize();

	delete engine;
	engine = nullptr;

	// COM 終了
	CoUninitialize();
}

bool Engine::IsFinalize() {
	return engine->isFinalize;
}





///
/// DirectXDevice
/// 

void Engine::InitializeDirectXDevice() {
	DirectXDevice::Initialize();
	directXDevice_ = DirectXDevice::GetInstance();
}




/// 
/// DirectXCommon
/// 

void Engine::InitializeDirectXCommon() {
	DirectXCommon::Initialize();
	directXCommon_ = DirectXCommon::GetInstance();
}


///
///  DirectXTK
///

void Engine::InitializeDirectXTK() {
	StringOutPutManager::Initialize();
	stringOutPutManager_ = StringOutPutManager::GetInstance();
}




///
/// 描画用
/// 
bool Engine::InitializeDraw() {
	static ID3D12Device* device = engine->directXDevice_->GetDevice();

	// DepthStencilTextureをウィンドウサイズで作成
	depthStencilResource = directXDevice_->CreateDepthStencilTextureResource(clientWidth, clientHeight);
	assert(depthStencilResource);
	if (!depthStencilResource) {
		assert(!"depthStencilResource failed");
		ErrorCheck::GetInstance()->ErrorTextBox("InitializeDraw() : DepthStencilResource Create Failed", "Engine");
		return false;
	}

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	dsvHeap = nullptr;
	if(!SUCCEEDED(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(dsvHeap.GetAddressOf())))) {
		assert(!"CreateDescriptorHeap failed");
		ErrorCheck::GetInstance()->ErrorTextBox("InitializeDraw() : CreateDescriptorHeap()  Failed", "Engine");
		return false;
	}


	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	device->CreateDepthStencilView(depthStencilResource.Get(), &dsvDesc, dsvHeap->GetCPUDescriptorHandleForHeapStart());

	return true;
}



/// 
/// MianLoop用
/// 

bool Engine::WindowMassage() {
	MSG msg{};

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	static auto err = ErrorCheck::GetInstance();

	return (msg.message != WM_QUIT) && !(err->GetError());
}

void Engine::FrameStart() {
	static FrameInfo* const frameInfo = FrameInfo::GetInstance();
	frameInfo->Start();

#ifdef _DEBUG
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif // _DEBUG

	engine->directXCommon_->ChangeBackBufferState();
	engine->directXCommon_->SetMainRenderTarget();
	engine->directXCommon_->ClearBackBuffer();

	// ビューポート
	engine->directXCommon_->SetViewPort(engine->clientWidth, engine->clientHeight);

	// SRV用のヒープ
	static auto srvDescriptorHeap = DescriptorHeap::GetInstance();

	srvDescriptorHeap->SetHeap();
}

void Engine::FrameEnd() {
	static auto err = ErrorCheck::GetInstance();
	if (err->GetError()) {
		return;
	}


#ifdef _DEBUG
	ID3D12GraphicsCommandList* commandList = engine->directXCommon_->GetCommandList();
	// ImGui描画
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
#endif // DEBUG

	
	engine->directXCommon_->ChangeBackBufferState();

	// コマンドリストを確定させる
	engine->directXCommon_->CloseCommandlist();

	// GPUにコマンドリストの実行を行わせる
	engine->directXCommon_->ExecuteCommandLists();


	// GPUとOSに画面の交換を行うように通知する
	engine->directXCommon_->SwapChainPresent();

	engine->stringOutPutManager_->GmemoryCommit();

	engine->directXCommon_->WaitForFinishCommnadlist();

	engine->directXCommon_->ResetCommandlist();
	
	// テクスチャの非同期読み込み
	auto textureManager = TextureManager::GetInstance();
	textureManager->ThreadLoadTexture();
	textureManager->ResetCommandList();

	// このフレームで画像読み込みが発生していたら開放する
	// またUnloadされていたらそれをコンテナから削除する
	textureManager->ReleaseIntermediateResource();

	// メッシュの非同期読み込み
	auto meshManager = MeshManager::GetInstance();
	meshManager->ThreadLoad();
	meshManager->JoinThread();
	meshManager->CheckLoadFinish();
	
	// 音の非同期読み込み
	auto audioManager = AudioManager::GetInstance();
	audioManager->ThreadLoad();
	audioManager->CheckThreadLoadFinish();

	static FrameInfo* const frameInfo = FrameInfo::GetInstance();
	frameInfo->End();
}





/// 
/// 各種解放処理
/// 
Engine::~Engine() {
	depthStencilResource->Release();

	DirectXCommon::Finalize();
	DirectXDevice::Finalize();
}