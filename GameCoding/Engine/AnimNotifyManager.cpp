#include "pch.h"
#include "AnimNotifyManager.h"
#include <fstream>
#include <filesystem>
#include "AnimNotifyState.h"
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

void AnimNotifyManager::AddNotify(const wstring& animName, const shared_ptr<AnimNotify>& notify)
{
    auto& container = GetOrCreateContainer(animName);
    container.notifies.push_back(notify);
}

void AnimNotifyManager::AddNotifyState(const wstring& animName, const shared_ptr<AnimNotifyState>& notifyState)
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
        notifyJson["name"] = Utils::ToString(notify->GetDisplayName());
        notify->Serialize(notifyJson);

        j["notifies"].push_back(notifyJson);
    }

    // NotifyStates
    j["notifyStates"] = json::array();
    for (auto& state : container->notifyStates)
    {
        json stateJson;
        stateJson["name"] = Utils::ToString(state->GetDisplayName());
        state->Serialize(stateJson);

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
            string name = notifyJson["name"].get<string>();

            auto notify = AnimNotifyFactory::CreateNotify(name);

            if (notify)
            {
                notify->Deserialize(notifyJson);
                container.notifies.push_back(notify);
            }
        }
    }

    // NotifyStates
    if (j.contains("notifyStates"))
    {
        for (auto& stateJson : j["notifyStates"])
        {
            string name = stateJson["name"].get<string>();

            auto state = AnimNotifyFactory::CreateNotifyState(name);
            if (state)
            {
                state->Deserialize(stateJson);

                container.notifyStates.push_back(state);
            }
        }
    }

    _containers[container.animationName] = container;
}

void AnimNotifyManager::LoadAllFromJson(const wstring& folderPath)
{
    if (!filesystem::exists(folderPath))
        return;

    for (const auto& entry : filesystem::directory_iterator(folderPath))
    {
        if (entry.path().extension() == L".json")
        {
            wstring filePath = entry.path().wstring();
            LoadFromJson(filePath);
        }
    }

    LOG_WARNING("[ Notify Load Clear! ]");
}
