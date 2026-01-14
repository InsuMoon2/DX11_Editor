#include "pch.h"
#include "ContentBrowserView.h"
#include "EditorManager.h"
#include "AnimationView.h"
#include "Engine/Utils.h"
#include "ModelAnimation.h"
#include "Model.h"
#include "FileUtils.h"

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

    auto& entries = GET_SINGLE(ModelRegistry)->GetAll();

    for (auto& [modelName, entry] : entries)
    {
        string modelNameStr = Utils::ToString(modelName);

        if (ImGui::TreeNode(modelNameStr.c_str()))
        {
            auto& animations = entry.model->GetAnimations();

            for (size_t i = 0; i < animations.size(); i++)
            {
                wstring animNameW = FileUtils::PathToAnimName(entry.animPaths[i]);
                string animName = Utils::ToString(animNameW);

                bool isSelected = (_selectedIndex == (int)i);

                if (ImGui::Selectable(animName.c_str(), isSelected))
                {
                    _selectedIndex = (int)i;
                }

                // 더블 클릭 -> Animation View
                if (ImGui::IsItemHovered && ImGui::IsMouseDoubleClicked(0))
                {
                    auto animView = dynamic_pointer_cast<AnimationView>(
                        GET_SINGLE(EditorManager)->GetWindow(L"Animation"));

                    if (animView)
                    {
                        animView->SetAnimation(entry.model, (int)i, entry.animPaths);
                        //animView->SetModelAnimator(entry.animator);
                        animView->SetActive(true);
                    }
                }
            }
            ImGui::TreePop();
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
