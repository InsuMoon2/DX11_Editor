#include "pch.h"
#include "ImGuiManager.h"

void ImGuiManager::Init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;

    io.Fonts->AddFontFromFileTTF(
        "C:/Windows/Fonts/malgun.ttf",
        18.0f,
        nullptr,
        io.Fonts->GetGlyphRangesKorean());

    ImGuiStyleSetting();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(GAME->GetGameDesc().hWnd);
    ImGui_ImplDX11_Init(DEVICE.Get(), ENGINE_DC.Get());
}

void ImGuiManager::Update()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::Render()
{
    // Rendering
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // 멀티 뷰포트
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void ImGuiManager::ImGuiStyleSetting()
{
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    ImGuiStyle& style = ImGui::GetStyle();
    //style.FramePadding = ImVec2(6, 4);            // 기본 (4, 3)에서 늘리기
    //style.ItemSpacing = ImVec2(10, 6);            // 아이템 간격
    //style.WindowTitleAlign = ImVec2(0.0f, 0.5f);  // 제목 정렬

    // 일반 버튼 (Button)
    style.Colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f); // 평소 (짙은 회색)
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f); // 마우스 올림 (조금 밝게)
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f); // 클릭 중 (더 밝게)

    // 탭 상단의 강조용 얇은 선 (Overline)
    style.Colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f); // 회색으로 변경

    // 체크박스 내부 체크 표시 (CheckMark)
    // 이미지의 'Auto-scroll' 옆 파란색 체크를 회색이나 흰색으로 바꿉니다.
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);

    // 슬라이더 손잡이 (SliderGrab)
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);

    // 탭(Tab) 색상 - 이미지 상단의 'Scene', 'Console' 탭 부분
    style.Colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f); // 비활성 탭
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f); // 마우스 올렸을 때
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f); // 활성 탭 (지금 파란색인 곳)
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);

    // 타이틀 배경 (TitleBg) - 탭 아래쪽이나 창 상단
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f); // 창이 선택됐을 때 파란색 방지
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);

    // 헤더 및 하이라이트 (Header) - 하이어라키나 컨텐츠 브라우저 선택 색상
    style.Colors[ImGuiCol_Header] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

    // 체크박스, 슬라이더 등 (FrameBg)
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);

    // 도킹(Docking) 미리보기 색상 (창 옮길 때 파란색 칸 생기는 것)
    style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.40f, 0.40f, 0.40f, 0.70f);
}
