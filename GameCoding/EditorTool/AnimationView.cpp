#include "pch.h"
#include "imgui_internal.h"
#include "AnimationView.h"

#include <filesystem>

#include "Model.h"
#include "ModelAnimation.h"
#include "ImSequencer.h"
#include "ModelAnimator.h"
#include "RenderTarget.h"
#include "Camera.h"
#include "Utils.h"
#include "FileUtils.h"
#include "AnimNotifyManager.h"

void AnimSequence::Get(int index, int** start, int** end, int* type, unsigned* color)
{
    if (!start || !end)
        return;

    static int _start = 0;
    static int _end = 100;
    _end = frameMax;
    *start = &_start;
    *end = &_end;

    if (type)
        *type = 0;

    if (color)
        *color = 0xFF4488AA;
}


void AnimSequence::CustomDraw(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& legendRect,
    const ImRect& clippingRect, const ImRect& legendClippingRect)
{
    if (!notifyContainer)
        return;

    float frameCount = (float)(frameMax - frameMin);
    if (frameCount <= 0)
        return;

    float pixelPerFrame = (rc.Max.x - rc.Min.x) / frameCount;

    // 단일 Notify - 빨간 사각형 마커 (언리얼 스타일)
    for (auto& notify : notifyContainer->notifies)
    {
        float x = rc.Min.x + (notify.frame - frameMin) * pixelPerFrame;
        float markerWidth = 6.f;
        float markerHeight = rc.Max.y - rc.Min.y - 4.f;

        // 배경 사각형
        draw_list->AddRectFilled(
            ImVec2(x - markerWidth * 0.5f, rc.Min.y + 2),
            ImVec2(x + markerWidth * 0.5f, rc.Min.y + 2 + markerHeight),
            0xFF2222DD);  // 빨간색 (ABGR)

        // 테두리
        draw_list->AddRect(
            ImVec2(x - markerWidth * 0.5f, rc.Min.y + 2),
            ImVec2(x + markerWidth * 0.5f, rc.Min.y + 2 + markerHeight),
            0xFFFFFFFF);  // 흰색 테두리
    }

    // NotifyState - 주황 바 (구간)
    for (auto& state : notifyContainer->notifyStates)
    {
        float x1 = rc.Min.x + (state.startFrame - frameMin) * pixelPerFrame;
        float x2 = rc.Min.x + (state.endFrame - frameMin) * pixelPerFrame;

        // 구간 배경
        draw_list->AddRectFilled(
            ImVec2(x1, rc.Max.y - 10),
            ImVec2(x2, rc.Max.y - 2),
            0xFFFF8822);  // 주황색 (ABGR)

        // 테두리
        draw_list->AddRect(
            ImVec2(x1, rc.Max.y - 10),
            ImVec2(x2, rc.Max.y - 2),
            0xFFFFFFFF);  // 흰색 테두리
    }
}

AnimationView::AnimationView()
    : EditorWindow("Animation")
{
    _isActive = false;
}

AnimationView::~AnimationView()
{
}

void AnimationView::Init()
{
    // ─────────────────────────────────────────────
    // 셰이더 로드 (ModelAnimator에 필요)
    // ─────────────────────────────────────────────
    _shader = make_shared<Shader>(L"17. TweenDemo.fx");

    // 프리뷰용 RenderTarget 생성
    _previewRenderTarget = make_shared<RenderTarget>();
    _previewRenderTarget->Create(100, 400);

    // 프리뷰 카메라 설정
    _previewCamera = make_shared<GameObject>();
    _previewCamera->GetOrAddTransform()->SetPosition(Vec3(0, 3, -15));
    _previewCamera->AddComponent(make_shared<Camera>());


}

