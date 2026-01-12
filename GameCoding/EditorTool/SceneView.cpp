#include "pch.h"
#include "SceneView.h"
#include "Camera.h"
#include "Engine/RenderTarget.h"
#include "Transform.h"
#include "HierarchyView.h"
#include "EditorManager.h"

SceneView::SceneView()
    : EditorWindow("Scene")
{

}

SceneView::~SceneView()
{

}

void SceneView::Init()
{
   // ─────────────────────────────────────────────
   // RenderTarget 생성 (초기 크기는 임시로 설정)
   // ─────────────────────────────────────────────
    _renderTarget = make_shared<RenderTarget>();
    _renderTarget->Create(1280, 720);

    // 에디터 카메라 생성
    _editorCamera = make_shared<GameObject>();
    _editorCamera->GetOrAddTransform()->SetPosition(Vec3(0.f, 5.f, -10.f));
    _editorCamera->AddComponent(make_shared<Camera>());
}

void SceneView::Update()
{
    UpdateCameraInput();
    
    if (_editorCamera)
        _editorCamera->Update();

    UpdateCameraLerp();
}

void SceneView::OnGui()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Scene");

    auto viewportMin = ImGui::GetWindowContentRegionMin();  // 콘텐츠 영역 좌상단 (윈도우 로컬)
    auto viewportMax = ImGui::GetWindowContentRegionMax();  // 콘텐츠 영역 우하단 (윈도우 로컬)
    auto windowPos = ImGui::GetWindowPos();                 // 윈도우 위치 (스크린)

    // 스크린 좌표로 변환
    _viewportBounds[0] = Vec2(viewportMin.x + windowPos.x, viewportMin.y + windowPos.y);
    _viewportBounds[1] = Vec2(viewportMax.x + windowPos.x, viewportMax.y + windowPos.y);

    _isFocused = ImGui::IsWindowFocused();
    _isHovered = ImGui::IsWindowHovered();

    // 뷰포트 크기 계산
    ImVec2 panelSize = ImGui::GetContentRegionAvail();
    _viewportSize = Vec2(panelSize.x, panelSize.y);

    // RenderTarget 크기 조정
    if (_renderTarget && panelSize.x > 0 && panelSize.y > 0)
    {
        _renderTarget->Resize((uint32)panelSize.x, (uint32)panelSize.y);

        // ─────────────────────────────────────────────
        // RenderTarget 텍스처를 ImGui::Image로 표시
        // ─────────────────────────────────────────────
        ImGui::Image(_renderTarget->GetSRV(), panelSize);

        UpdateImGuiZmo();
    }

    if (_editorCamera && panelSize.x > 0 && panelSize.y > 0)
    {
        auto camera = _editorCamera->GetCamera();
        camera->SetWidth(panelSize.x);
        camera->SetHeight(panelSize.y);
    }

    ImGui::PopStyleVar();
    ImGui::End();

}

void SceneView::FocusOnPosition(const Vec3& targetPos)
{
    _isCameraLerping = true;
    _lerpTargetPos = targetPos;
}

void SceneView::UpdateCameraLerp()
{
    if (!_isCameraLerping || !_editorCamera)
        return;

    auto transform = _editorCamera->GetTransform();
    Vec3 currentPos = transform->GetPosition();

    // 타겟 뒤쪽에서 바라보는 위치 계산
    Vec3 targetCamPos = _lerpTargetPos - transform->GetLook() * _lerpDistance;
    targetCamPos.y = _lerpTargetPos.y + 4.f;

    // Lerp 이동
    Vec3 newPos = Vec3::Lerp(currentPos, targetCamPos, DT * 5.f);
    transform->SetPosition(newPos);

    // 거의 도착하면 종료
    float distance = (newPos - targetCamPos).Length();
    if (distance < 0.1f)
    {
        _isCameraLerping = false;
    }

}

