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
	for (int i = 0; i < _countof(worldTransform_[0]); i++) {
		for (int j = 0; j < _countof(worldTransform_[0]); j++) {
			for (int m = 0; m < _countof(worldTransform_[0]); m++) {
				worldTransform_[i][j][m].scale_ = {1, 1, 1};
				worldTransform_[i][j][m].translation_ = {
				  (i *3) - 12.0f, (j *3) - 12.0f, (m * 3) + 5.0f};
				worldTransform_[i][j][m].Initialize();
			}
		}
	}
	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update() {}

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
	for (int i = 0; i < _countof(worldTransform_); i++) {
		for (int j = 0; j < _countof(worldTransform_); j++) {
			for (int m = 0; m < _countof(worldTransform_); m++) {
				model_->Draw(worldTransform_[i][j][m], viewProjection_, textureHandle_);
			}
		}
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
