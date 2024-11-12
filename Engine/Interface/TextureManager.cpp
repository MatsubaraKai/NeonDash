#include "TextureManager.h"
#include "DirectXCommon.h"
#include "SRVManager.h"
#include "WinAPI.h"
#include <d3dx12.h>
#include <Mesh.h>
// DirectXCommon dirX;

/// <summary>
/// リソースを保存する場所を設定
/// ImGuiが0に格納されているため1から格納する
/// </summary>
int TextureManager::kSRVIndexTop = 1;

/// <summary>
/// パーティクル用のインデックスを定義
/// </summary>
int TextureManager::kParIndez = 10;

/// <summary>
/// テクスチャ生成に必要なデータを定義
/// </summary>
std::unordered_map<std::string, TextureData> TextureManager::textureDatas_;

/// <summary>
/// 中間リソースの配列（テクスチャデータ転送用）
/// </summary>
Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::intermediateResource_[kNumDescriptors] = { nullptr };

/// <summary>
/// テクスチャマネージャーを初期化する関数
/// </summary>
void TextureManager::Init()
{
    // テクスチャデータのマップの容量を予約
    textureDatas_.reserve(SRVManager::kMaXSRVCount);
}

/// <summary>
/// テクスチャを読み込み、SRVを作成する関数
/// </summary>
/// <param name="filePath">テクスチャファイルのパス</param>
/// <returns>SRVのインデックス</returns>
int TextureManager::StoreTexture(const std::string& filePath) {
    // 既に読み込み済みのテクスチャか確認
    if (textureDatas_.contains(filePath)) {
        return textureDatas_[filePath].srvIndex;
    };
    // テクスチャ枚数上限チェック（必要であれば実装）
    // assert(SRVManager::kMaXSRVCount <= SRVManager::);

    DirectXCommon* sDirectXCommon = DirectXCommon::GetInstance();
    WinAPI* sWinAPI = WinAPI::GetInstance();

    TextureData& textureData = textureDatas_[filePath];

    // テクスチャを読み込み、ミップマップを作成
    DirectX::ScratchImage mipImages_ = LoadTexture(filePath);
    textureData.srvIndex = SRVManager::Allocate();
    textureData.metaData = mipImages_.GetMetadata();
    textureData.resource = CreateTextureResource(sDirectXCommon->GetDevice().Get(), textureData.metaData);

    // テクスチャデータをGPUにアップロード
    intermediateResource_[textureData.srvIndex] = UploadTextureData(textureData.resource.Get(), mipImages_);

    // SRVを作成するディスクリプタヒープの場所を設定
    textureData.textureSrvHandleCPU = SRVManager::GetCPUDescriptorHandle(textureData.srvIndex);
    textureData.textureSrvHandleGPU = SRVManager::GetGPUDescriptorHandle(textureData.srvIndex);

    // SRVを作成
    SRVManager::CreateSRVforTexture2D(textureData);

    return textureData.srvIndex;
};

/// <summary>
/// パーティクル用のインデックスを増加させる関数
/// </summary>
/// <returns>更新後のインデックス</returns>
int TextureManager::PlusIndex() {
    // インデックスをインクリメント
    kParIndez++;
    return kParIndez;
}

/// <summary>
/// テクスチャマネージャーのリソースを解放する関数
/// </summary>
void TextureManager::Release() {
    // 必要に応じてリソース解放処理を実装
}

/// <summary>
/// シングルトンインスタンスを取得する関数
/// </summary>
/// <returns>TextureManagerのインスタンス</returns>
TextureManager* TextureManager::GetInstance() {
    static TextureManager instance;
    return &instance;
}

// void TextureManager::SetTexture() {
//     sDirectXCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU_);
// };

/// <summary>
/// テクスチャのメタデータを取得する関数
/// </summary>
/// <param name="filePath">テクスチャファイルのパス</param>
/// <returns>テクスチャのメタデータ</returns>
const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath)
{
    // 範囲外指定違反チェック（必要であれば実装）
    // assert();
    TextureData& textureData = textureDatas_[filePath];
    return textureData.metaData;
}