void SceneView::UpdateCameraInput()
{
    if (!_editorCamera || !_isFocused)
        return;

    auto transform = _editorCamera->GetTransform();
    Vec3 pos = transform->GetPosition();
    Vec3 rot = transform->GetRotation();

    // 속도 배율 (Shift 누르면 빠르게)
    float speedMultiplier = INPUT->GetButton(KEY_TYPE::SHIFT) ? 3.0f : 1.0f;
    float currentSpeed = _moveSpeed * speedMultiplier;

    // 우클릭 드래그로 카메라 조작
    POINT currentMouse = INPUT->GetMousePos();

    if (INPUT->GetButton(KEY_TYPE::RBUTTON))
    {
        // 마우스 회전
        float deltaX = (float)(currentMouse.x - _lastMousePos.x);
        float deltaY = (float)(currentMouse.y - _lastMousePos.y);

        rot.y += deltaX * _rotSpeed;
        rot.x += deltaY * _rotSpeed;

        // WASD 이동 (우클릭 중에만)
        if (INPUT->GetButton(KEY_TYPE::W))
            pos += transform->GetLook() * currentSpeed * DT;

        if (INPUT->GetButton(KEY_TYPE::S))
            pos -= transform->GetLook() * currentSpeed * DT;

        if (INPUT->GetButton(KEY_TYPE::A))
            pos -= transform->GetRight() * currentSpeed * DT;

        if (INPUT->GetButton(KEY_TYPE::D))
            pos += transform->GetRight() * currentSpeed * DT;

        if (INPUT->GetButton(KEY_TYPE::Q))
            pos.y -= currentSpeed * DT;

        if (INPUT->GetButton(KEY_TYPE::E))
            pos.y += currentSpeed * DT;
    }

    _lastMousePos = currentMouse;
    transform->SetPosition(pos);
    transform->SetRotation(rot);

}

void SceneView::UpdateImGuiZmo()
{
    // ─────────────────────────────────────────────
    // ImGuizmo 설정
    // ─────────────────────────────────────────────
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(_viewportBounds[0].x, _viewportBounds[0].y,
        _viewportSize.x, _viewportSize.y);

    // 단축키로 모드 변환
    if (!ImGuizmo::IsUsing() && !INPUT->GetButton(KEY_TYPE::RBUTTON))
    {
        if (INPUT->GetButtonDown(KEY_TYPE::W))
            _gizmoOperation = ImGuizmo::TRANSLATE;

        if (INPUT->GetButtonDown(KEY_TYPE::E))
            _gizmoOperation = ImGuizmo::ROTATE;

        if (INPUT->GetButtonDown(KEY_TYPE::R))
            _gizmoOperation = ImGuizmo::SCALE;
    }

    // ─────────────────────────────────────────────
    // Hierarchy에서 선택된 오브젝트 가져오기
    // ─────────────────────────────────────────────

    auto hierarchy = dynamic_pointer_cast<HierarchyView>(
        GET_SINGLE(EditorManager)->GetWindow(L"Hierarchy"));

    if (!hierarchy) return;

    auto selectedObject = hierarchy->GetSelectedObject();
    if (!selectedObject) return;

    // ─────────────────────────────────────────────
    // ImGuizmo로 오브젝트 조작
    // ─────────────────────────────────────────────
    Matrix view = Camera::S_MatView;
    Matrix proj = Camera::S_MatProjection;
    Matrix world = selectedObject->GetTransform()->GetWorldMatrix();

    ImGuizmo::Manipulate(
        (float*)&view,
        (float*)&proj,
        _gizmoOperation,
        _gizmoMode,
        (float*)&world
    );

    // Guizmo 사용중이면 Transform 업데이트
    if (ImGuizmo::IsUsing())
    {
        Vec3 pos, rot, scale;
        ImGuizmo::DecomposeMatrixToComponents((float*)&world, (float*)&pos,
            (float*)&rot, (float*)&scale);

        selectedObject->GetTransform()->SetPosition(pos);
        selectedObject->GetTransform()->SetRotation(rot * (3.14159f / 180.0f));  // degree → radian
        selectedObject->GetTransform()->SetScale(scale);

    }

}
