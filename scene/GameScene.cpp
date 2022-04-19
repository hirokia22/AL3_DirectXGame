#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <random>

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
	//乱数シード生成器
	std::random_device seed_gen;
	//メルセンス・ツイスター
	std::mt19937_64 engine(seed_gen());
	//乱数範囲(回転角用)
	std::uniform_real_distribution<float> rotDist(0.0f, XM_2PI);
	//乱数範囲(座標用)
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);
	for (size_t i = 0; i < _countof(worldTransform_); i++) {
		//スケール
		worldTransform_[i].scale_ = {1.0f, 1.0f, 1.0f};
		//回転
		worldTransform_[i].rotation_ = {rotDist(engine) , rotDist(engine) , rotDist(engine)};
		//平行移動
		worldTransform_[i].translation_ = {posDist(engine), posDist(engine), posDist(engine)};
		//ワールドトランスフォームの初期化
		worldTransform_[i].Initialize();
	}
	//カメラ垂直方向視野角を設定
	viewProjection_.fovAngleY = XMConvertToRadians(10.0f);
	//ニアクリップ垂直を設定
	viewProjection_.nearZ = 52.0f;
	//ファークリップ垂直を設定
	viewProjection_.farZ = 53.0f;
	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update() {
	
#pragma region 視点移動処理
	/*
	XMFLOAT3 move = {0, 0, 0};
	const float kEyeSpeed = 0.2f;
	if (input_->PushKey(DIK_W)) {
		move = {0, 0, kEyeSpeed};
	} else if (input_->PushKey(DIK_S)) {
		move = {0, 0, -kEyeSpeed};
	}
	//視点移動(ベクトルの計算)
	viewProjection_.eye.x += move.x;
	viewProjection_.eye.y += move.y;
	viewProjection_.eye.z += move.z;
	*/
	//行列の再計算
	viewProjection_.UpdateMatrix();
	//デバッグ再表示
	debugText_->SetPos(50, 50);
	debugText_->Printf(
	  "eye:(% f, % f, % f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);
#pragma endregion 

#pragma region 注視点移動処理
	/*
	const float kTargetSpeed = 0.2f;
	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_LEFT)) {
		move = {-kTargetSpeed, 0, 0};
	} else if (input_->PushKey(DIK_RIGHT)) {
		move = {kTargetSpeed, 0, 0};
	}
	//視点移動(ベクトルの計算)
	viewProjection_.target.x += move.x;
	viewProjection_.target.y += move.y;
	viewProjection_.target.z += move.z;
	*/
	//行列の再計算
	viewProjection_.UpdateMatrix();
	//デバッグ再表示
	debugText_->SetPos(50, 70);
	debugText_->Printf(
	  "target:(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y,
	  viewProjection_.target.z);
#pragma endregion

#pragma region 上方向回転処理
	/*
	const float kUpRotSpeed = 0.05f;
	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_SPACE)) {
		viewAngle += kUpRotSpeed;
		viewAngle = fmodf(viewAngle, XM_2PI);
	}
	//上方向ベクトルを計算(半径1の円周上の座標)
	viewProjection_.up = {cosf(viewAngle), sinf(viewAngle), 0.0f};
	*/
	//行列の再計算
	viewProjection_.UpdateMatrix();
	//デバッグ再表示
	debugText_->SetPos(50, 90);
	debugText_->Printf(
	  "up:(%f,%f,%f)", viewProjection_.up.x, viewProjection_.up.y,
	  viewProjection_.up.z);
#pragma endregion

#pragma region FoV変更処理
	//上キーで視野角が広がる
	if (input_->PushKey(DIK_W)) {
		viewProjection_.fovAngleY += 0.01f;
		viewProjection_.fovAngleY = min(viewProjection_.fovAngleY, XM_PI);
	}//下キーで視野角が狭まる
	else if (input_->PushKey(DIK_S)) {
		viewProjection_.fovAngleY -= 0.01f;
		viewProjection_.fovAngleY = max(viewProjection_.fovAngleY, 0.01f);
	}
	//行列の再計算
	viewProjection_.UpdateMatrix();
	//デバッグ再表示
	debugText_->SetPos(50, 110);
	debugText_->Printf(
	  "fovAngleY(Degree):%f", XMConvertToDegrees(viewProjection_.fovAngleY));
#pragma endregion

#pragma region クリップ垂直変更処理
	//上下キーでニアクリップ垂直を増減
	if (input_->PushKey(DIK_UP)) {
		viewProjection_.nearZ += 0.1f;
	} else if (input_->PushKey(DIK_DOWN)) {
		viewProjection_.nearZ -= 0.1f;
	}
	//行列の再計算
	viewProjection_.UpdateMatrix();
	//デバッグ再表示
	debugText_->SetPos(50, 130);
	debugText_->Printf("nearZ:%f", XMConvertToDegrees(viewProjection_.nearZ));
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
	for (size_t i = 0; i < _countof(worldTransform_); i++) {
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
