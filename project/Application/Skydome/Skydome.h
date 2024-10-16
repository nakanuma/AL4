#pragma once
#include "ModelManager.h"
#include "Object3D.h"

class Skydome
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ModelManager::ModelData* modelSkydome);
	
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	// 天球オブジェクト
	std::unique_ptr<Object3D> objectSkydome_;
};

