#pragma once

#include "EditorWindow.h"

class GameObject;

/// <summary>
/// Hierarchy 창
/// - 씬의 모든 GameObject 목록 표시
/// - 클릭하면 선택됨 (Inspector에서 편집 가능)
/// </summary>
class HierarchyView : public EditorWindow
{
public:
    HierarchyView();
    virtual ~HierarchyView();

public:
    void Init() override;
    void Update() override;
    void OnGui() override;

    // 선택된 오브젝트
    const vector<shared_ptr<GameObject>>& GetSelectedObjects() const { return _selectedObjects; }
    // 단일 선택
    shared_ptr<GameObject> GetSelectedObject() const;

    void SetSelectedObject(shared_ptr<GameObject> obj);
    void AddSelectedObjects(shared_ptr<GameObject> obj);
    void RemoveSelectedObject(shared_ptr<GameObject> obj);

    void ClearSelection();
    bool IsSelected(shared_ptr<GameObject> obj) const;

    void AddSelectionTowards(int direction);
    void MoveSelection(int direction);
    int  GetCurrentSelectedIndex();

    // 씬에 오브젝트 추가 (이걸 여기서 해야하나 ?)
    void AddObject(shared_ptr<GameObject> obj);
    void RemoveObject(shared_ptr<GameObject> obj);

    shared_ptr<GameObject> CloneGameObject(shared_ptr<GameObject> original);

    const vector<shared_ptr<GameObject>>& GetSceneObjects() const { return _sceneObjects; }
    void ClearSceneObjects() { _sceneObjects.clear(); _selectedObjects.clear(); }

private:
    void HandleInput();
    void DrawToolbar();
    void DrawObjectList();

    shared_ptr<GameObject> EnsureUniqueName(GameObjectCreateEvent event);

private:
    // 씬의 모든 오브젝트 목록
    vector<shared_ptr<GameObject>> _sceneObjects;

    // 현재 선택된 오브젝트
    vector<shared_ptr<GameObject>> _selectedObjects;

    // 클립보드 (복사된 오브젝트 원본 참조)
    vector<shared_ptr<GameObject>> _clipboard;

    // 키보드 포커스
    int _keyboardFocuseIndex = -1;
    float _keyRepeatTimer = 0.f;
    float _keyRepeatDelay = 0.12f;

};



