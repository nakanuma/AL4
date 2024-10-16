#include "GamePlayScene.h" 
#include "ImguiWrapper.h" 
#include "DirectXBase.h"
#include "SRVManager.h"
#include "SpriteCommon.h"

#include "GlobalVariables.h"

void GamePlayScene::Initialize()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// カメラのインスタンスを生成
	camera = std::make_unique<Camera>(Float3{0.0f, 0.0f, -50.0f}, Float3{0.0f, 0.0f, 0.0f}, 0.75f);
	Camera::Set(camera.get()); // 現在のカメラをセット

	// SpriteCommonの生成と初期化
	spriteCommon = std::make_unique<SpriteCommon>();
	spriteCommon->Initialize(DirectXBase::GetInstance());

	// TextureManagerの初期化
	TextureManager::Initialize(dxBase->GetDevice(), SRVManager::GetInstance());

	// SoundManagerの初期化
	soundManager = std::make_unique<SoundManager>();
	soundManager->Initialize();

	// Inputの初期化
	input = Input::GetInstance();

	// LightManagerの初期化
	lightManager = LightManager::GetInstance();
	lightManager->Initialize();
	lightManager->directionalLightCB_.data_->intensity = 2.5f;

	///
	///	↓ ゲームシーン用 
	///	
	
	// Texture読み込み
	uint32_t uvCheckerGH = TextureManager::Load("resources/Images/uvChecker.png", dxBase->GetDevice());

	int dummy = 0;
	GlobalVariables::getInstance()->addValue("Game","Dummy","dummy",dummy);


	/* プレイヤー関連 */

	// プレイヤーのモデルを読み込み
	modelPlayer_ = ModelManager::LoadModelFile("resources/Models", "cube.obj", dxBase->GetDevice());
	modelPlayer_.material.textureHandle = TextureManager::Load("resources/Images/uvChecker.png", dxBase->GetDevice());

	// プレイヤーの弾モデル読み込み
	modelPlayerBullet_ = ModelManager::LoadModelFile("resources/Models", "cube.obj", dxBase->GetDevice());
	modelPlayerBullet_.material.textureHandle = TextureManager::Load("resources/Images/monsterBall.png", dxBase->GetDevice());

	// プレイヤー本体の生成と初期化
	player_ = std::make_unique<Player>();
	player_->Initialize(input, &modelPlayer_, &modelPlayerBullet_);

	// プレイヤーとカメラの親子関係を結ぶ
	player_->SetParent(&camera->transform);

	/* レールカメラ */

	// スプライン曲線制御点（通過点）を設定
	controlPoints_ = {
		{0.0f,  0.0f,  0.0f},
		{10.0f,  5.0f,  10.0f},
		{20.0f,  10.0f, 20.0f},
		{20.0f,  20.0f, 15.0f},
		{10.0f,  20.0f, 30.0f},
		{10.0f,  10.0f, 25.0f},
	};

	// 制御点用の球を読み込み
	modelSphere_ = ModelManager::LoadModelFile("resources/Models", "sphere.obj", dxBase->GetDevice());
	modelSphere_.material.textureHandle = uvCheckerGH;

	// 制御点に対応する球体オブジェクトを生成
	for (const auto& point : controlPoints_) {
		auto sphere = std::make_unique<Object3D>();
		sphere->model_ = &modelSphere_;
		sphere->transform_.translate = point;
		sphere->transform_.scale = { 0.5f, 0.5f, 0.5f };
		objectSpheres_.push_back(std::move(sphere));
	}

	/* 天球 */

	// 天球のモデル読み込みと生成
	modelSkydome_ = ModelManager::LoadModelFile("resources/Models", "skydome.obj", dxBase->GetDevice());
	modelSkydome_.material.textureHandle = uvCheckerGH; // 一時的にuvCheckerを設定

	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(&modelSkydome_);
}

void GamePlayScene::Finalize()
{
}

void GamePlayScene::Update() { 
	/* プレイヤー関連 */

	// プレイヤーの更新
	player_->Update();

	// 天球の更新
	skydome_->Update();

	// レールカメラ更新
	UpdateRailCamera();

	// 制御点オブジェクトの更新
	for(auto& sphere : objectSpheres_){
		sphere->UpdateMatrix();
	}
}

void GamePlayScene::Draw()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();
	SRVManager* srvManager = SRVManager::GetInstance();

	// 描画前処理
	dxBase->PreDraw();
	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvManager->descriptorHeap.heap_.Get() };
	dxBase->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
	// ImGuiのフレーム開始処理
	ImguiWrapper::NewFrame();
	// カメラの定数バッファを設定
	Camera::TransferConstantBuffer();
	// ライトの定数バッファを設定
	lightManager->TransferContantBuffer();

	///
	///	↓ ここから3Dオブジェクトの描画コマンド
	/// 

	/* プレイヤー関連 */

	// プレイヤーの描画
	player_->Draw();

	/* その他 */

	// 天球の描画
	skydome_->Draw();

	// 制御点の描画
	for (auto& sphere : objectSpheres_) {
		sphere->Draw();
	}

	///
	///	↑ ここまで3Dオブジェクトの描画コマンド
	/// 

	// Spriteの描画準備。全ての描画に共通のグラフィックスコマンドを積む
	spriteCommon->PreDraw();

	///
	/// ↓ ここからスプライトの描画コマンド
	/// 

	// プレイヤーUIの描画
	player_->DrawUI();

	///
	/// ↑ ここまでスプライトの描画コマンド
	/// 
	
	// デバッグ表示
	Debug();

#ifdef _DEBUG
	GlobalVariables::getInstance()->Update();
#endif // _DEBUG
	// ImGuiの内部コマンドを生成する
	ImguiWrapper::Render(dxBase->GetCommandList());
	// 描画後処理
	dxBase->PostDraw();
	// フレーム終了処理
	dxBase->EndFrame();
}

void GamePlayScene::Debug() {
	ImGui::Begin("GameScene"); 

	ImGui::Text("%.2f", ImGui::GetIO().Framerate);

	ImGui::DragFloat3("camera.translate", &camera->transform.translate.x, 0.1f);
	ImGui::DragFloat3("camera.rotate", &camera->transform.rotate.x, 0.01f);

	ImGui::Checkbox("railcamera.stop", &stop);
	ImGui::DragFloat("timer", &t);

	ImGui::End();
}

void GamePlayScene::UpdateRailCamera()
{
	if (!stop) {
		t += 0.001f;
	}
	if (t > 1.0f) t = 1.0f; // スプラインの終端で止まるようにする

	// 視点（カメラのある位置）
	Float3 eye = Float3::CatmullRomPosition(controlPoints_, t);
	camera->transform.translate = eye;

	// 少し先の位置を計算して、注視点とする
	float nextT = t + 0.05f;
	if (nextT > 1.0f) nextT = 1.0f; // 範囲の制限

	Float3 target = Float3::CatmullRomPosition(controlPoints_, nextT);

	// 差分ベクトルから回転角を計算する
	Float3 forward = target - eye;
	if (forward.z == 0) { // 0除算を回避
		forward.z = 0.001f;
	}

	float yaw = atan2f(forward.x, forward.z); // Y軸周り
	float pitch = atan2f(-forward.y, sqrtf(forward.x * forward.x + forward.z * forward.z)); // X軸周り
	float roll = 0.0f; // Z軸周り

	camera->transform.rotate = { pitch, yaw, roll };
}
