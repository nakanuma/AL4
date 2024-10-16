#pragma once
#include "BaseScene.h"
#include "Camera.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "Sprite.h"
#include "ModelManager.h"
#include "Object3D.h"
#include "SoundManager.h"
#include "Input.h"
#include "LightManager.h"

#include "Player.h"
#include "Skydome.h"

// ゲームプレイシーン
class GamePlayScene : public BaseScene
{
public:
	// 初期化
	void Initialize() override;

	// 終了
	void Finalize() override;

	// 毎フレーム更新
	void Update() override;

	// 描画
	void Draw() override;

	// デバッグ表示
	void Debug();

	// レールカメラ更新
	void UpdateRailCamera();

private:
	std::unique_ptr<Camera> camera = nullptr;
	std::unique_ptr<SpriteCommon> spriteCommon = nullptr;
	std::unique_ptr<SoundManager> soundManager = nullptr;
	Input* input = nullptr;
	LightManager* lightManager = nullptr;

	///
	/// ↓ ゲームシーン用
	///

	/* プレイヤー関連 */

	// プレイヤーモデル
	ModelManager::ModelData modelPlayer_;
	// プレイヤー弾モデル
	ModelManager::ModelData modelPlayerBullet_;

	// プレイヤー本体
	std::unique_ptr<Player> player_;

	/* その他 */

	// レールの制御点
	std::vector<Float3> controlPoints_;
	// 制御点描画用の球モデル
	ModelManager::ModelData modelSphere_;
	// 制御点描画用のオブジェクト
	std::list<std::unique_ptr<Object3D>> objectSpheres_;

	bool stop;
	float t = 0.0f;

	// 天球
	std::unique_ptr<Skydome> skydome_;
	// 天球モデル
	ModelManager::ModelData modelSkydome_;
};

