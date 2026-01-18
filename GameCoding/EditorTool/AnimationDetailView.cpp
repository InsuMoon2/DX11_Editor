#include "pch.h"
#include "AnimationDetailView.h"
#include "EditorManager.h"
#include "AnimationView.h"
#include "AnimNotifyManager.h"
#include "AnimNotifyFactory.h"
#include "Utils.h"
#include "ModelAnimation.h"

AnimationDetailView::AnimationDetailView()
    : EditorWindow("Animation Details")
{
    _isActive = false;
}

AnimationDetailView::~AnimationDetailView()
{ }

void AnimationDetailView::Init()
{ }

void AnimationDetailView::Update()
{ }

void AnimationDetailView::OnGui()
{
    ImGui::Begin("Animation Details", &_isActive);

    if (_currentAnimName.empty())
    {
        ImGui::TextDisabled("No animation selected");
        ImGui::End();

        return;
    }

    ImGui::Text("Animation : %s", Utils::ToString(_currentAnimName).c_str());
    ImGui::Separator();

    DrawAnimationProperties();
    ImGui::Separator();

    DrawAddNotifySection();
    ImGui::Separator();

    DrawAddNotifyStateSection();
    ImGui::Separator();

    // 저장 버튼
    if (ImGui::Button("Save Notifies"))
    {
        wstring filePath = L"../Resources/Notifies/" + _currentAnimName + L".json";
        GET_SINGLE(AnimNotifyManager)->SaveToJson(_currentAnimName, filePath);
    }

    ImGui::Separator();
    DrawNotifyInspector();

    ImGui::End();
}

void AnimationDetailView::SetContext(const wstring& animName, int selectedNotifyIdx, int selectedStateIdx)
{
    _currentAnimName = animName;
    _selectedNotifyIndex = selectedNotifyIdx;
    _selectedNotifyStateIndex = selectedStateIdx;
}

void AnimationDetailView::DrawNotifyInspector()
{
    auto* container = GET_SINGLE(AnimNotifyManager)->GetContainer(_currentAnimName);
    if (!container) return;

    ImGui::Dummy(ImVec2(0, 15));

    //ImGui::Text("Selected Notify");

    if (_selectedNotifyIndex >= 0 && _selectedNotifyIndex < (int)container->notifies.size())
    {
        auto& notify = container->notifies[_selectedNotifyIndex];

        ImGui::Text("[ Details ]");
        ImGui::Dummy(ImVec2(0, 5));
        ImGui::Text(U8("선택된 노티파이 : ")); ImGui::SameLine();

        ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "[ %s ]",
            Utils::ToString(notify->GetDisplayName()).c_str());

        notify->OnGui();

        if (ImGui::Button("Remove Notify"))
        {
            GET_SINGLE(AnimNotifyManager)->RemoveNotify(_currentAnimName, _selectedNotifyIndex);
            _selectedNotifyIndex = -1;

            auto animView = dynamic_pointer_cast<AnimationView>(
                GET_SINGLE(EditorManager)->GetWindow(L"Animation"));
            if (animView)
                animView->ClearSelection();
        }
    }
    else
    {
        ImGui::TextDisabled(U8("타임라인에서 노티파이 선택"));
    }
}

void AnimationDetailView::DrawAddNotifySection()
{
    // Notify
    ImGui::Text("Add Notify");

    auto& creators = AnimNotifyFactory::GetNotifyCreators();
    vector<string> typeNames;

    for (auto& pair : creators)
        typeNames.push_back(pair.first);

    if (typeNames.empty()) return;

    static int selectedTypeIndex = 0;
    if (selectedTypeIndex >= typeNames.size())
        selectedTypeIndex = 0;

    if (ImGui::BeginCombo("Type##Notify", typeNames[selectedTypeIndex].c_str()))
    {
        for (int i = 0; i < typeNames.size(); i++)
        {
            if (ImGui::Selectable(typeNames[i].c_str(), selectedTypeIndex == i))
                selectedTypeIndex = i;
        }
        ImGui::EndCombo();
    }

    ImGui::InputInt("Frame", &_newNotifyFrame);

    if (ImGui::Button("Add Notify"))
    {
        auto notify = AnimNotifyFactory::CreateNotify(typeNames[selectedTypeIndex]);
        if (notify)
        {
            notify->SetFrame(_newNotifyFrame);
            GET_SINGLE(AnimNotifyManager)->AddNotify(_currentAnimName, notify);
        }
    }

}

