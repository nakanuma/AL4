#pragma once
#include "MyMath.h"
#include "ModelManager.h"
#include "Object3D.h"

class PlayerBullet {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelBullet">弾のモデルデータ</param>
	/// <param name="position">初期座標</param>
	/// <param name="velocity">速度</param>
	void Initialize(ModelManager::ModelData* modelBullet, const Float3& position, const Float3& velocity);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 死亡したか
	/// </summary>
	/// <returns>デスフラグ</returns>
	bool IsDead() const { return isDead_; }

	/// <summary>
	/// 衝突時コールバック
	/// </summary>
	void OnCollision();

	/// <summary>
	/// ワールド座標の取得
	/// </summary>
	/// <returns>ワールド座標</returns>
	Float3 GetWorldPosition();

	/// <summary>
	/// 半径の取得
	/// </summary>
	/// <returns></returns>
	float GetRadius() const { return kRadius_; }

private:
	// 弾オブジェクト
	std::unique_ptr<Object3D> objectBullet_;

	// 速度
	Float3 velocity_;

	// 寿命<frm>
	static const int32_t kLifeTime = 60 * 5;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_ = false;

	// 半径
	const float kRadius_ = 1.0f;
};
