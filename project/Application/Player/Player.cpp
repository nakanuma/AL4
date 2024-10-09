#include "Player.h"
#include <cassert>
#include "ImguiWrapper.h"

void Player::Initialize(Input* input, ModelManager::ModelData* modelPlayer) {
	input_ = input;

	// NULLポインタチェック
	assert(modelPlayer);

	// プレイヤー本体の生成
	objectPlayer_ = std::make_unique<Object3D>();
	objectPlayer_->model_ = modelPlayer;
}

void Player::Update() {
	///
	///	移動処理
	/// 

	Float3 move = {0.0f, 0.0f, 0.0f}; // プレイヤーの移動ベクトル
	const float kCharacterSpeed = 0.2f; // プレイヤーの移動速度

	// 押したキーで移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_D)) {
		move.x += kCharacterSpeed;
	} 
	
	if (input_->PushKey(DIK_W)) {
		move.y += kCharacterSpeed;
	} else if (input_->PushKey(DIK_S)) { 
		move.y -= kCharacterSpeed;
	}

	// 移動ベクトルを加算して実際に移動
	objectPlayer_->transform_.translate += move;

	///
	///	移動制限
	///	

	// 移動限界座標
	const float kMoveLimitX = 18.0f;
	const float kMoveLimitY = 10.0f;

	// 範囲を超えないようにする
	objectPlayer_->transform_.translate.x = std::clamp(objectPlayer_->transform_.translate.x, -kMoveLimitX, kMoveLimitX);
	objectPlayer_->transform_.translate.y = std::clamp(objectPlayer_->transform_.translate.y, -kMoveLimitY, kMoveLimitY);

	///
	///	行列を更新
	/// 
	
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