void AnimationDetailView::DrawNotifyStateInspector()
{
    // Notify State
    ImGui::Separator();
    ImGui::Text("Notify States");

    auto& stateCreators = AnimNotifyFactory::GetNotifyStateCreators();
    vector<string> stateTypeNames;

    for (auto& pair : stateCreators)
        stateTypeNames.push_back(pair.first);

    if (!stateTypeNames.empty())
    {
        static int selectedStateIndex = 0;

        // 인덱스 안전장치
        if (selectedStateIndex >= stateTypeNames.size())
            selectedStateIndex = 0;

        if (ImGui::BeginCombo("State Type", stateTypeNames[selectedStateIndex].c_str()))
        {
            for (int i = 0; i < stateTypeNames.size(); i++)
            {
                bool isSelected = (selectedStateIndex == i);
                if (ImGui::Selectable(stateTypeNames[i].c_str(), isSelected))
                    selectedStateIndex = i;

                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }
}

void AnimationDetailView::DrawAddNotifyStateSection()
{
    ImGui::Text("Add Notify State");

    auto& stateCreators = AnimNotifyFactory::GetNotifyStateCreators();

    vector<string> stateTypeNames;
    for (auto& pair : stateCreators)
        stateTypeNames.push_back(pair.first);

    if (stateTypeNames.empty())
        return;

    static int selectedStateIndex = 0;

    if (selectedStateIndex >= stateTypeNames.size())
        selectedStateIndex = 0;

    if (ImGui::BeginCombo("State Type", stateTypeNames[selectedStateIndex].c_str()))
    {
        for (int i = 0; i < stateTypeNames.size(); i++)
        {
            bool isSelected = (selectedStateIndex == i);
            if (ImGui::Selectable(stateTypeNames[i].c_str(), isSelected))
                selectedStateIndex = i;
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::InputInt("Start Frame", &_newStateStartFrame);
    ImGui::InputInt("End Frame", &_newStateEndFrame);

    if (ImGui::Button("Add Notify State"))
    {
        string typeName = stateTypeNames[selectedStateIndex];
        auto state = AnimNotifyFactory::CreateNotifyState(typeName);

        if (state)
        {
            state->SetStartFrame(_newStateStartFrame);
            state->SetEndFrame(_newStateEndFrame);

            GET_SINGLE(AnimNotifyManager)->AddNotifyState(_currentAnimName, state);
        }
    }
}

void AnimationDetailView::DrawAnimationProperties()
{
    ImGui::Text("Animation Properties");
    ImGui::Dummy(ImVec2(0, 5));

    auto animView = GET_ANIMATION_VIEW();

    if (!animView)
        return;

    auto model = animView->GetCurrentModel();
    if (!model)
        return;

    auto animation = model->GetAnimationByName(_currentAnimName);
    if (!animation)
        return;

    // ─────────────────────────────────────────────
    // PlayRate 슬라이더
    // ─────────────────────────────────────────────
    float playRate = animation->playRate;
    if (ImGui::SliderFloat("Play Rate", &playRate, 0.1f, 3.0f, "%.2f"))
    {
        animation->playRate = playRate;
    }

    ImGui::Dummy(ImVec2(0, 5));

    // ─────────────────────────────────────────────
    // 애니메이션 정보 (읽기 전용)
    // ─────────────────────────────────────────────
    ImGui::TextDisabled("Duration: %.2f sec", animation->duration);
    ImGui::TextDisabled("Frame Rate: %.1f fps", animation->frameRate);
    ImGui::TextDisabled("Frame Count: %d", animation->frameCount);

    ImGui::Dummy(ImVec2(0, 10));

    if (ImGui::Button("Save Animation Settings"))
    {
        auto model = animView->GetCurrentModel();
        int animIndex = animView->GetCurrentAnimIndex();

        if (model && animIndex >= 0)
        {
            wstring filePath = animView->GetAnimPath(animIndex);
            model->SaveAnimation(animIndex, filePath);
        }
    }
}
