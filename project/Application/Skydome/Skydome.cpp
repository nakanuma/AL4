#include "Skydome.h"
#include <cassert>

void Skydome::Initialize(ModelManager::ModelData* modelSkydome)
{
	assert(modelSkydome);

	objectSkydome_ = std::make_unique<Object3D>();
	objectSkydome_->model_ = modelSkydome;

	// サイズを変更
	objectSkydome_->transform_.scale = { 500.0f, 500.0f, 500.0f };
}

void Skydome::Update()
{
	// 行列の更新
	objectSkydome_->UpdateMatrix();
}

void Skydome::Draw()
{
	// オブジェクト描画
	objectSkydome_->Draw();
}