void AnimationView::Update()
{
    // 스페이스바로 재생/정지 토글
    if (_isFocused && INPUT->GetButtonDown(KEY_TYPE::SPACE))
    {
        _isPlaying = !_isPlaying;
    }

    if (_isFocused)
        UpdateCameraInput();

    if (_isPlaying && _model)
    {
        _playbackTime += DT;

        // 프레임 계산 (30fps 기준)
        _currentFrame = (int)(_playbackTime * 30.f);

        if (_currentFrame > _frameMax)
        {
            _currentFrame = _frameMin;
            _playbackTime = 0.f;
        }
    }

}

void AnimationView::OnGui()
{
    if (!ImGui::Begin("Animation", &_isActive, ImGuiWindowFlags_MenuBar))
    {
        ImGui::End();
        return;
    }

    _isFocused = ImGui::IsWindowFocused();
    _isHovered = ImGui::IsWindowHovered();

    DrawAnimationList();
    ImGui::SameLine();

    ImGui::BeginGroup();
    {
        // 프리뷰 영역
        DrawPreview();
        ImGui::Separator();

        DrawControls();
        DrawSequencer();
        DrawNotifyPanel();
    }
    ImGui::EndGroup();

    ImGui::End();
}

void AnimationView::SetAnimation(shared_ptr<Model> model, int animIndex, vector<wstring>& animPaths)
{
    _model = model;  // 원본 참조용
    _animIndex = animIndex;
    _currentFrame = 0;
    _playbackTime = 0.f;
    _animPaths = animPaths;

    _isPlaying = true;

    // 애니메이션 정보 가져오기
    _animNames.clear();
    for (const auto& path : _animPaths)
    {
        _animNames.push_back(FileUtils::PathToAnimName(path));
    }
    if (_model && _model->GetAnimations().size() > animIndex)
    {
        auto& anim = _model->GetAnimations()[animIndex];
        _frameMax = anim->frameCount;
    }

    if (_animNames.size() > _animIndex)
    {
        wstring animName = _animNames[_animIndex];
        wstring filePath = L"../Resources/Notifies/" + animName + L".json";

        // 파일이 존재하면 로드
        if (std::filesystem::exists(filePath))
        {
            GET_SINGLE(AnimNotifyManager)->LoadFromJson(filePath);
        }
    }
}

void AnimationView::DrawPreview()
{
    if (!_previewRenderTarget || !_modelAnimator || !_model)
    {
        ImGui::Text("No animation to preview");
        return;
    }

    ImVec2 previewSize = ImVec2(1000, 400);
    _previewRenderTarget->Resize((uint32)previewSize.x, (uint32)previewSize.y);

    _previewRenderTarget->BindAsTarget();
    _previewRenderTarget->Clear(Color(0.15f, 0.15f, 0.15f, 1.f));

    // 카메라 설정
    _previewCamera->GetTransform()->UpdateTransform();
    auto camera = _previewCamera->GetCamera();
    camera->SetWidth(previewSize.x);
    camera->SetHeight(previewSize.y);
    camera->UpdateMatrix();
    RENDER->PushGlobalData(Camera::S_MatView, Camera::S_MatProjection);

    // 라이트 설정
    LightDesc lightDesc;
    lightDesc.ambient = Vec4(0.5f);
    lightDesc.diffuse = Vec4(1.f);
    lightDesc.specular = Vec4(0.f);
    lightDesc.direction = Vec3(1.f, -1.f, 1.f);
    RENDER->PushLightData(lightDesc);

    auto owner = _modelAnimator->GetGameObject();
    if (owner)
    {
        auto originalPos = owner->GetTransform()->GetPosition();
        owner->GetTransform()->SetPosition(Vec3(0, 0, 0));
        owner->GetTransform()->UpdateTransform();

        TweenDesc tweenDesc;
        tweenDesc.curr.animIndex = _animIndex;
        tweenDesc.curr.currFrame = min(_currentFrame, max(0, _frameMax - 1));
        tweenDesc.curr.nextFrame = tweenDesc.curr.currFrame;
        tweenDesc.curr.ratio = 0.f;
        tweenDesc.curr.sumTime = 0.f; 
        tweenDesc.curr.speed = 0.f;   
        _modelAnimator->SetTweenDesc(tweenDesc);
        _modelAnimator->Update();

        owner->GetTransform()->SetPosition(originalPos);
    }

    RenderTarget::UnbindAll();

    ImGui::Text("Preview");
    ImGui::Image(_previewRenderTarget->GetSRV(), previewSize);
}

