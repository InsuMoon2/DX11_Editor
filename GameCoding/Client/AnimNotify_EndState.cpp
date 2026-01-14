#include "pch.h"
#include "AnimNotify_EndState.h"
#include "ModelAnimator.h"
#include "GameObject.h"
#include "magic_enum/magic_enum.hpp"

REGISTER_ANIM_NOTIFY(AnimNotify_EndState);

wstring AnimNotify_EndState::GetDisplayName() const
{
    return L"End_State";
}

void AnimNotify_EndState::OnNotify(ModelAnimator* animator)
{
    shared_ptr<GameObject> owner = animator->GetGameObject();
    if (owner == nullptr) return;

    auto state = owner->GetScriptComponent<StateComponent>();
    if (state)
    {
        state->SetState(_state);
    }

    LOG_INFO("Notify EndState");
}

void AnimNotify_EndState::OnGui()
{
    AnimNotify::OnGui();

    string currentName = string(magic_enum::enum_name(_state));

    if (ImGui::BeginCombo("State", currentName.c_str()))
    {
        // 모든 Enum값 순회
        for (auto val : magic_enum::enum_values<CharacterState>())
        {
            // 각 Enum의 이름 가져오기
            string stateName = string(magic_enum::enum_name(val));

            bool isSelected = (_state == val);

            if (ImGui::Selectable(stateName.c_str(), isSelected))
            {
                _state = val;
            }

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }

}
