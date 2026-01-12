#include "pch.h"
#include "ConsoleView.h"
#include "Logger.h"

ConsoleView::ConsoleView()
    : EditorWindow("Console")
{ }

ConsoleView::~ConsoleView()
{ }

void ConsoleView::Init()
{ }

void ConsoleView::Update()
{ }

void ConsoleView::OnGui()
{
    ImGui::Begin("Console");

    // 클리어 버튼
    if (ImGui::Button("Clear"))
        GET_SINGLE(Logger)->Clear();

    ImGui::SameLine();
    ImGui::Checkbox("Auto-scroll", &_autoScroll);

    ImGui::Separator();

    // 로그 목록
    ImGui::BeginChild("LogScroll", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    for (auto& log : GET_SINGLE(Logger)->GetLogs())
    {
        ImVec4 color;
        switch (log.level)
        {
        case LogLevel::Info:    color = ImVec4(1, 1, 1, 1); break;
        case LogLevel::Warning: color = ImVec4(1, 1, 0, 1); break;
        case LogLevel::Error:   color = ImVec4(1, 0, 0, 1); break;
        }

        // 타임스태프가 필요할지?
        // ImGui::TextColored(color, "%s %s", log.timestamp.c_str(), log.message.c_str());
        ImGui::TextColored(color, "%s", log.message.c_str());
    }

    if (_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollY())
        ImGui::SetScrollHereY(1.f);

    ImGui::EndChild();
    ImGui::End();
}
