#pragma once

#include "AnimNotifyData.h"

class AnimNotifyManager
{
    DECLARE_SINGLE(AnimNotifyManager);

public:
    // ─────────────────────────────────────────────
    // 노티파이 컨테이너 관리
    // ─────────────────────────────────────────────
    AnimNotifyContainer& GetOrCreateContainer(const wstring& animName);
    AnimNotifyContainer* GetContainer(const wstring& animName);

    // ─────────────────────────────────────────────
    // 노티파이 추가/삭제
    // ─────────────────────────────────────────────
    void AddNotify(const wstring& animName, const shared_ptr<AnimNotify>& notify);
    void AddNotifyState(const wstring& animName, const shared_ptr<AnimNotifyState>& notifyState);

    void RemoveNotify(const wstring& animName, int index);
    void RemoveNotifyState(const wstring& animName, int index);

    // ─────────────────────────────────────────────
    // JSON 저장/불러오기
    // ─────────────────────────────────────────────
    void SaveToJson(const wstring& animName, const wstring& filePath);
    void LoadFromJson(const wstring& filePath);

    void LoadAllFromJson(const wstring& folderPath);

private:
    unordered_map<wstring, AnimNotifyContainer> _containers;

};

