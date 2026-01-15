#include "pch.h"
#include "imgui_internal.h"
#include "AnimationView.h"
#include <filesystem>
#include "EditorManager.h"
#include "AnimationDetailView.h"
#include "Model.h"
#include "ModelAnimation.h"
#include "ImSequencer.h"
#include "ModelAnimator.h"
#include "RenderTarget.h"
#include "Camera.h"
#include "Utils.h"
#include "FileUtils.h"
#include "AnimNotifyManager.h"
#include "AnimNotifyState.h"
#include "AnimNotifyFactory.h"

void AnimSequence::Get(int index, int** start, int** end, int* type, unsigned* color)
{
    if (!start || !end)
        return;

    static int animStart = 0;
    static int animEnd = 100;
    static int notifyStart = 0;
    static int notifyEnd = 100;
    static int stateStart = 0;
    static int stateEnd = 100;

    animEnd = frameMax;
    notifyEnd = frameMax;
    stateEnd = frameMax;

    switch (index)
    {
    case 0:
        *start = &animStart;
        *end = &animEnd;
        if (color)
            *color = 0xFF4488AA; // 파란색 계열
        break;
    case 1:
        *start = &notifyStart;
        *end = &notifyEnd;
        break;
    case 2:
        *start = &stateStart;
        *end = &stateEnd;
        break;
    }

    if (type)
        *type = 0;
}

