#include "Player.h"
#include <cassert>
#include "ImguiWrapper.h"

void Player::Initialize(Input* input, ModelManager::ModelData* modelPlayer, ModelManager::ModelData* modelPlayerBullet) {
	input_ = input;

	// NULLポインタチェック
	assert(modelPlayer);
	assert(modelPlayerBullet);

	// プレイヤー本体の生成
	objectPlayer_ = std::make_unique<Object3D>();
	objectPlayer_->model_ = modelPlayer;
	// プレイヤーの弾モデルをセット
	modelPlayerBullet_ = modelPlayerBullet;

	// 3Dレティクルオブジェクトの生成
	object3DReticle_ = std::make_unique<Object3D>();
	object3DReticle_->model_ = modelPlayer; // なんでもいいので一旦入れておく
}

void Player::Update() {
	///
	///	3Dレティクルの配置
	/// 

	// 自機のワールド座標から3Dレティクルのワールド座標を計算
	// 自機から3Dレティクルへの距離
	const float kDistancePlayerTo3DReticle = 50.0f;
	// 自機から3Dレティクルへのオフセット（Z+向き）
	Float3 offset = {0, 0, 1.0f};
	// 自機のワールド行列の回転を反映
	offset = Float3::TransformNormal(offset, objectPlayer_->worldMatrix_);
	// ベクトルの長さを整える
	offset = Float3::Normalize(offset) * kDistancePlayerTo3DReticle;
	// 3Dレティクルの座標を設定
	object3DReticle_->transform_.translate = objectPlayer_->transform_.translate + offset;
	object3DReticle_->UpdateMatrix();

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
	///	弾の発射
	/// 

	Attack();

	// 弾の更新
	for (auto& bullet : bullets_) {
		bullet->Update();
	}

	// デスフラグの立った弾を削除
	bullets_.remove_if([](const std::unique_ptr<PlayerBullet>& bullet) { 
		return bullet->IsDead();
	});

	///
	///	行列を更新
	/// 
	
	objectPlayer_->UpdateMatrix();
}

void Player::Draw() {
	// プレイヤー本体の描画
	objectPlayer_->Draw();

	// 3Dレティクルを描画
	object3DReticle_->Draw();

	// 弾を描画
	for (auto& bullet : bullets_) {
		bullet->Draw();
	}

	// デバッグ表示
	Debug();
}

void Player::Attack() { 
	// 一旦スペースキーで弾を発射
	if (input_->TriggerKey(DIK_SPACE)) {
		// 自キャラの座標をコピー
		Float3 position = GetWorldPosition();

		// 弾の速度
		const float kBulletSpeed = 1.0f;
		Float3 velocity(0, 0, kBulletSpeed);

		// 自機から照準オブジェクトへのベクトル
		velocity = object3DReticle_->transform_.translate - objectPlayer_->transform_.translate;
		velocity = Float3::Normalize(velocity) * kBulletSpeed;

		// 速度ベクトルを自機の向きに合わせて回転させる
		velocity = Float3::TransformNormal(velocity, objectPlayer_->worldMatrix_);

		// 弾を生成し、初期化
		auto newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(modelPlayerBullet_, position, velocity);

		// 弾を登録する
		bullets_.push_back(std::move(newBullet));
	} 

}

void Player::Debug() { 
	ImGui::Begin("Player"); 

	ImGui::DragFloat3("translation", &objectPlayer_->transform_.translate.x, 0.1f);

	ImGui::End();
}

Float3 Player::GetWorldPosition() { 
	Float3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = objectPlayer_->worldMatrix_.r[3][0];
	worldPos.y = objectPlayer_->worldMatrix_.r[3][1];
	worldPos.z = objectPlayer_->worldMatrix_.r[3][2];

	return worldPos;
}
