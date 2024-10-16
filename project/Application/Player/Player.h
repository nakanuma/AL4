#pragma once
#include "ModelManager.h"
#include "Object3D.h"
#include "Input.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "DirectXBase.h"

#include <list>

#include "PlayerBullet.h"

class Player {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Input* input, ModelManager::ModelData* modelPlayer, ModelManager::ModelData* modelPlayerBullet);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI();

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

	/// <summary>
	/// デバッグ用（描画関数内で呼ぶ）
	/// </summary>
	void Debug();

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	/// <returns></returns>
	Float3 GetWorldPosition();

	/// <summary>
	/// 半径を取得
	/// </summary>
	/// <returns>プレイヤー半径</returns>
	float GetRadius() const { return kRadius_; }

private:
	// 汎用機能
	Input* input_;
	std::unique_ptr<SpriteCommon> spriteCommon = nullptr;

	// プレイヤー本体オブジェクト
	std::unique_ptr<Object3D> objectPlayer_;

	// 弾リスト
	std::list<std::unique_ptr<PlayerBullet>> bullets_;

	// 弾モデル
	ModelManager::ModelData* modelPlayerBullet_;

	// 半径
	const float kRadius_ = 1.0f;

	// 3Dレティクル用オブジェクト
	std::unique_ptr<Object3D> object3DReticle_;

	// 2Dレティクル用オブジェクト
	std::unique_ptr<Sprite> sprite2DReticle_;
};