const char* AnimSequence::GetItemLabel(int index) const
{
    switch (index)
    {
    case 0:  return "Animation";
    case 1:  return "Notifies";
    case 2:  return "States";
    default: return "";
    }
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

    // ─────────────────────────────────────────────
    // index == 0: Animation 트랙
    // ─────────────────────────────────────────────
    if (index == 0)
        return;

    // ─────────────────────────────────────────────
    // index == 1: Notifies 트랙
    // ─────────────────────────────────────────────
    if (index == 1)
    {
        auto& io = ImGui::GetIO();

        for (int i = 0; i < notifyContainer->notifies.size(); i++)
        {
            auto& notify = notifyContainer->notifies[i];

            float x = rc.Min.x + (notify->GetFrame() - frameMin) * pixelPerFrame;
            float markerWidth = 40.f;
            float markerHeight = rc.Max.y - rc.Min.y - 4.f;

            ImVec2 rectMin(x, rc.Min.y + 2);
            ImVec2 rectMax(x + markerWidth, rc.Min.y + 2 + markerHeight);
            ImRect notifyRect(rectMin, rectMax);

            // Hover Check
            bool isHovered = notifyRect.Contains(io.MousePos);
            bool isSelected = (i == selectedNotifyIndex);

            // 배경 색상
            unsigned int bgColor = isHovered ? SeqColors::NotifyBgHover : SeqColors::NotifyBg;
            draw_list->AddRectFilled(rectMin, rectMax, bgColor);

            draw_list->AddRectFilled(rectMin, rectMax, 0xFF33CC33);

            // 테두리
            unsigned int borderColor = isSelected ? SeqColors::Selected : SeqColors::NotifyBorder;
            float borderThickness = isSelected ? 2.f : 1.f;
            draw_list->AddRect(rectMin, rectMax, borderColor, 0.f, 0, borderThickness);

            // 이름
            string name = Utils::ToString(notify->GetDisplayName());
            ImVec2 textSize = ImGui::CalcTextSize(name.c_str());

            // 가운데 좌표 계산
            float centerX = rectMin.x + (markerWidth - textSize.x) / 2.f;
            float centerY = rectMin.y + (markerHeight - textSize.y) / 2.f;

            draw_list->AddText(ImVec2(centerX, centerY), SeqColors::Text, name.c_str());

            if (notifyRect.Contains(io.MousePos) && ImGui::IsMouseClicked(0))
            {
                // 클릭 시작
                selectedNotifyIndex = i;
                draggingNotifyIndex = i;
                clickedOnNotify = true;

                dragOffset = io.MousePos.x - rectMin.x;
            }

            // 드래그 중
            if (draggingNotifyIndex == i && ImGui::IsMouseDown(0))
            {
                float relativeX = (io.MousePos.x - dragOffset) - rc.Min.x;
                int newFrame = frameMin + (int)(relativeX / pixelPerFrame);
                newFrame = max(frameMin, min(frameMax, newFrame));
                notifyContainer->notifies[i]->SetFrame(newFrame);
            }

            if (notifyRect.Contains(io.MousePos) && ImGui::IsMouseClicked(0))
            {
                selectedNotifyIndex = i;
                clickedOnNotify = true;
            }
        }

        if (!ImGui::IsMouseDown(0))
        {
            draggingNotifyIndex = -1;
        }

        return;
    }
    // ─────────────────────────────────────────────
    // index == 2: States 트랙
    // ─────────────────────────────────────────────
    if (index == 2)
    {
        auto& io = ImGui::GetIO();
        float handleWidth = 10.f;  

        for (int i = 0; i < notifyContainer->notifyStates.size(); i++)
        {
            auto& state = notifyContainer->notifyStates[i];

            float x1 = rc.Min.x + (state->GetStartFrame() - frameMin) * pixelPerFrame;
            float x2 = rc.Min.x + (state->GetEndFrame() - frameMin) * pixelPerFrame;
            float y1 = rc.Min.y + 2;
            float y2 = rc.Max.y - 2;

            ImRect fullRect(ImVec2(x1, y1), ImVec2(x2, y2));
            ImRect leftHandle(ImVec2(x1, y1), ImVec2(x1 + handleWidth, y2));
            ImRect rightHandle(ImVec2(x2 - handleWidth, y1), ImVec2(x2, y2));

            bool isHovered = fullRect.Contains(io.MousePos);
            bool isSelected = (i == selectedNotifyStateIndex);
            bool leftHandleHovered = leftHandle.Contains(io.MousePos);
            bool rightHandleHovered = rightHandle.Contains(io.MousePos);

            // 메인 배경
            unsigned int bgColor = isHovered ? SeqColors::StateBgHover : SeqColors::StateBg;
            draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), bgColor);

            // 핸들 배경
            unsigned int leftHandleColor = leftHandleHovered ? SeqColors::HandleBgHover : SeqColors::HandleBg;
            unsigned int rightHandleColor = rightHandleHovered ? SeqColors::HandleBgHover : SeqColors::HandleBg;
            draw_list->AddRectFilled(leftHandle.Min, leftHandle.Max, leftHandleColor);
            draw_list->AddRectFilled(rightHandle.Min, rightHandle.Max, rightHandleColor);

            // 테두리
            unsigned int borderColor = isSelected ? SeqColors::Selected : SeqColors::StateBorder;
            float borderThickness = isSelected ? 2.f : 1.f;
            draw_list->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), borderColor, 0.f, 0, borderThickness);

            // 이름 (가운데 정렬)
            string name = Utils::ToString(state->GetDisplayName());

            float textWidth = ImGui::CalcTextSize(name.c_str()).x;
            float centerX = x1 + (x2 - x1 - textWidth) / 2.f;
            draw_list->AddText(ImVec2(centerX, y1 + 2), SeqColors::Text, name.c_str());

            // ============ 클릭 ===================
            ImRect centerRect(ImVec2(x1 + handleWidth, y1), ImVec2(x2 - handleWidth, y2));

            if (ImGui::IsMouseClicked(0))
            {
                if (leftHandle.Contains(io.MousePos))
                {
                    draggingStateIndex = i;
                    stateDragMode = 2;  // 왼쪽 핸들
                    selectedNotifyStateIndex = i;
                    clickedOnNotify = true;
                }
                else if (rightHandle.Contains(io.MousePos))
                {
                    draggingStateIndex = i;
                    stateDragMode = 3;  // 오른쪽 핸들
                    selectedNotifyStateIndex = i;
                    clickedOnNotify = true;
                }
                else if (centerRect.Contains(io.MousePos))
                {
                    draggingStateIndex = i;
                    stateDragMode = 1;  // 전체 이동
                    stateDragOffset = io.MousePos.x - x1;
                    selectedNotifyStateIndex = i;
                    clickedOnNotify = true;
                }
            }

            // ==== 드래그 중 ====
            if (draggingStateIndex == i && ImGui::IsMouseDown(0))
            {
                float relativeX = io.MousePos.x - rc.Min.x;
                int newFrame = frameMin + (int)(relativeX / pixelPerFrame);
                newFrame = max(frameMin, min(frameMax, newFrame));

                if (stateDragMode == 1)  // 전체 이동
                {
                    int duration = state->GetEndFrame() - state->GetStartFrame();
                    int startFrame = frameMin + (int)((io.MousePos.x - stateDragOffset - rc.Min.x) / pixelPerFrame);
                    startFrame = max(frameMin, min(frameMax - duration, startFrame));
                    state->SetStartFrame(startFrame);
                    state->SetEndFrame(startFrame + duration);
                }
                else if (stateDragMode == 2)  // 왼쪽 핸들 (시작)
                {
                    newFrame = min(newFrame, state->GetEndFrame() - 1);
                    state->SetStartFrame(newFrame);
                }
                else if (stateDragMode == 3)  // 오른쪽 핸들 (끝)
                {
                    newFrame = max(newFrame, state->GetStartFrame() + 1);
                    state->SetEndFrame(newFrame);
                }
            }
        }

        if (!ImGui::IsMouseDown(0))
        {
            draggingStateIndex = -1;
            stateDragMode = 0;
        }

        return;
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
    _shader = GET_SINGLE(RenderManager)->GetShader();

    // 프리뷰용 RenderTarget 생성
    _previewRenderTarget = make_shared<RenderTarget>();
    _previewRenderTarget->Create(100, 400);

    // 프리뷰 카메라 설정
    _previewCamera = make_shared<GameObject>();
    _previewCamera->GetOrAddTransform()->SetPosition(Vec3(0, 1, -5));
    _previewCamera->GetOrAddTransform()->LookAt(Vec3(0.f, 1.0f, 0.f));
    _previewCamera->AddComponent(make_shared<Camera>());

    _previewCharacter = make_shared<GameObject>();
    _previewCharacter->GetOrAddTransform();
    _previewCharacter->AddComponent(make_shared<ModelAnimator>(_shader));

    _previewAnimator = _previewCharacter->GetModelAnimator();

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

    if (_isPlaying && _previewModel)
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
    bool wasActive = _isActive;

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
    }
    ImGui::EndGroup();
    ImGui::End();

    if (wasActive && !_isActive)
        CloseDetailView();
}

