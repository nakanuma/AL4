#pragma once
#include "ModelManager.h"
#include "Object3D.h"

class Player {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ModelManager::ModelData* modelPlayer);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// デバッグ用（描画関数内で呼ぶ）
	/// </summary>
	void Debug();

private:
	// プレイヤー本体オブジェクト
	std::unique_ptr<Object3D> objectPlayer_;

};
