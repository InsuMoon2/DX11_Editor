#include "pch.h"
#include "ContentBrowserView.h"
#include "EditorManager.h"
#include "AnimationView.h"
#include "Engine/Utils.h"
#include "ModelAnimation.h"
#include "Model.h"
#include "FileUtils.h"
#include "AnimationDetailView.h"

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
    _clipIcon = RESOURCES->GetOrAddTexture(L"ClipIcon", L"../Resources/Textures/Clip_Icon.png");

    // 폴더 스캔
    ScanFolder(L"../Resources", _rootFolder);
    _currentFolder = &_rootFolder;
}

void ContentBrowserView::Update()
{
    EditorWindow::Update();
}

void ContentBrowserView::OnGui()
{
    ImGui::Begin("Content Browser");

    auto& entries = GET_SINGLE(ModelRegistry)->GetAll();

    float iconSize = 22.f;
    ImTextureID texID = (ImTextureID)_folderIcon->GetComPtr().Get();

    // === 왼쪽 패널 (폴더 트리) ===
    ImGui::BeginChild("FolderTree", ImVec2(_leftPanelWidth, 0), true);
    {
        ImGui::SetNextItemWidth(-1);
        ImGui::InputTextWithHint("##Search", "Search...", _searchBuffer, 256);
        ImGui::Separator();

        // 폴더 트리 표시
        DrawFolderTree(_rootFolder, texID, iconSize);
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // 드래그 가능한 구분선 (Splitter)
    ImGui::InvisibleButton("##splitter", ImVec2(4.f, ImGui::GetContentRegionAvail().y));
    if (ImGui::IsItemActive())
    {
        _leftPanelWidth += ImGui::GetIO().MouseDelta.x;
        _leftPanelWidth = std::clamp(_leftPanelWidth, 100.f, 250.f);  
    }
    if (ImGui::IsItemHovered())
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

    ImGui::SameLine();

    // === 오른쪽 패널 ===
    if (ImGui::BeginChild("AssetView", ImVec2(0, 0), true))
    {
        if (_currentFolder != nullptr)
        {
            string folderName = Utils::ToString(_currentFolder->name);
            ImGui::Text("Folder: %s", folderName.c_str());
            ImGui::Separator();

            // 그리드 레이아웃 설정
            float panelWidth = ImGui::GetContentRegionAvail().x;
            int columns = (int)(panelWidth / (_thumbnailSize + 16.f));  // 16 = 패딩
            if (columns < 1) columns = 1;

            ImGui::Columns(columns, nullptr, false);
            // .clip 파일만 표시
            for (auto& filePath : _currentFolder->files)
            {
                namespace fs = std::filesystem;

                if (fs::path(filePath).extension() != L".clip")
                    continue;
                wstring fileName = fs::path(filePath).filename().wstring();
                wstring fileNameNoExt = fs::path(filePath).stem().wstring();  // 확장자 제거
                string fileNameStr = Utils::ToString(fileNameNoExt);
                ImGui::PushID(fileNameStr.c_str());

                // 썸네일 버튼
                ImTextureID iconTex = (ImTextureID)_clipIcon->GetComPtr().Get();
                ImGui::ImageButton("##btn", iconTex, ImVec2(_thumbnailSize, _thumbnailSize));

                // 더블클릭 처리
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                {
                    // 기존 더블클릭 로직 그대로...
                    auto& entries = GET_SINGLE(ModelRegistry)->GetAll();
                    auto iter = entries.find(_currentFolder->name);

                    if (iter != entries.end())
                    {
                        auto animView = GET_ANIMATION_VIEW();
                        if (animView)
                        {
                            int animIndex = 0;
                            for (size_t j = 0; j < iter->second.animPaths.size(); j++)
                            {
                                if (iter->second.animPaths[j].find(fileName) != wstring::npos)
                                {
                                    animIndex = (int)j;
                                    break;
                                }
                            }

                            animView->SetAnimation(iter->second.model, animIndex, iter->second.animPaths);
                            animView->SetActive(true);
                        }

                        auto detailView = GET_ANIMATION_DETAILS();
                        if (detailView)
                            detailView->SetActive(true);
                    }
                }
                // 파일명 표시 (썸네일 아래)
                // 가운데 정렬 버전
                float textWidth = ImGui::CalcTextSize(fileNameStr.c_str()).x;
                float columnWidth = ImGui::GetColumnWidth();
                float offset = (columnWidth - textWidth) * 0.5f;
                if (offset > 0)
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
                ImGui::Text("%s", fileNameStr.c_str());
                ImGui::PopID();
                ImGui::NextColumn();
            }
            ImGui::Columns(1);
        }
        else
        {
            ImGui::Text("Select a folder from the left panel");
        }
    }
    ImGui::EndChild();
    ImGui::End();

    // Legacy
    {
        //for (auto& [modelName, entry] : entries)
    //{
    //    string modelNameStr = Utils::ToString(modelName);

    //    // 폴더 아이콘
    //    ImGui::Image(texID, ImVec2(iconSize, iconSize));
    //    ImGui::SameLine();



    //    bool open = ImGui::TreeNodeEx(modelNameStr.c_str(), ImGuiTreeNodeFlags_OpenOnArrow);

    //    if (open)
    //    {
    //        auto& animations = entry.model->GetAnimations();

    //        for (size_t i = 0; i < animations.size(); i++)
    //        {
    //            wstring animNameW = FileUtils::PathToAnimName(entry.animPaths[i]);
    //            string animName = Utils::ToString(animNameW);

    //            // 들여쓰기 + 텍스트만
    //            ImGui::Indent(iconSize + 4.f);

    //            bool isSelected = (_selectedIndex == (int)i);
    //            if (ImGui::Selectable(animName.c_str(), isSelected))
    //            {
    //                _selectedIndex = (int)i;
    //            }

    //            // 더블 클릭 -> Animation View, Animation Details View 보여주기
    //            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
    //            {
    //                auto animView = dynamic_pointer_cast<AnimationView>(
    //                    GET_SINGLE(EditorManager)->GetWindow(L"Animation"));

    //                if (animView)
    //                {
    //                    animView->SetAnimation(entry.model, (int)i, entry.animPaths);
    //                    //animView->SetModelAnimator(entry.animator);
    //                    animView->SetActive(true);
    //                }

    //                auto detailView = GET_SINGLE(EditorManager)->GetWindow(L"Animation Details");
    //                if (detailView)
    //                    detailView->SetActive(true);
    //            }

    //            ImGui::Unindent(iconSize + 4.f);
    //        }
    //        ImGui::TreePop();
    //    }
    //}
    }



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

void ContentBrowserView::ScanFolder(const wstring& path, FolderNode& node)
{
    namespace fs = std::filesystem;

    node.fullPath = path;
    node.name = fs::path(path).filename().wstring();

    if (!fs::exists(path))
        return;

    for (auto& entry : fs::directory_iterator(path))
    {
        if (entry.is_directory())
        {
            FolderNode child;
            ScanFolder(entry.path().wstring(), child);
            node.subFolders.push_back(child);
        }
        else
        {
            node.files.push_back(entry.path().wstring());
        }
    }
}

void ContentBrowserView::DrawFolderTree(FolderNode& node, ImTextureID folderIcon, float iconSize)
{
    bool hasMesh = false;
    for (auto& file : node.files)
    {
        if (file.ends_with(L".mesh"))
        {
            hasMesh = true;
            break;
        }
    }

    // .mesh가 있는 폴더이거나 하위에 .mesh 폴더가 있으면 표시
    if (hasMesh)
    {
        string folderName = Utils::ToString(node.name);

        // 검색 필터링
        if (strlen(_searchBuffer) > 0)
        {
            // 대소문자 무시 검색
            string searchLower = _searchBuffer;
            string nameLower = folderName;

            // 소문자로 변환
            std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);
            std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);

            // 검색어가 포함되지 않으면 스킵
            if (nameLower.find(searchLower) == string::npos)
            {
                // 하위 폴더도 검색
                for (auto& child : node.subFolders)
                    DrawFolderTree(child, folderIcon, iconSize);
                return;
            }
        }

        ImGui::Image(folderIcon, ImVec2(iconSize, iconSize));
        ImGui::SameLine();

        bool isSelected = (_currentFolder == &node);

        if (ImGui::Selectable(folderName.c_str(), isSelected))
        {
            _currentFolder = &node;
        }
    }

    // 하위 폴더 재귀 탐색
    for (auto& child : node.subFolders)
    {
        DrawFolderTree(child, folderIcon, iconSize);
    }
}
