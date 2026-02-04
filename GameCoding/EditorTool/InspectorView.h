#pragma once

#include "EditorWindow.h"

class GameObject;

/// <summary>
/// Inspector 창
/// - 선택된 GameObject의 컴포넌트 표시/편집
/// - Transform, Mesh, Material 등
/// </summary>
class InspectorView : public EditorWindow
{
public:
    InspectorView();
    virtual ~InspectorView();

public:
    void Init() override;
    void Update() override;
    void OnGui() override;

    void SetTarget(shared_ptr<GameObject> target) { _target = target; }

private:
    void DrawTransform();

private:
    shared_ptr<GameObject> _target;

};

