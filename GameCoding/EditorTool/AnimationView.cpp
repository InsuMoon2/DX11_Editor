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
#include "AnimNotifyState.h"

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
        for (int i = 0; i < notifyContainer->notifies.size(); i++)
        {
            auto& notify = notifyContainer->notifies[i];

            float x = rc.Min.x + (notify->GetFrame() - frameMin) * pixelPerFrame;
            float markerWidth = 40.f;
            float markerHeight = rc.Max.y - rc.Min.y - 4.f;

            // 배경 사각형
            draw_list->AddRectFilled(
                ImVec2(x, rc.Min.y + 2),
                ImVec2(x + markerWidth, rc.Min.y + 2 + markerHeight),
                0xFF33CC33); // 연한 초록?
            unsigned int borderColor = (i == selectedNotifyIndex) ? 0xFF000000 : 0xFFFFFFFF;

            // 테두리
            draw_list->AddRect(
                ImVec2(x, rc.Min.y + 2),
                ImVec2(x + markerWidth, rc.Min.y + 2 + markerHeight),
                borderColor);
            string name = Utils::ToString(notify->GetDisplayName());
            ImVec2 textPos(x + 2.f, rc.Min.y + 2);
            draw_list->AddText(textPos, 0xFFFFFFFF, name.c_str());
        }
        return;
    }
    // ─────────────────────────────────────────────
    // index == 2: States 트랙
    // ─────────────────────────────────────────────
    if (index == 2)
    {
        for (int i = 0; i < notifyContainer->notifyStates.size(); i++)
        {
            auto& state = notifyContainer->notifyStates[i];
            float x1 = rc.Min.x + (state->GetStartFrame() - frameMin) * pixelPerFrame;
            float x2 = rc.Min.x + (state->GetEndFrame() - frameMin) * pixelPerFrame;

            // 구간 배경 (트랙 전체 높이 사용)
            draw_list->AddRectFilled(
                ImVec2(x1, rc.Min.y + 2),
                ImVec2(x2, rc.Max.y - 2),
                0xFFFFAA44);  // 하늘색

            unsigned int borderColor = (i == selectedNotifyIndex) ? 0xFF000000 : 0xFFFFFFFF;

            // 테두리
            draw_list->AddRect(
                ImVec2(x1, rc.Min.y + 2),
                ImVec2(x2, rc.Max.y - 2),
                borderColor);
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
    _previewModel = model;  // 모델 저장
    _animIndex = animIndex;
    _currentFrame = 0;
    _playbackTime = 0.f;
    _animPaths = animPaths;
    _isPlaying = true;

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

    ImGui::EndChild();
}

void AnimationView::DrawNotifyPanel()
{
    if (!_previewModel)
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
    ImGui::Text("Notify List :");
    if (container)
    {
        for (int i = 0; i < (int)container->notifies.size(); i++)
        {
            auto& notify = container->notifies[i];

            string label = Utils::ToString(notify->GetDisplayName()) + " (Frame : " + to_string(notify->GetFrame()) + ")";

            bool isSelected = (_selectedNotifyIndex == i);
            if (ImGui::Selectable(label.c_str(), isSelected))
            {
                _selectedNotifyIndex = i;
                _currentFrame = notify->GetFrame(); // 해당 프레임으로 이동
            }
        }
    }

    // 선택된 Notify 편집
    if (container && _selectedNotifyIndex >= 0 && _selectedNotifyIndex < (int)container->notifies.size())
    {
        ImGui::Dummy(ImVec2(0, 10)); // 여백
        ImGui::BeginGroup();
        ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "[ Inspector ]"); // 노란색 제목?

        auto& notify = container->notifies[_selectedNotifyIndex];

        // 각 노티파이가 자신의 Gui 렌더
        notify->OnGui();

        ImGui::Dummy(ImVec2(0, 5));

        // 삭제 버튼
        if (ImGui::Button("Remove Selected Notify"))
        {
            GET_SINGLE(AnimNotifyManager)->RemoveNotify(animName, _selectedNotifyIndex);
            _selectedNotifyIndex = -1;
        }

        ImGui::EndGroup();
    }

    ImGui::Separator();

    // Notify 추가 Factory + Combo Box
    auto& creators = AnimNotifyFactory::GetCreators();
    vector<string> typeNames;

    for (auto& pair : creators)
        typeNames.push_back(pair.first);

    if (!typeNames.empty())
    {
        static int selectedTypeIndex = 0;

        // 인덱스 안전장치
        if (selectedTypeIndex >= typeNames.size())
            selectedTypeIndex = 0;

        // COmbo Box
        if (ImGui::BeginCombo("Notify Type", typeNames[selectedTypeIndex].c_str()))
        {
            for (int i = 0; i < typeNames.size(); i++)
            {
                bool isSelected = (selectedTypeIndex == i);
                if (ImGui::Selectable(typeNames[i].c_str(), isSelected))
                    selectedTypeIndex = i;

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::InputInt("Frame##New", &_newNotifyFrame);

        if (ImGui::Button("Add Notify"))
        {
            // 선택된 타입 이름으로 객체 생성
            string typeName = typeNames[selectedTypeIndex];
            auto notify = AnimNotifyFactory::CreateNotify(typeName);

            if (notify)
            {
                notify->SetFrame(_newNotifyFrame);
                GET_SINGLE(AnimNotifyManager)->AddNotify(animName, notify);
            }
        }
    }

    // ─────────────────────────────────────────────
    // 저장
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
