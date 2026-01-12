#pragma once

#include "EditorWindow.h"

class RenderTarget;
class GameObject; 

/// <summary>
/// Scene View 윈도우
/// - 에디터 전용 카메라로 씬을 렌더링
/// - 나중에 ImGuizmo로 오브젝트 조작 기능 추가 예정
/// </summary>
class SceneView : public EditorWindow
{
public:
    SceneView();
    virtual ~SceneView();

public:
    void Init() override;
    void Update() override;
    void OnGui() override;

    /// <summary>
    /// 이 RenderTarget에 씬을 렌더링해야 함
    /// </summary>
    shared_ptr<RenderTarget> GetRenderTarget() const { return _renderTarget; }
    shared_ptr<GameObject> GetEditorCamera() const { return _editorCamera; }  // 추가

    // 카메라 Lerp 이동
    void FocusOnPosition(const Vec3& targetPos);
    void UpdateCameraLerp();

private:
    void UpdateCameraInput();
    void UpdateImGuiZmo();

private:
    // ─────────────────────────────────────────────
    // RenderTarget (씬 렌더링 결과 저장)
    // ─────────────────────────────────────────────
    shared_ptr<RenderTarget> _renderTarget;
    shared_ptr<GameObject> _editorCamera;

    // ─────────────────────────────────────────────
    // 뷰포트 영역 좌표 (ImGuizmo에서 사용)
    // [0] 좌상단, [1] 우하단
    // ─────────────────────────────────────────────
    Vec2 _viewportBounds[2] = {};
    Vec2 _viewportSize = {};

    bool _isHovered = false;
    bool _isFocused = false;

    // 카메라 이동 속도
    float _moveSpeed = 10.f;
    float _rotSpeed = 0.003f;

    POINT _lastMousePos = {};

    bool _isCameraLerping = false;
    Vec3 _lerpTargetPos;
    float _lerpDistance = 15; // 타겟으로의 거리

private:
    // ─────────────────────────────────────────────
    // ImGuizmo: 3D 기즈모 (Unity처럼 오브젝트 조작용 화살표/원/박스)
    // ─────────────────────────────────────────────

    // 현재 조작 모드 (어떤 변환을 할지)
    // - TRANSLATE: 이동 (화살표)
    // - ROTATE: 회전 (원)
    // - SCALE: 크기 (박스)
    ImGuizmo::OPERATION _gizmoOperation = ImGuizmo::TRANSLATE;

    // 좌표계 모드
    // - LOCAL: 오브젝트 기준 (오브젝트가 회전하면 축도 같이 회전)
    // - WORLD: 월드 기준 (항상 XYZ 축 고정)
    ImGuizmo::MODE _gizmoMode = ImGuizmo::LOCAL;

};

