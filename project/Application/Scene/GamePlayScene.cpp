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
	camera = std::make_unique<Camera>(Float3{0.0f, 0.0f, -50.0f}, Float3{0.0f, 0.0f, 0.0f}, 0.45f);
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
}

void GamePlayScene::Finalize()
{
}

void GamePlayScene::Update() { 
	/* プレイヤー関連 */

	// プレイヤーの更新
	player_->Update();
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
	

#ifdef _DEBUG
	GlobalVariables::getInstance()->Update();

	// デバッグ表示
	Debug();
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

	ImGui::DragFloat3("camera.translate", &camera->transform.translate.x, 0.01f);
	ImGui::DragFloat3("camera.rotate", &camera->transform.rotate.x, 0.01f);


	ImGui::End();
}
