#include "Particle.h"
#include "Sprite.h"
#include "WinAPI.h"
#include "DirectXCommon.h"
#include "Camera.h"
#include "mathFunction.h"
#include "Mesh.h"
#include "SRVManager.h"
#include <numbers>

/**
* @file Particle.cpp
* @brief Particle
*/

Particle::Particle() {};
Particle::~Particle() {};
void Particle::Initialize(Emitter emitter) {
	sWinAPI = WinAPI::GetInstance();
	sDirectXCommon = DirectXCommon::GetInstance();
	// Sprite用の頂点リソースを作る
	vertexResourceSprite_ = Mesh::CreateBufferResource(sDirectXCommon->GetDevice(), sizeof(VertexData) * 4);

	// リソースを先頭のアドレスから使う
	vertexBufferViewSprite_.BufferLocation = vertexResourceSprite_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferViewSprite_.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点あたりのサイズ
	vertexBufferViewSprite_.StrideInBytes = sizeof(VertexData);

	vertexResourceSprite_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite_));

	// 1枚目の三角形
	vertexDataSprite_[0].position = { -1.0f,-1.0f,0.0f,1.0f };//左下
	vertexDataSprite_[0].texcorrd = { 0.0f,1.0f };
	vertexDataSprite_[1].position = { -1.0f,1.0f,0.0f,1.0f }; // 左上
	vertexDataSprite_[1].texcorrd = { 0.0f,0.0f };
	vertexDataSprite_[2].position = { 1.0f,-1.0f,0.0f,1.0f }; // 右下
	vertexDataSprite_[2].texcorrd = { 1.0f,1.0f };

	vertexDataSprite_[3].position = { 1.0f,1.0f,0.0f,1.0f }; // 右上
	vertexDataSprite_[3].texcorrd = { 1.0f,0.0f };

	// 実際に頂点リソースを作る
	materialResource = Mesh::CreateBufferResource(sDirectXCommon->GetDevice(), sizeof(Material));

	/*materialBufferView = CreateBufferView();;*/
	// 頂点リソースにデータを書き込む
	materialData = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 色のデータを変数から読み込み
	materialData->color = { 1.0f,1.0f,1.0f,1.0f };
	materialData->uvTransform = MakeIdentity4x4();
	materialData->enableLighting = false;

	// Transform変数の初期化

	// Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	transformationMatrixResouceSprite = Mesh::CreateBufferResource(sDirectXCommon->GetDevice(), sizeof(ParticleForGPU) * kNumMaxInstance);
	// 書き込むためのアドレスを取得
	transformationMatrixResouceSprite->Map(0, nullptr, reinterpret_cast<void**>(&instancingData));
	// 単位行列を書き込んでおく

	for (uint32_t index = 0; index < kNumMaxInstance; ++index) {
		instancingData[index].WVP = MakeIdentity4x4();
		instancingData[index].World = MakeIdentity4x4();
		instancingData[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	indexResourceSprite = Mesh::CreateBufferResource(sDirectXCommon->GetDevice(), sizeof(uint32_t) * 6);
	// リソースの先頭のアドレスから使う
	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint_tとする
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

	// インデックスリソースにデータを書き込む
	uint32_t* indexDataSprite = nullptr;
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	indexDataSprite[0] = 0;
	indexDataSprite[1] = 1;
	indexDataSprite[2] = 2;
	indexDataSprite[3] = 1;
	indexDataSprite[4] = 3;
	indexDataSprite[5] = 2;

	transformUv = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	SRVIndex_ = SRVManager::Allocate();
	SRVManager::CreateSRVforStructuredBuffer(SRVIndex_, transformationMatrixResouceSprite.Get(), kNumMaxInstance, sizeof(ParticleForGPU));
	instancingSrvHandleCPU = SRVManager::GetCPUDescriptorHandle(SRVIndex_);
	instancingSrvHandleGPU = SRVManager::GetGPUDescriptorHandle(SRVIndex_);

	directionalLightData = nullptr;
	directionalLightResource = Mesh::CreateBufferResource(sDirectXCommon->GetDevice(), sizeof(DirectionalLight));
	// 書き込むためのアドレスを取得
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));

	// デフォルト値はとりあえず以下のようにしておく
	directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData->intensity = 1.0f;
	emitter_ = emitter;
	emitter_.count = 6;
	emitter_.frequency = 0.02f;// 0.5秒ごとに発生
	emitter_.frequencyTime = 0.0f;// 発生頻度用の時刻、0で初期化
};
//void Sprite::Update() {
//
//};

