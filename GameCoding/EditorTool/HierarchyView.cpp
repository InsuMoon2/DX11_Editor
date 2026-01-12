#include "pch.h"
#include "HierarchyView.h"
#include "GameObject.h"
#include "InspectorView.h"
#include "EditorManager.h"
#include "SceneView.h"

HierarchyView::HierarchyView()
    : EditorWindow("Hierarchy")
{

}

HierarchyView::~HierarchyView()
{
}

void HierarchyView::Init()
{
    
}

void HierarchyView::Update()
{
    
}

void HierarchyView::OnGui()
{
    ImGui::Begin("Hierarchy");

    // ─────────────────────────────────────────────
    // 오브젝트 목록 표시
    // ─────────────────────────────────────────────
    for (int i = 0; i < _sceneObjects.size(); i++)
    {
        auto& obj = _sceneObjects[i];
        if (!obj) continue;

        // 선택 상태 확인
        bool isSelected = (_selectedObject == obj);

        // 트리 노드 또는 선택 가능한 아이템으로 표시
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        if (isSelected)
            flags |= ImGuiTreeNodeFlags_Selected;

        // 오브젝트 이름 (없으면 인덱스로 표시)
        string name = "GameObject_" + to_string(i);

        ImGui::TreeNodeEx((void*)(intptr_t)i, flags, name.c_str());

        // 클릭하면 선택
        if (ImGui::IsItemClicked())
        {
            _selectedObject = obj;

            // Ispector에 선택된 오브젝트를 전달
            auto inspector = dynamic_pointer_cast<InspectorView>(
                GET_SINGLE(EditorManager)->GetWindow(L"Inspector"));

            if (inspector)
                inspector->SetTarget(obj);
        }

        // === 더블 클릭 시 카메라 이동 ===
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
        {
            // 카메라 Lerp 이동
            auto sceneView = dynamic_pointer_cast<SceneView>(
                GET_SINGLE(EditorManager)->GetWindow(L"Scene"));

            if (sceneView && obj->GetTransform())
            {
                Vec3 targetPos = obj->GetTransform()->GetPosition();
                sceneView->FocusOnPosition(targetPos);
            }
        }
    }

    ImGui::End();
}

void HierarchyView::AddObject(shared_ptr<GameObject> obj)
{
    _sceneObjects.push_back(obj);
}

void HierarchyView::RemoveObject(shared_ptr<GameObject> obj)
{
    auto it = find(_sceneObjects.begin(), _sceneObjects.end(), obj);
    if (it != _sceneObjects.end())
    {
        _sceneObjects.erase(it);
    }
}