void AnimationView::DrawControls()
{
    // 재생 컨트롤
    if (ImGui::Button(_isPlaying ? "||" : ">"))
    {
        _isPlaying = !_isPlaying;
    }
    ImGui::SameLine();

    if (ImGui::Button("<<"))
    {
        _currentFrame = _frameMin;
        _playbackTime = 0.f;
    }
    ImGui::SameLine();

    if (ImGui::Button(">>"))
    {
        _currentFrame = _frameMax;
    }
    ImGui::SameLine();

    // 프레임 슬라이더
    ImGui::SetNextItemWidth(200);
    if (ImGui::SliderInt("Frame", &_currentFrame, _frameMin, _frameMax))
    {
        _playbackTime = _currentFrame / 30.f;
        _isPlaying = false;
    }

    ImGui::Text("Animation Index : %d", _animIndex);

}

void AnimationView::DrawSequencer()
{
    if (!_model)
        return;

    ImGui::Separator();

    // 시퀀서 프레임 범위 동기화
    _sequence.frameMin = _frameMin;
    _sequence.frameMax = _frameMax;

    wstring animName = (_animNames.size() > _animIndex) ? _animNames[_animIndex] : L"";
    _sequence.notifyContainer = GET_SINGLE(AnimNotifyManager)->GetContainer(animName);

    _firstFrame = 0;

    // ImSequencer 그리기
    ImSequencer::Sequencer(
        &_sequence,
        &_currentFrame,
        &_sequencerExpanded,
        &_selectedEntry,
        &_firstFrame,
        ImSequencer::SEQUENCER_EDIT_STARTEND | ImSequencer::SEQUENCER_CHANGE_FRAME
    );

}

void AnimationView::DrawAnimationList()
{
    ImGui::BeginChild("AnimList", ImVec2(200, 0), true);

    ImGui::Text("Animations");
    ImGui::Separator();

    for (int i = 0; i < _animNames.size(); i++)
    {
        string name = Utils::ToString(_animNames[i]);

        // 이름이 비어있으면 인덱스로 대체
        if (name.empty())
            name = "Animation_" + to_string(i);

        bool isSelected = (_animIndex == i);

        if (ImGui::Selectable(name.c_str(), isSelected))
        {
            _animIndex = i;
            _currentFrame = 0;
            _playbackTime = 0.f;

            // 프레임 범위 업데이트
            if (_model && _model->GetAnimations().size() > i)
            {
                auto& anim = _model->GetAnimations()[i];
                _frameMax = anim->frameCount;
            }
        }
    }

    ImGui::EndChild();
}

