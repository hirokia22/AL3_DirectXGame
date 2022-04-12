#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() { delete sprite_; }

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();
	textureHundle_ = TextureManager::Load("Mario.jpg");
	sprite_ = Sprite::Create(textureHundle_, {100, 50});
	soundDateHandle_ = audio_->LoadWave("se_sad03.wav");
	audio_->PlayWave(soundDateHandle_);
	voiceHandle_ = audio_->PlayWave(soundDateHandle_, true);
}

void GameScene::Update() {
	XMFLOAT2 position = sprite_->GetPosition();
	position.x += 2.0f;
	position.y += 1.0f;

	sprite_->SetPosition(position);
	if (input_->TriggerKey(DIK_SPACE)) {
		//音声停止
		audio_->StopWave(voiceHandle_);
	}
	//変数の値をインクリメント
	value_++;
	//値を含んだ文字列
	std::string strDebug = std::string("value:")+
	std::to_string(value_);
	//デバッグテキストの表示
	debugText_->Print(strDebug, 50, 50, 1.0f);
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
	sprite_->Draw();
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
