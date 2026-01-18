#pragma once

#include "EditorWindow.h"

class AnimationDetailView : public EditorWindow
{
public:
    AnimationDetailView();
    virtual ~AnimationDetailView();

public:
    virtual void Init() override;
    virtual void Update() override;
    virtual void OnGui() override;

    // Animation View에서 호출
    void SetContext(const wstring& animName, int selectedNotifyIdx, int selectedStateIdx);

private:
    void DrawNotifyInspector();
    void DrawAddNotifySection();

    void DrawNotifyStateInspector();
    void DrawAddNotifyStateSection();

    void DrawAnimationProperties();

private:
    wstring _currentAnimName;
    int     _selectedNotifyIndex = -1;
    int     _selectedNotifyStateIndex = -1;

    // Notify 새로 추가할 때 -> 원래 AnimView에 있던거 이동
    int     _newNotifyFrame = 0;
    int     _newStateStartFrame = 0;
    int     _newStateEndFrame = 10;


};

