#include "pch.h"
#include "GameView.h"

GameView::GameView()
    : EditorWindow("Gmme")
{
}

GameView::~GameView()
{
}

void GameView::Init()
{
    // ─────────────────────────────────────────────
    // TODO: 게임용 RenderTarget 설정
    // ─────────────────────────────────────────────
}

void GameView::Update()
{
    // ─────────────────────────────────────────────
    // Game View는 보통 입력을 직접 처리하지 않음
    // (게임 로직에서 처리)
    // ─────────────────────────────────────────────

    //LOG_INFO("Test Logging");

}

void GameView::OnGui()
{
    // ─────────────────────────────────────────────
    // Game View 윈도우 시작
    // ─────────────────────────────────────────────
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Game");

    // ─────────────────────────────────────────────
    // 포커스 / 호버 상태 체크
    // ─────────────────────────────────────────────
    _isFocused = ImGui::IsWindowFocused();
    _isHovered = ImGui::IsWindowHovered();

    // ─────────────────────────────────────────────
    // 뷰포트 크기 계산
    // ─────────────────────────────────────────────
    ImVec2 panelSize = ImGui::GetContentRegionAvail();
    _viewportSize = Vec2(panelSize.x, panelSize.y);

    // ─────────────────────────────────────────────
    // 게임 화면 표시
    // TODO: 실제 RenderTarget 텍스처로 교체
    // ─────────────────────────────────────────────
    ImGui::Text("Game View");
    ImGui::Text("Size: %.0f x %.0f", _viewportSize.x, _viewportSize.y);
    ImGui::Text("Focused: %s", _isFocused ? "Yes" : "No");
    ImGui::Text("Hovered: %s", _isHovered ? "Yes" : "No");

    // TODO: 게임 RenderTarget 이미지 표시
    // ImGui::Image((ImTextureID)gameRenderTarget->GetSRV(), panelSize);
    // ─────────────────────────────────────────────
    // Game View 윈도우 종료
    // ─────────────────────────────────────────────
    ImGui::PopStyleVar();
    ImGui::End();

}
