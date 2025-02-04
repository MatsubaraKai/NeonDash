#include "ImGuiCommon.h"
#include"WinAPI.h"
#include"DirectXCommon.h"
#include "SRVManager.h"
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_win32.h"
#include "../../imgui/imgui_impl_dx12.h"

//
void ImGuiCommon::Initialize() {
	sWinAPI_ = WinAPI::GetInstance();
	sDirectXCommon_ = DirectXCommon::GetInstance();
	//ImGuiの初期化。詳細はさして重要ではないので省略する。
	//こういうもんである
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();  // ImGuiIO オブジェクトを取得
#ifdef _DEBUG
	// デバッグビルドでは imgui.ini を保存
	io.IniFilename = "imgui.ini";
#else
	// リリースビルドでは imgui.ini を保存しない
	io.IniFilename = nullptr;
#endif
	ImGui::StyleColorsCustom();
	ImGui_ImplWin32_Init(sWinAPI_->GetHwnd());
	ImGui_ImplDX12_Init(sDirectXCommon_->GetDevice().Get(),
		sDirectXCommon_->GetSwapChainDesc().BufferCount,
		sDirectXCommon_->GetrtvDesc().Format,
		SRVManager::GetInstance()->GetDescriptorHeap().Get(),
		SRVManager::GetInstance()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		SRVManager::GetInstance()->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

}



void ImGuiCommon::Update() {
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//ImGui::ShowDemoWindow();
};

void ImGuiCommon::Draw() {
	ImGui::Render();
	////描画用のDescriptorの設定
	//ID3D12DescriptorHeap* descriptorHeaps[] = { sDirectXCommon_->GetSrvDescriptorHeap()};
	//sDirectXCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), sDirectXCommon_->GetCommandList().Get());
}

ImGuiCommon* ImGuiCommon::GetInstance() {
	static ImGuiCommon instance;
	return &instance;
}

void ImGuiCommon::UICreate() {
	//ImGui::Render();
	//描画用のDescriptorの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { SRVManager::GetInstance()->GetDescriptorHeap().Get() };
	sDirectXCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
}


void ImGuiCommon::Release() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}