/// <summary>
/// テクスチャファイルを読み込み、ミップマップを作成する関数
/// </summary>
/// <param name="filePath">テクスチャファイルのパス</param>
/// <returns>読み込んだテクスチャのScratchImageオブジェクト</returns>
DirectX::ScratchImage TextureManager::LoadTexture(const std::string& filePath) {
    // ファイルパスをワイド文字列に変換
    std::wstring filePathW = ConvertString(filePath);
    // テクスチャを読み込むためのScratchImage
    DirectX::ScratchImage image{};
    // エラー検知用変数
    HRESULT hr_;

    if (filePathW.ends_with(L".dds")) {
        // .ddsファイルとして読み込む
        hr_ = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
    }
    else {
        // WIC形式として読み込む
        hr_ = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
    }

    // ミップマップの作成
    DirectX::ScratchImage mipImages{};
    if (DirectX::IsCompressed(image.GetMetadata().format)) {
        // 圧縮フォーマットならそのまま使用
        mipImages = std::move(image);
    }
    else {
        // ミップマップを生成
        hr_ = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 4, mipImages);
    }
    assert(SUCCEEDED(hr_));

    // ミップマップ付きのデータを返す
    return mipImages;
}

/// <summary>
/// テクスチャリソースを作成する関数
/// </summary>
/// <param name="device">D3D12デバイス</param>
/// <param name="metadata">テクスチャのメタデータ</param>
/// <returns>作成されたテクスチャリソース</returns>
Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata)
{
    Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

    D3D12_RESOURCE_DESC resourceDesc_{};
    // メタデータを基にリソースの設定を行う
    resourceDesc_.Width = UINT(metadata.width); // テクスチャの幅
    resourceDesc_.Height = UINT(metadata.height); // テクスチャの高さ
    resourceDesc_.MipLevels = UINT16(metadata.mipLevels); // ミップマップの数
    resourceDesc_.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行きまたは配列数
    resourceDesc_.Format = metadata.format; // テクスチャのフォーマット
    resourceDesc_.SampleDesc.Count = 1; // サンプリングカウント。1固定
    resourceDesc_.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); // テクスチャの次元数

    // ヒーププロパティの設定
    D3D12_HEAP_PROPERTIES heapProperties_{};
    heapProperties_.Type = D3D12_HEAP_TYPE_DEFAULT; // デフォルトヒープを使用

    // リソースの生成
    resource_ = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties_, // ヒープの設定
        D3D12_HEAP_FLAG_NONE, // ヒープフラグ
        &resourceDesc_, // リソースの設定
        D3D12_RESOURCE_STATE_COPY_DEST, // 初期状態はコピー先
        nullptr, // クリア値（不要）
        IID_PPV_ARGS(&resource_));
    assert(SUCCEEDED(hr));

    resource_->SetName(L"TextureResource");
    return resource_;
}

/// <summary>
/// テクスチャデータをGPUにアップロードする関数
/// </summary>
/// <param name="texture">アップロード先のテクスチャリソース</param>
/// <param name="mipImages">ミップマップ付きのテクスチャデータ</param>
/// <returns>中間リソース（アップロード用バッファ）</returns>
[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages) {

    // サブリソースデータの準備
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    DirectX::PrepareUpload(DirectXCommon::GetInstance()->GetDevice().Get(),
        mipImages.GetImages(), mipImages.GetImageCount(),
        mipImages.GetMetadata(), subresources);

    // 必要な中間リソースのサイズを計算
    uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));

    // 中間リソースを作成
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = Mesh::CreateBufferResource(DirectXCommon::GetInstance()->GetDevice().Get(), intermediateSize);

    // テクスチャデータをアップロード
    UpdateSubresources(DirectXCommon::GetInstance()->GetCommandList().Get(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());

    // リソースの状態をコピー先から読み取り可能に変更
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = texture.Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
    DirectXCommon::GetInstance()->GetCommandList()->ResourceBarrier(1, &barrier);

    return intermediateResource;
    // 以下は旧方式でのアップロードコード（コメントアウト）
    /*
    // メタデータを取得
    const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
    // 全ミップマップについて
    for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
        // ミップマップレベルを指定してイメージを取得
        const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
        // テクスチャにデータを転送
        HRESULT hr = texture->WriteToSubresource(
            UINT(mipLevel),
            nullptr,             // 全領域へコピー
            img->pixels,         // 元データのポインタ
            UINT(img->rowPitch), // 1ラインのサイズ
            UINT(img->slicePitch) // 1枚のサイズ
        );
        assert(SUCCEEDED(hr));
    }
    */
};