void AnimationView::SetAnimation(shared_ptr<Model> model, int animIndex, vector<wstring>& animPaths)
{
    if (_previewModel == model && _animIndex == animIndex)
        return;

    if (_previewModel && !_animNames.empty() && _animIndex < _animNames.size())
    {
        wstring oldAnimName = _animNames[_animIndex];
        wstring oldFilePath = L"../Resources/Notifies/" + oldAnimName + L".json";
        // 기존에 작업하던 내용을 파일에 덮어쓰기
        GET_SINGLE(AnimNotifyManager)->SaveToJson(oldAnimName, oldFilePath);
    }

    _previewModel = model;  // 모델 저장
    _animIndex = animIndex;
    _currentFrame = 0;
    _playbackTime = 0.f;
    _animPaths = animPaths;
    _isPlaying = true;

    _selectedNotifyIndex = -1;
    _selectedNotifyStateIndex = -1;

    if (_previewAnimator)
    {
        _previewAnimator->SetModel(_previewModel);
        _previewAnimator->SetPass(2);
    }

    // 애니메이션 정보 가져오기
    _animNames.clear();

    for (const auto& path : _animPaths)
    {
        _animNames.push_back(FileUtils::PathToAnimName(path));
    }
    if (_previewModel && _previewModel->GetAnimations().size() > animIndex)
    {
        auto& anim = _previewModel->GetAnimations()[animIndex];
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

    NotifyDetailView();
}

void AnimationView::DrawPreview()
{
    if (!_previewRenderTarget || !_previewAnimator || !_previewModel)
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

    auto owner = _previewCharacter;
    if (owner)
    {
        auto originalPos = owner->GetTransform()->GetPosition();

        owner->GetTransform()->SetScale(Vec3(0.01f));
        owner->GetTransform()->SetPosition(Vec3(0, 0, 0));
        owner->GetTransform()->SetRotation(Vec3(0, 0, 0));
        owner->GetTransform()->UpdateTransform();

        TweenDesc tweenDesc = {};
        tweenDesc.curr.animIndex = _animIndex;
        tweenDesc.curr.currFrame = min(_currentFrame, max(0, _frameMax - 1));
        tweenDesc.curr.nextFrame = tweenDesc.curr.currFrame;
        tweenDesc.curr.ratio = 0.f;
        tweenDesc.curr.sumTime = 0.f;
        tweenDesc.curr.speed = 1.f;
        tweenDesc.next.animIndex = -1;

        _previewAnimator->SetTweenDesc(tweenDesc);
        _previewAnimator->UpdateTweenData();

        // 단일 오브젝트 렌더링
        vector<shared_ptr<GameObject>> vec;
        vec.push_back(owner);
        INSTANCING->Render(vec);

        //owner->GetTransform()->SetPosition(originalPos);
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
    if (!_previewModel)
        return;

    ImGui::Separator();

    // 시퀀서 프레임 범위 동기화
    _sequence.frameMin = _frameMin;
    _sequence.frameMax = _frameMax;

    wstring animName = (_animNames.size() > _animIndex) ? _animNames[_animIndex] : L"";
    _sequence.notifyContainer = GET_SINGLE(AnimNotifyManager)->GetContainer(animName);
    _sequence.selectedNotifyIndex = _selectedNotifyIndex;
    _sequence.selectedNotifyStateIndex = _selectedNotifyStateIndex;
    _firstFrame = 0;

    _sequence.clickedOnNotify = false;

    // ImSequencer 그리기
    ImSequencer::Sequencer(
        &_sequence,
        &_currentFrame,
        &_sequencerExpanded,
        &_selectedEntry,
        &_firstFrame,
        ImSequencer::SEQUENCER_EDIT_STARTEND | ImSequencer::SEQUENCER_CHANGE_FRAME
    );

    if (_selectedNotifyIndex != _sequence.selectedNotifyIndex)
    {
        _selectedNotifyIndex = _sequence.selectedNotifyIndex;
        NotifyDetailView();
    }

    if (_selectedNotifyStateIndex != _sequence.selectedNotifyStateIndex)
    {
        _selectedNotifyStateIndex = _sequence.selectedNotifyStateIndex;
        NotifyDetailView();
    }

    // 허공 클릭
    if (ImGui::IsMouseClicked(0) && !_sequence.clickedOnNotify)
    {
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
        {
            _selectedNotifyIndex = -1;
            _selectedNotifyStateIndex = -1;
            NotifyDetailView();
        }
    }
}

void AnimationView::DrawAnimationList()
{
    ImGui::BeginChild("AnimList", ImVec2(200, 0), true);

    ImGui::Text("Animations");
    ImGui::Separator();
    ImGui::BeginChild("AnimScroll", ImVec2(0, 325), false);
    {
        auto& entries = GET_SINGLE(ModelRegistry)->GetAll();

        for (auto& [modelName, entry] : entries)
        {
            string modelNameStr = Utils::ToString(modelName);

            ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);

            if (ImGui::TreeNode(modelNameStr.c_str()))
            {
                auto& animations = entry.model->GetAnimations();

                for (size_t i = 0; i < animations.size(); i++)
                {
                    wstring animNameW = FileUtils::PathToAnimName(entry.animPaths[i]);
                    string animName = Utils::ToString(animNameW);

                    bool isSelected = (_previewModel == entry.model) && (_animIndex == (int)i);
                    if (ImGui::Selectable(animName.c_str(), isSelected))
                    {
                        // 클릭 시 해당 애니메이션으로 즉시 전환 (SetAnimation 재활용)
                        SetAnimation(entry.model, (int)i, entry.animPaths);
                    }
                }
                ImGui::TreePop();
            }
        }
    }
    ImGui::EndChild();

    // Notify List
    ImGui::Separator();

    wstring animName = (_animNames.size() > _animIndex) ? _animNames[_animIndex] : L"";
    auto* container = GET_SINGLE(AnimNotifyManager)->GetContainer(animName);

    // Notify List
    ImGui::Text("[ Notify List ]");
    ImGui::BeginChild("NotifyScroll", ImVec2(0, 150), true);
    {
        if (container)
        {
            for (int i = 0; i < (int)container->notifies.size(); i++)
            {
                auto& notify = container->notifies[i];
                string label = Utils::ToString(notify->GetDisplayName())
                    + " (" + to_string(notify->GetFrame()) + ")";

                bool isSelected = (_selectedNotifyIndex == i);
                if (ImGui::Selectable(label.c_str(), isSelected))
                {
                    _selectedNotifyIndex = i;
                    _selectedNotifyStateIndex = -1;  // State 선택 해제
                    NotifyDetailView();
                }
            }
        }
        else
        {
            ImGui::TextDisabled("No notifies");
        }
    }
    ImGui::EndChild();

    // Notify State List
    ImGui::Text("[ NotifyState List ]");
    ImGui::BeginChild("StateScroll", ImVec2(0, 150), true);
    {
        if (container)
        {
            for (int i = 0; i < (int)container->notifyStates.size(); i++)
            {
                auto& state = container->notifyStates[i];
                string label = Utils::ToString(state->GetDisplayName())
                    + " (" + to_string(state->GetStartFrame())
                    + "~" + to_string(state->GetEndFrame()) + ")";

                bool isSelected = (_selectedNotifyStateIndex == i);
                if (ImGui::Selectable(label.c_str(), isSelected))
                {
                    _selectedNotifyStateIndex = i;
                    _selectedNotifyIndex = -1;  // Notify 선택 해제
                    NotifyDetailView();
                }
            }
        }
        else
        {
            ImGui::TextDisabled("No states");
        }

    }
    ImGui::EndChild();

    ImGui::EndChild();
}

void AnimationView::NotifyDetailView()
{
    auto detailView = dynamic_pointer_cast<AnimationDetailView>(
        GET_SINGLE(EditorManager)->GetWindow(L"Animation Details"));

    if (!detailView)
        return;

    // 현재 애니메이션 이름
    wstring animName = L"";
    if (_animNames.size() > _animIndex)
        animName = _animNames[_animIndex];

    // Context 전달
    detailView->SetContext(animName, _selectedNotifyIndex, _selectedNotifyStateIndex);
}

void AnimationView::CloseDetailView()
{
    auto detailView = GET_SINGLE(EditorManager)->GetWindow(L"Animation Details");

    if (detailView)
        detailView->SetActive(false);
}

void AnimationView::ClearSelection()
{
    _selectedNotifyIndex = -1;
    _selectedNotifyStateIndex = 1;
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
