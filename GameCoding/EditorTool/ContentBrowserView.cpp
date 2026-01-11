#include "pch.h"
#include "ContentBrowserView.h"
#include "EditorManager.h"
#include "AnimationView.h"
#include "Engine/Utils.h"

ContentBrowserView::ContentBrowserView()
    : EditorWindow("Content Browser")
{
}

ContentBrowserView::~ContentBrowserView()
{
}

void ContentBrowserView::Init()
{
    EditorWindow::Init();
}

void ContentBrowserView::Update()
{
    EditorWindow::Update();
}

void ContentBrowserView::OnGui()
{
    ImGui::Begin("Content Browser");

    ImGui::Text("Animations");
    ImGui::Separator();

    // 애니메이션 목록 표시
    for (int i = 0; i < _animations.size(); i++)
    {
        auto& anim = _animations[i];

        string name = Utils::ToString(anim.name);

        bool isSelected = (_selectedIndex == i);

        if (ImGui::Selectable(name.c_str(), isSelected))
        {
            _selectedIndex = i;
        }

        // 더블클릭 -> AnimatinoView 열기
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
        {
            auto animView = dynamic_pointer_cast<AnimationView>(
                GET_SINGLE(EditorManager)->GetWindow(L"Animation"));

            if (animView)
            {
                animView->SetAnimation(anim.model, anim.animIndex, anim.animPaths);
                animView->SetModelAnimator(anim.animator);

                animView->SetActive(true);
            }
        }
    }

    ImGui::End();

}

void ContentBrowserView::AddAnimation(const wstring& name, shared_ptr<Model> model, shared_ptr<ModelAnimator> animator,
    const vector<wstring>& animPaths, int animIndex)
{
    AnimationEntry entry;
    entry.name = name;
    entry.model = model;
    entry.animator = animator;
    entry.animIndex = animIndex;
    entry.animPaths = animPaths;

    _animations.push_back(entry);
}
