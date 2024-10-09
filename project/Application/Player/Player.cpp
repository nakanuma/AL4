#include "Player.h"
#include <cassert>
#include "ImguiWrapper.h"

void Player::Initialize(ModelManager::ModelData* modelPlayer) {
	// NULLポインタチェック
	assert(modelPlayer);

	// プレイヤー本体の生成
	objectPlayer_ = std::make_unique<Object3D>();
	objectPlayer_->model_ = modelPlayer;
}

void Player::Update() {
	// プレイヤー本体の行列を更新
	objectPlayer_->UpdateMatrix();
}

void Player::Draw() {
	// プレイヤー本体の描画
	objectPlayer_->Draw();

	// デバッグ表示
	Debug();
}

void Player::Debug() { 
	ImGui::Begin("Player"); 

	ImGui::DragFloat3("translation", &objectPlayer_->transform_.translate.x, 0.1f);

	ImGui::End();
}