void AnimationView::DrawNotifyPanel()
{
    if (!_model)
        return;

    ImGui::Separator();
    ImGui::Text("Notifies");

    // 현재 애니메이션 이름 가져오기
    wstring animName = L"";
    if (_animNames.size() > _animIndex)
        animName = _animNames[_animIndex];

    if (animName.empty())
        return;

    auto* container = GET_SINGLE(AnimNotifyManager)->GetContainer(animName);

    // ─────────────────────────────────────────────
    // Notify 목록
    // ─────────────────────────────────────────────
    ImGui::Text("Single Frame Notifies :");

    if (container)
    {
        for (int i = 0; i < (int)container->notifies.size(); i++)
        {
            auto& notify = container->notifies[i];
            string label = Utils::ToString(notify.name) + " (Frame : " +
                to_string(notify.frame) + ")";

            bool isSelected = (_selectedNotifyIndex == i);
            if (ImGui::Selectable(label.c_str(), isSelected))
            {
                _selectedNotifyIndex = i;
                _currentFrame = notify.frame; // 해당 프레임으로 이동
            }
        }
    }

    // Notify 추가 UI
    ImGui::InputText("Name##Notify", _newNotifyName, 128);
    ImGui::InputInt("Frame##Notify", &_newNotifyFrame);

    if (ImGui::Button("Add Notify"))
    {
        if (strlen(_newNotifyName) > 0)
        {
            AnimNotifyData notify;
            notify.name = Utils::ToWString(string(_newNotifyName));
            notify.frame = _newNotifyFrame;

            GET_SINGLE(AnimNotifyManager)->AddNotify(animName, notify);
            memset(_newNotifyName, 0, 128);
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Remove Notify") && _selectedNotifyIndex >= 0)
    {
        GET_SINGLE(AnimNotifyManager)->RemoveNotify(animName, _selectedNotifyIndex);
        _selectedNotifyIndex = -1;
    }

    ImGui::Separator();

    // ─────────────────────────────────────────────
    // NotifyState 목록
    // ─────────────────────────────────────────────
    ImGui::Text("Notify States:");

    if (container)
    {
        for (int i = 0; i < (int)container->notifyStates.size(); i++)
        {
            auto& state = container->notifyStates[i];
            string label = Utils::ToString(state.name) + " (" + to_string(state.startFrame) + " - " + to_string(state.endFrame) + ")";

            bool isSelected = (_selectedNotifyStateIndex == i);
            if (ImGui::Selectable(label.c_str(), isSelected))
            {
                _selectedNotifyStateIndex = i;
                _currentFrame = state.startFrame;
            }
        }
    }

    // NotifyState 추가 UI
    ImGui::InputInt("Start##State", &_newNotifyStateStartFrame);
    ImGui::InputInt("End##State", &_newNotifyStateEndFrame);

    if (ImGui::Button("Add NotifyState"))
    {
        if (strlen(_newNotifyName) > 0)
        {
            AnimNotifyStateData state;
            state.name = Utils::ToWString(string(_newNotifyName));
            state.startFrame = _newNotifyStateStartFrame;
            state.endFrame = _newNotifyStateEndFrame;
            GET_SINGLE(AnimNotifyManager)->AddNotifyState(animName, state);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Remove State") && _selectedNotifyStateIndex >= 0)
    {
        GET_SINGLE(AnimNotifyManager)->RemoveNotifyState(animName, _selectedNotifyStateIndex);
        _selectedNotifyStateIndex = -1;
    }

    // ─────────────────────────────────────────────
    // 저장 버튼
    // ─────────────────────────────────────────────
    ImGui::Separator();
    if (ImGui::Button("Save Notifies"))
    {
        wstring filePath = L"../Resources/Notifies/" + animName + L".json";
        GET_SINGLE(AnimNotifyManager)->SaveToJson(animName, filePath);
    }

}

void AnimationView::UpdateCameraInput()
{
    if (!_previewCamera)
        return;
    auto transform = _previewCamera->GetTransform();
    Vec3 pos = transform->GetPosition();
    Vec3 rot = transform->GetRotation();

    float speedMultiplier = INPUT->GetButton(KEY_TYPE::SHIFT) ? 3.0f : 1.0f;
    float currentSpeed = _moveSpeed * speedMultiplier;

    POINT currentMouse = INPUT->GetMousePos();

    if (INPUT->GetButton(KEY_TYPE::RBUTTON))
    {
        float deltaX = (float)(currentMouse.x - _lastMousePos.x);
        float deltaY = (float)(currentMouse.y - _lastMousePos.y);

        rot.y += deltaX * _rotSpeed;
        rot.x += deltaY * _rotSpeed;

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


