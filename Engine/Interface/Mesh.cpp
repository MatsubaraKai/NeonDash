#include "Mesh.h"

/// <summary>
/// メッシュクラスのコンストラクタ
/// </summary>
Mesh::Mesh() {
};

/// <summary>
/// バッファリソースを作成する関数
/// </summary>
/// <param name="device">デバイスのComPtr</param>
/// <param name="sizeInBytes">バッファのサイズ（バイト単位）</param>
/// <returns>作成されたバッファリソースのComPtr</returns>
Microsoft::WRL::ComPtr<ID3D12Resource> Mesh::CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, size_t sizeInBytes) {
    // 頂点リソース用のヒープの設定
    D3D12_HEAP_PROPERTIES uploadHeapProperties{};
    uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // Uploadヒープを使用

    // リソースの記述子を設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; // バッファリソースとして設定
    resourceDesc.Width = sizeInBytes; // リソースのサイズを指定
    // バッファの場合、以下の値は1に設定する必要がある
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR; // バッファの場合はRowMajorレイアウトを使用

    // リソースの作成
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &uploadHeapProperties, // ヒーププロパティ
        D3D12_HEAP_FLAG_NONE,  // ヒープフラグ
        &resourceDesc,         // リソース記述子
        D3D12_RESOURCE_STATE_GENERIC_READ, // リソースの初期状態
        nullptr,               // クリア値（バッファなので不要）
        IID_PPV_ARGS(&resource)
    );
    assert(SUCCEEDED(hr)); // リソース作成が成功したか確認

    resource->SetName(L"Resources/game/cone.png"); // リソースに名前を設定
    return resource; // 作成したリソースを返す
};
