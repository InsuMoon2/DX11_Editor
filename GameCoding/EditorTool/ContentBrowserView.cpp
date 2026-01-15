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

    _folderIcon = RESOURCES->GetOrAddTexture(L"FolderIcon", L"../Resources/Textures/Folder_Icon.png");
}

void ContentBrowserView::Update()
{
    EditorWindow::Update();
}

void ContentBrowserView::OnGui()
{
    ImGui::Begin("Content Browser");

    //ImGui::Text("Animations");
    //ImGui::Separator();

    auto& entries = GET_SINGLE(ModelRegistry)->GetAll();

    float iconSize = 32.f;
    ImTextureID texID = (ImTextureID)_folderIcon->GetComPtr().Get();

    for (auto& [modelName, entry] : entries)
    {
        string modelNameStr = Utils::ToString(modelName);

        // 폴더 아이콘
        ImGui::Image(texID, ImVec2(iconSize, iconSize));
        ImGui::SameLine();

        bool open = ImGui::TreeNodeEx(modelNameStr.c_str(), ImGuiTreeNodeFlags_OpenOnArrow);

        if (open)
        {
            auto& animations = entry.model->GetAnimations();

            for (size_t i = 0; i < animations.size(); i++)
            {
                wstring animNameW = FileUtils::PathToAnimName(entry.animPaths[i]);
                string animName = Utils::ToString(animNameW);

                // 들여쓰기 + 텍스트만
                ImGui::Indent(iconSize + 4.f);

                bool isSelected = (_selectedIndex == (int)i);
                if (ImGui::Selectable(animName.c_str(), isSelected))
                {
                    _selectedIndex = (int)i;
                }

                // 더블 클릭 -> Animation View, Animation Details View 보여주기
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                {
                    auto animView = dynamic_pointer_cast<AnimationView>(
                        GET_SINGLE(EditorManager)->GetWindow(L"Animation"));

                    if (animView)
                    {
                        animView->SetAnimation(entry.model, (int)i, entry.animPaths);
                        //animView->SetModelAnimator(entry.animator);
                        animView->SetActive(true);
                    }

                    auto detailView = GET_SINGLE(EditorManager)->GetWindow(L"Animation Details");
                    if (detailView)
                        detailView->SetActive(true);
                }

                ImGui::Unindent(iconSize + 4.f);
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
