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
    shared_ptr<GameObject> GetSelectedObject() const { return _selectedObject; }
    void SetSelectedObject(shared_ptr<GameObject> obj) { _selectedObject = obj; }

    // 씬에 오브젝트 추가 (이걸 여기서 해야하나 ?)
    void AddObject(shared_ptr<GameObject> obj);
    void RemoveObject(shared_ptr<GameObject> obj);


private:
    // 씬의 모든 오브젝트 목록
    vector<shared_ptr<GameObject>> _sceneObjects;

    // 현재 선택된 오브젝트
    shared_ptr<GameObject> _selectedObject;

};