void Particle::Draw(Emitter emitter, const Vector3& worldTransformPa, uint32_t texture, Camera* camera,
	const RandRangePro& randRange, bool scaleAddFlag, float minLifetime, float maxLifetime) {
	pso_ = PSOParticle::GetInstance();

	emitter_.count = emitter.count;
	emitter_.transform = { emitter.transform.scale, {0.0f, 0.0f, 0.0f}, worldTransformPa };
	randRange_ = randRange;

	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());
	std::uniform_real_distribution<float> lifetimeDistribution(minLifetime, maxLifetime);

	emitter_.frequencyTime += kDeltaTime;  // 時刻を進める
	if (emitter_.frequency <= emitter_.frequencyTime) {
		// 寿命範囲に基づいてパーティクルを生成
		for (uint32_t count = 0; count < emitter_.count; ++count) {
			ParticlePro newParticle = MakeNewParticle(randomEngine, emitter_.transform.scale, worldTransformPa, randRange_);
			newParticle.lifeTime = lifetimeDistribution(randomEngine);  // 寿命をランダム設定
			particles_.push_back(newParticle);
		}
		emitter_.frequencyTime -= emitter_.frequency;  // 頻度計算の余剰を引く
	}

	uint32_t numInstance = 0;  // 描画すべきインスタンス数

	for (auto particleIterator = particles_.begin(); particleIterator != particles_.end();) {
		if (particleIterator->lifeTime <= particleIterator->currentTime) {
			particleIterator = particles_.erase(particleIterator);  // 寿命を超えたパーティクルを削除
			continue;
		}

		// パーティクルの動き更新
		particleIterator->transform.translate.x += particleIterator->velocity.x * kDeltaTime;
		particleIterator->transform.translate.y += particleIterator->velocity.y * kDeltaTime;
		particleIterator->transform.translate.z += particleIterator->velocity.z * kDeltaTime;

		if (scaleAddFlag) {
			particleIterator->transform.scale = Add(particleIterator->transform.scale, { 0.1f, 0.1f, 0.1f });
		}
		particleIterator->currentTime += kDeltaTime;

		// アルファ値を寿命に基づいて計算
		float alpha = 1.0f - (particleIterator->currentTime / particleIterator->lifeTime);

		// ビルボード行列の計算（カメラの方向を向く）
		Matrix4x4 billboardMatrix = MakeIdentity4x4();
		billboardMatrix.m[0][0] = camera->GetViewMatrix().m[0][0];
		billboardMatrix.m[0][1] = camera->GetViewMatrix().m[1][0];
		billboardMatrix.m[0][2] = camera->GetViewMatrix().m[2][0];
		billboardMatrix.m[1][0] = camera->GetViewMatrix().m[0][1];
		billboardMatrix.m[1][1] = camera->GetViewMatrix().m[1][1];
		billboardMatrix.m[1][2] = camera->GetViewMatrix().m[2][1];
		billboardMatrix.m[2][0] = camera->GetViewMatrix().m[0][2];
		billboardMatrix.m[2][1] = camera->GetViewMatrix().m[1][2];
		billboardMatrix.m[2][2] = camera->GetViewMatrix().m[2][2];

		// ワールド行列の計算
		Matrix4x4 worldMatrix = Multiply(
			MakeScaleMatrix(particleIterator->transform.scale),
			Multiply(billboardMatrix, MakeTranslateMatrix(particleIterator->transform.translate))
		);

		// ワールド・ビュー・プロジェクション行列の計算
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(camera->GetViewMatrix(), camera->GetProjectionMatrix()));

		if (numInstance < kNumMaxInstance) {
			instancingData[numInstance].WVP = worldViewProjectionMatrix;
			instancingData[numInstance].World = worldMatrix;
			instancingData[numInstance].color = particleIterator->color;
			instancingData[numInstance].color.w = alpha;
		}

		++numInstance;
		++particleIterator;
	}

	textureManager_ = TextureManager::GetInstance();

	sDirectXCommon->GetCommandList()->SetGraphicsRootSignature(pso_->GetProperty().rootSignature.Get());
	sDirectXCommon->GetCommandList()->SetPipelineState(pso_->GetProperty().graphicsPipelineState.Get());
	sDirectXCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite_);
	sDirectXCommon->GetCommandList()->IASetIndexBuffer(&indexBufferViewSprite);
	sDirectXCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	sDirectXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	sDirectXCommon->GetCommandList()->SetGraphicsRootDescriptorTable(1, instancingSrvHandleGPU);
	sDirectXCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, SRVManager::GetInstance()->GetGPUDescriptorHandle(texture));
	sDirectXCommon->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

	sDirectXCommon->GetCommandList()->DrawIndexedInstanced(6, numInstance, 0, 0, 0);
}


