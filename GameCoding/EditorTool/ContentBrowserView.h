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

/// <summary>
/// Content Browser 창
/// - 애니메이션 파일 목록 표시
/// - 더블클릭하면 AnimationView 열기
/// </summary>
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

private:

    vector<AnimationEntry> _animations;
    int _selectedIndex = -1;

};

