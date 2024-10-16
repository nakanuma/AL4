#include "Player.h"
#include "Camera.h"
#include "ImguiWrapper.h"
#include "MyWindow.h"
#include <cassert>

void Player::Initialize(Input* input, ModelManager::ModelData* modelPlayer, ModelManager::ModelData* modelPlayerBullet) {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	// inputを引数から受けとる
	input_ = input;
	// SpriteCommonの生成と初期化
	spriteCommon = std::make_unique<SpriteCommon>();
	spriteCommon->Initialize(DirectXBase::GetInstance());

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

	// 2Dレティクルスプライトの生成
	uint32_t reticleGH = TextureManager::Load("resources/Images/reticle.png", dxBase->GetDevice());
	sprite2DReticle_ = std::make_unique<Sprite>();
	sprite2DReticle_->Initialize(spriteCommon.get(), reticleGH);
	sprite2DReticle_->SetAnchorPoint({0.5f, 0.5f}); // アンカーポイントを中心に設定
	sprite2DReticle_->SetSize({128.0f, 128.0f});
}

void Player::Update() {
	///
	///	マウスカーソルのスクリーン座標からワールド座標を取得して3Dレティクルを配置
	///

	Set3DReticleFromCursor();

	///
	///	移動処理
	///

	Float3 move = {0.0f, 0.0f, 0.0f};   // プレイヤーの移動ベクトル
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
	bullets_.remove_if([](const std::unique_ptr<PlayerBullet>& bullet) { return bullet->IsDead(); });

	///
	///	行列を更新
	///

	objectPlayer_->UpdateMatrix();
}

void Player::Draw() {
	// プレイヤー本体の描画
	objectPlayer_->Draw();

	// 3Dレティクルを描画
	/*object3DReticle_->Draw();*/

	// 弾を描画
	for (auto& bullet : bullets_) {
		bullet->Draw();
	}

	// デバッグ表示
	Debug();
}

void Player::DrawUI() {
	// 2Dレティクルを描画
	sprite2DReticle_->Draw();
}

void Player::Attack() {
	// 左クリックを押した瞬間のみ発射
	if (input_->IsTriggerMouse(0)) {
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

void Player::Set3DReticleFromCursor() {
	// マウス座標を取得する
	POINT mousePosition;
	GetCursorPos(&mousePosition);

	// クライアントエリア座標に変換する
	HWND hwnd = Window::GetHandle();
	ScreenToClient(hwnd, &mousePosition);

	// マウス座標を2Dレティクルのスプライトに代入する
	sprite2DReticle_->SetPosition(Float2(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y)));

	// ビュー行列
	Matrix viewMatrix = Camera::GetCurrent()->MakeViewMatrix();
	// プロジェクション行列
	Matrix projectionMatrix = Camera::GetCurrent()->MakePerspectiveFovMatrix();
	// ビューポート行列
	Matrix matViewport = Matrix::MakeViewportMatrix(0, 0, static_cast<float>(Window::GetWidth()), static_cast<float>(Window::GetHeight()), 0, 1);

	// ビュープロジェクションビューポート合成行列
	Matrix matVPV = viewMatrix * projectionMatrix * matViewport;
	// 合成行列の逆行列を計算する
	Matrix matInverseVPV = Matrix::Inverse(matVPV);

	// スクリーン座標
	Float3 posNear = Float3(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y), 0);
	Float3 posFar = Float3(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y), 1);
	// スクリーン座標系からワールド座標系へ
	posNear = Float3::Transform(posNear, matInverseVPV);
	posFar = Float3::Transform(posFar, matInverseVPV);

	// マウスレイの方向
	Float3 mouseDirection = posFar - posNear;
	// ベクトルの正規化
	mouseDirection = Float3::Normalize(mouseDirection);
	// カメラから照準オブジェクトの距離
	const float kDistanceTestObject = 100.0f; // レールカメラ実装でずれる可能性あるので調整必須かも
	object3DReticle_->transform_.translate = posNear + mouseDirection * kDistanceTestObject;
	object3DReticle_->UpdateMatrix();

	// 2Dスプライトの更新
	sprite2DReticle_->Update();
}