Particle::ParticlePro Particle::MakeNewParticle(std::mt19937& randomEngine, const Vector3& scale, const Vector3& translate, const RandRangePro& randRange)
{
	RandRangePro ran = randRange;

	// ランダム分布を設定
	std::uniform_real_distribution<float> velocityDistribution(-2.0f, 2.0f); // 初速をより広い範囲で設定
	std::uniform_real_distribution<float> distriposX(ran.rangeX.x, ran.rangeX.y);
	std::uniform_real_distribution<float> distriposY(ran.rangeY.x, ran.rangeY.y);
	std::uniform_real_distribution<float> distriposZ(ran.rangeZ.x, ran.rangeZ.y);
	std::uniform_real_distribution<float> distTime(0.3f, 1.0f); // 寿命を延ばして長く動かす
	std::uniform_real_distribution<float> accelerationDistribution(-0.1f, 0.1f); // 加速度をランダム化
	std::uniform_real_distribution<float> rotationDistribution(0.0f, 360.0f); // 回転角度

	// パーティクルプロパティを設定
	Particle::ParticlePro particle;
	particle.transform.scale = scale;

	// 初期回転をランダム化
	particle.transform.rotate = {
		rotationDistribution(randomEngine),
		rotationDistribution(randomEngine),
		rotationDistribution(randomEngine)
	};

	// ランダム位置を設定
	Vector3 randomTranslate = { distriposX(randomEngine), distriposY(randomEngine), distriposZ(randomEngine) };
	particle.transform.translate = Add(translate, randomTranslate);

	// 初速度を設定（方向性を持たせる）
	particle.velocity = {
		velocityDistribution(randomEngine), // X方向の速度
		velocityDistribution(randomEngine) + 6.0f, // Y方向に上昇する力を追加
		velocityDistribution(randomEngine) // Z方向の速度
	};

	// 加速度をランダム化（動きを曲線的にする）
	Vector3 acceleration = {
		accelerationDistribution(randomEngine),
		accelerationDistribution(randomEngine),
		accelerationDistribution(randomEngine)
	};
	particle.color = { 1.0f, 1.0f, 1.0f, 1.0f }; // アルファ値を1.0（完全不透明）に変更
	// 寿命を設定
	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0;

	// 動きに加速度を加味
	particle.velocity = Add(particle.velocity, acceleration);

	return particle;
}

std::list<Particle::ParticlePro> Particle::Emission(const Emitter& emitter, std::mt19937& randEngine, const Vector3& worldTransformPa, const RandRangePro& randRange)
{
	std::list<Particle::ParticlePro> particles;
	for (uint32_t count = 0; count < emitter.count; ++count) {
		particles.push_back(MakeNewParticle(randEngine, emitter.transform.scale, emitter.transform.translate, randRange));

	}
	return particles;
}

D3D12_VERTEX_BUFFER_VIEW Particle::CreateBufferView() {
	D3D12_VERTEX_BUFFER_VIEW view{};

	//リソースの先頭のアドレスから使う
	view.BufferLocation = vertexResourceSprite_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	view.SizeInBytes = sizeof(VertexData) * 6;
	// 1頂点あたりのサイズ
	view.StrideInBytes = sizeof(VertexData);


	return view;
};

void Particle::Particledebug(const char* name, WorldTransform& worldtransform)
{
	//#ifdef _DEBUG
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.7f, 0.2f, 0.8f));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.3f, 0.1f, 0.5f));
	ImGui::Begin("Particle");

	if (ImGui::TreeNode(name))
	{
		float translate[3] = { worldtransform.translation_.x,worldtransform.translation_.y,worldtransform.translation_.z };
		ImGui::DragFloat3("transform", translate, 0.01f);
		ImGui::TreePop();
		worldtransform.translation_ = { translate[0],translate[1],translate[2] };
	}
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	//#endif // _DEBUG
}