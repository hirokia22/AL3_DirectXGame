#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() { delete model_;}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();
	textureHandle_ = TextureManager::Load("Mario.jpg");
	model_ = Model::Create();
	
#pragma region プレイヤーの設定
		//大本
		worldTransform_[0].Initialize();

#pragma region 上半身
		//胴体
	    worldTransform_[1].parent_ = &worldTransform_[0];
		worldTransform_[1].Initialize();
		//頭
		worldTransform_[2].translation_ = {0,4.5f, 0};
		worldTransform_[2].parent_ = &worldTransform_[1];
		worldTransform_[2].Initialize();
		//左腕
		worldTransform_[3].translation_ = {4.5f, 0, 0};
		worldTransform_[3].parent_ = &worldTransform_[1];
		worldTransform_[3].Initialize();
		//右腕
		worldTransform_[4].translation_ = {-4.5f, 0, 0};
		worldTransform_[4].parent_ = &worldTransform_[1];
		worldTransform_[4].Initialize();
#pragma endregion

#pragma region 下半身
		//尻
		worldTransform_[5].translation_ = {0, -4.5f, 0};
		worldTransform_[5].parent_ = &worldTransform_[0];
		worldTransform_[5].Initialize();
		//左足
	    worldTransform_[6].translation_ = {4.5f, -4.5f, 0};
	    worldTransform_[6].parent_ = &worldTransform_[5];
	    worldTransform_[6].Initialize();
		//右足
	    worldTransform_[7].translation_ = {-4.5f, -4.5f, 0};
	    worldTransform_[7].parent_ = &worldTransform_[5];
	    worldTransform_[7].Initialize();
#pragma endregion
#pragma endregion
	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update() {
#pragma region キャラクター移動処理
	//キャラクターの移動ベクトル
	XMFLOAT3 move = {0, 0, 0};

	//キャラクターの移動早さ
	const float kCharacterSpeed = 0.2f;
	
	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_LEFT)) {
		move = {-kCharacterSpeed, 0, 0};
	} else if (input_->PushKey(DIK_RIGHT)) {
		move = {kCharacterSpeed, 0, 0};
	}
	//注視点移動(ベクトルの加算)
	worldTransform_[PartId::Root].translation_.x += move.x;
	worldTransform_[PartId::Root].translation_.y += move.y;
	worldTransform_[PartId::Root].translation_.z += move.z;
	//デバッグ用表示
	debugText_->SetPos(50, 50);
	debugText_->Printf(
	  "Root:(%f,%f,%f)", worldTransform_[PartId::Root].translation_.x,
	  worldTransform_[PartId::Root].translation_.y, worldTransform_[PartId::Root].translation_.z);
	for (int i = Root; i <= LegR; i++) {
		worldTransform_[i].UpdateMatrix();
	}
#pragma endregion

#pragma region 上半身回転処理
	//上半身の回転早さ
	const float kChestRotSpeed = 0.05f;
	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_U)) {
		worldTransform_[PartId::Chest].rotation_.y -= kChestRotSpeed;
	} else if (input_->PushKey(DIK_I)) {
		worldTransform_[PartId::Chest].rotation_.y += kChestRotSpeed;
	}
	for (int i = Chest; i <= ArmR; i++) {
		worldTransform_[i].UpdateMatrix();
	}
#pragma endregion

#pragma region 下半身回転処理
	//下半身の回転速さ
	const float kHipRotSpeed = 0.05f;
	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_J)) {
		worldTransform_[PartId::Hip].rotation_.y -= kHipRotSpeed;
	} else if (input_->PushKey(DIK_K)) {
		worldTransform_[PartId::Hip].rotation_.y += kHipRotSpeed;
	}
	for (int i = Hip; i <= LegR; i++) {
		worldTransform_[i].UpdateMatrix();
	}
#pragma endregion
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
	for (int i = Chest; i <= LegR; i++) {
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
