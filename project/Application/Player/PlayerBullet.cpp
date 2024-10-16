#include "PlayerBullet.h"
#include <cassert>

void PlayerBullet::Initialize(ModelManager::ModelData* modelBullet, const Float3& position, const Float3& velocity) {
	// NULLポインタチェック
	assert(modelBullet);

	// 弾オブジェクトの生成
	objectBullet_ = std::make_unique<Object3D>();
	objectBullet_->model_ = modelBullet;

	// 弾の初期位置を設定
	objectBullet_->transform_.translate = position;

	// 弾の速度を設定
	velocity_ = velocity;
}

void PlayerBullet::Update() {
	// 移動処理
	objectBullet_->transform_.translate += velocity_;

	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	// 行列の更新
	objectBullet_->UpdateMatrix();
}

void PlayerBullet::Draw() {
	// モデルの描画
	objectBullet_->Draw();
}

void PlayerBullet::OnCollision() {
	// デスフラグを立てる
	isDead_ = true;
}

Float3 PlayerBullet::GetWorldPosition() { 
	Float3 worldPos;
	// ワールド行列の平行移動成分を取得
	worldPos.x = objectBullet_->worldMatrix_.r[3][0];
	worldPos.x = objectBullet_->worldMatrix_.r[3][1];
	worldPos.x = objectBullet_->worldMatrix_.r[3][2];

	return worldPos;
}
