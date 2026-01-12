#include "pch.h"
#include "InspectorView.h"
#include "GameObject.h"
#include "Transform.h"
#include "ModelRenderer.h"

InspectorView::InspectorView()
    : EditorWindow("Inspector")
{
}

InspectorView::~InspectorView()
{
}

void InspectorView::Init()
{
    
}

void InspectorView::Update()
{
    
}

void InspectorView::OnGui()
{
    ImGui::Begin("Inspector");

    if (_target)
    {
        ImGui::Text("GameObject");
        ImGui::Separator();

        DrawTransform();

        ImGui::Separator();

        // === Add Component ===
        if (ImGui::Button("Add Component", ImVec2(-1, 30)))
        {
            ImGui::OpenPopup("AddComponentPopup");
        }

        // 팝업 메뉴
        if (ImGui::BeginPopup("AddComponentPopup"))
        {
            if (ImGui::MenuItem("ModelRenderer"))
            {
                auto shader = make_shared<Shader>(L"17. TweenDemo.fx");
                auto renderer = make_shared<ModelRenderer>(shader);
                _target->AddComponent(renderer);
            }

            ImGui::EndPopup();
        }
    }
    else
    {
        ImGui::Text("No Object Selected");
    }

    ImGui::End();
}

void InspectorView::DrawTransform()
{
    if (!_target) return;

    auto transform = _target->GetTransform();
    if (!transform) return;

    // ─────────────────────────────────────────────
    // Transform 컴포넌트 UI
    // ─────────────────────────────────────────────
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {
        Vec3 pos = transform->GetPosition();
        Vec3 rot = transform->GetRotation();
        Vec3 scale = transform->GetScale();

        // Position
        if (ImGui::DragFloat3("Position", (float*)&pos, 0.1f))
            transform->SetPosition(pos);

        // Rotation (Degree)
        Vec3 rotDegrees = rot * (180.0f / 3.14159f);
        if (ImGui::DragFloat3("Rotation", (float*)&rotDegrees, 1.0f))
        {
            rot = rotDegrees * (3.14159f / 180.0f);
            transform->SetRotation(rot);
        }

        // Scale
        if (ImGui::DragFloat3("Scale", (float*)&scale, 0.01f))
            transform->SetScale(scale);
    }
}
