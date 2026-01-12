#include "pch.h"
#include "AnimNotifyManager.h"
#include <fstream>
#include <filesystem>
#include "Utils.h"

namespace fs = std::filesystem;

AnimNotifyContainer& AnimNotifyManager::GetOrCreateContainer(const wstring& animName)
{
    if (_containers.find(animName) == _containers.end())
    {
        AnimNotifyContainer container;
        container.animationName = animName;
        _containers[animName] = container;
    }

    return _containers[animName];
}

AnimNotifyContainer* AnimNotifyManager::GetContainer(const wstring& animName)
{
    auto it = _containers.find(animName);
    if (it != _containers.end())
    {
        return &it->second;
    }
    
    return nullptr;
}

void AnimNotifyManager::AddNotify(const wstring& animName, const AnimNotifyData& notify)
{
    auto& container = GetOrCreateContainer(animName);
    container.notifies.push_back(notify);
}

void AnimNotifyManager::AddNotifyState(const wstring& animName, const AnimNotifyStateData& notifyState)
{
    auto& container = GetOrCreateContainer(animName);
    container.notifyStates.push_back(notifyState);
}

void AnimNotifyManager::RemoveNotify(const wstring& animName, int index)
{
    auto* container = GetContainer(animName);
    if (container && index >= 0 && index < (int)container->notifies.size())
    {
        container->notifies.erase(container->notifies.begin() + index);
    }
}

void AnimNotifyManager::RemoveNotifyState(const wstring& animName, int index)
{
    auto* container = GetContainer(animName);
    if (container && index >= 0 && index < (int)container->notifyStates.size())
    {
        container->notifyStates.erase(container->notifyStates.begin() + index);
    }
}

void AnimNotifyManager::SaveToJson(const wstring& animName, const wstring& filePath)
{
    auto* container = GetContainer(animName);
    if (!container)
        return;

    json j;
    j["animationName"] = Utils::ToString(container->animationName);

    // Notifies
    j["notifies"] = json::array();
    for (auto& notify : container->notifies)
    {
        json notifyJson;
        notifyJson["name"] = Utils::ToString(notify.name);
        notifyJson["frame"] = notify.frame;
        j["notifies"].push_back(notifyJson);
    }

    // NotifyStates
    j["notifyStates"] = json::array();
    for (auto& state : container->notifyStates)
    {
        json stateJson;
        stateJson["name"] = Utils::ToString(state.name);
        stateJson["startFrame"] = state.startFrame;
        stateJson["endFrame"] = state.endFrame;
        j["notifyStates"].push_back(stateJson);
    }

    fs::create_directories(fs::path(filePath).parent_path());

    // 파일 저장
    ofstream file(filePath);
    if (file.is_open())
    {
        file << j.dump(4);
        file.close();
    }
}

void AnimNotifyManager::LoadFromJson(const wstring& filePath)
{
    ifstream file(filePath);
    if (!file.is_open())
        return;

    json j;
    file >> j;
    file.close();

    AnimNotifyContainer container;
    container.animationName = Utils::ToWString(j["animationName"].get<string>());

    // Notifies
    if (j.contains("notifies"))
    {
        for (auto& notifyJson : j["notifies"])
        {
            AnimNotifyData notify;
            notify.name = Utils::ToWString(notifyJson["name"].get<string>());
            notify.frame = notifyJson["frame"].get<int>();
            container.notifies.push_back(notify);
        }
    }

    // NotifyStates
    if (j.contains("notifyStates"))
    {
        for (auto& stateJson : j["notifyStates"])
        {
            AnimNotifyStateData state;
            state.name = Utils::ToWString(stateJson["name"].get<string>());
            state.startFrame = stateJson["startFrame"].get<int>();
            state.endFrame = stateJson["endFrame"].get<int>();
            container.notifyStates.push_back(state);
        }
    }
    _containers[container.animationName] = container;
}
