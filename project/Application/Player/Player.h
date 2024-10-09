#pragma once
#include "ModelManager.h"
#include "Object3D.h"
#include "Input.h"

class Player {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Input* input, ModelManager::ModelData* modelPlayer);

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
	// 汎用機能
	Input* input_;

	// プレイヤー本体オブジェクト
	std::unique_ptr<Object3D> objectPlayer_;

};
