#pragma once

#include "EditorWindow.h"

class Model;

struct AnimationEntry
{
    wstring name;
    shared_ptr<Model> model;
    shared_ptr<ModelAnimator> animator;

    int animIndex;

    vector<wstring> animPaths;  
};

struct FolderNode
{
    wstring name;
    wstring fullPath;

    vector<FolderNode> subFolders;
    vector<wstring> files;  // 폴더 내 파일들
};

class ContentBrowserView : public EditorWindow
{
public:
    ContentBrowserView();
    virtual ~ContentBrowserView();

public:
    void Init() override;
    void Update() override;
    void OnGui() override;

    void AddAnimation(
        const wstring& name,
        shared_ptr<Model> model,
        shared_ptr<ModelAnimator> animator,
        const vector<wstring>& animPaths,
        int animIndex);

    void ScanFolder(const wstring& path, FolderNode& node);
    void DrawFolderTree(FolderNode& node, ImTextureID folderIcon, float iconSize);

private:
    vector<AnimationEntry> _animations;
    int _selectedIndex = -1;

    shared_ptr<Texture> _folderIcon;

private:
    // 폴더 관련
    wstring         _basePath = L"../Resources";
    wstring         _selectedFolder;

    FolderNode      _rootFolder;
    FolderNode*     _currentFolder;

    char            _searchBuffer[256] = "";
    float           _leftPanelWidth = 200.f; \

    // 오른쪽 그리드 뷰
    float _thumbnailSize = 48.f;
    shared_ptr<Texture> _clipIcon;

};

