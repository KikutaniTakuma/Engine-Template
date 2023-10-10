#include "Engine/Engine.h"
#include "externals/imgui/imgui.h"

#include "Engine/WinApp/WinApp.h"

#include "Engine/ErrorCheck/ErrorCheck.h"
#include "Engine/FrameInfo/FrameInfo.h"

#include "Input/Input.h"

#include "Utils/Camera/Camera.h"
#include "Drawers/Line/Line.h"
#include "Drawers/Model/Model.h"

#include "Game/Enemy/Enemy.h"
#include "SceneManager/GameScene/GameScene.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリ初期化
	if (!Engine::Initialize("DirectXGame", Engine::Resolution::HD)) {
		ErrorCheck::GetInstance()->ErrorTextBox(
			"InitializeDirect3D() : Engine::Initialize() Failed",
			"WinMain"
		);
		return -1;
	}

	// フォントロード
	Engine::LoadFont("./Resources/Font/fonttest.spritefont");

	static auto* const frameInfo = FrameInfo::GetInstance();

	auto input = Input::GetInstance();

	float fpsLimit = 165.0f;

	GameScene gameScene;
	gameScene.Initialize(nullptr);

	Camera camera;
	camera.pos.z = -5.0f;

	Collider test;
	Collider test2;


	/// 
	/// メインループ
	/// 
	while (Engine::WindowMassage()) {
		// 描画開始処理
		Engine::FrameStart();

		// fps
		ImGui::Begin("fps");
		ImGui::Text("Frame rate: %3.0lf fps", frameInfo->GetFps());
		ImGui::Text("Delta Time: %.4lf", frameInfo->GetDelta());
		ImGui::Text("Frame Count: %llu", frameInfo->GetFrameCount());
		ImGui::DragFloat("fps limit", &fpsLimit, 1.0f, 30.0f, 165.0f);
		ImGui::End();

		frameInfo->SetFpsLimit(static_cast<double>(fpsLimit));

		// 入力処理
		input->InputStart();

		/// 
		/// 更新処理
		/// 

		gameScene.Update();
		ImGui::Begin("test");
		ImGui::DragFloat3("pos", &test.collisionPos_.x, 0.01f);
		ImGui::End();

		ImGui::Begin("test2");
		ImGui::DragFloat3("pos", &test2.collisionPos_.x, 0.01f);
		ImGui::End();

		//if (input->GetInstance()->GetKey()->Pushed(DIK_L)) {
		//	test.Adjusment(test2, test.pos_ - test2.pos_);
		//}

		//test.UpdateCollision();

		//test2.UpdateCollision();

		///
		/// 更新処理ここまで
		/// 

		///
		/// 描画処理
		/// 
		//camera.Update();
		gameScene.Draw();
		/*test.DebugDraw(camera.GetViewProjection());
		test2.DebugDraw(camera.GetViewProjection());*/


		///
		/// 描画処理ここまで
		/// 


		// フレーム終了処理
		Engine::FrameEnd();

		// Escapeが押されたら終了
		if (input->GetKey()->Pushed(DIK_ESCAPE) || input->GetGamepad()->Pushed(Gamepad::Button::BACK)) {
			break;
		}
	}

	// ライブラリ終了
	Engine::Finalize();

	return 0;
}