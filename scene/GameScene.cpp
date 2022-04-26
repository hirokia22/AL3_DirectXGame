#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <cmath>

using namespace DirectX;

GameScene::GameScene(){};

GameScene::~GameScene() { delete model_; }

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();
	textureHandle_ = TextureManager::Load("Mario.jpg");
	model_ = Model::Create();
	worldTransform_[0].Initialize();
	for (int i = 1; i < _countof(worldTransform_); i++) {
		worldTransform_[i].translation_ = {
		  8.0f*(cosf((XM_PI / 5.0f)*i) - sinf((XM_PI / 5.0f)*i)), 8.0f*(cosf((XM_PI/5.0f)*i)+sinf((XM_PI/5.0f)*i)),0
		 };
		worldTransform_[i].parent_ = &worldTransform_[0];
		worldTransform_[i].Initialize();
	}
	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update() { 
	const float rotate = 0.04f;
	worldTransform_[0].rotation_.z += rotate;
	for (int i = 1; i < _countof(worldTransform_); i++) {
		worldTransform_[i].rotation_.z -= rotate;
	}
	for (int i = 0; i <= 10; i++) {
		worldTransform_[i].UpdateMatrix();
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// 3Dオブジェクト描画
	for (int i = 1; i < _countof(worldTransform_); i++) {
		model_->Draw(worldTransform_[i], viewProjection_, textureHandle_);
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

