#include "pch.h"
#include "SceneSerializer.h"
#include "GameObject.h"
#include "Transform.h"
#include "ModelRenderer.h"
#include <fstream>
#include <filesystem>

void SceneSerializer::SaveScene(const wstring& filePath, const vector<shared_ptr<GameObject>>& gameObjects)
{
    // 폴더 없으면 생성
    filesystem::path path(filePath);
    if (!filesystem::exists(path.parent_path()))
    {
        filesystem::create_directory(path.parent_path());
    }

    json sceneJson;
    sceneJson["sceneName"] = "EditorScene";

    json objectsArray = json::array();
    for (auto& obj : gameObjects)
    {
        objectsArray.push_back(GameObjectToJson(obj));
    }

    sceneJson["gameObjects"] = objectsArray;

    // 파일 저장
    ofstream file(filePath);
    file << sceneJson.dump(4); // 4칸 들여쓰기
    file.close();
}

vector<shared_ptr<GameObject>> SceneSerializer::LoadScene(const wstring& filePath)
{
    vector<shared_ptr<GameObject>> objects;

    ifstream file(filePath);
    if (!file.is_open())
        return objects;

    json sceneJson;
    file >> sceneJson;
    file.close();

    for (auto& objJson : sceneJson["gameObjects"])
    {
        auto obj = JsonToGameObject(objJson);
        if (obj)
            objects.push_back(obj);
    }

    return objects;
}

json SceneSerializer::GameObjectToJson(shared_ptr<GameObject> obj)
{
    json j;
    j["name"] = "GameObject"; // 나중에 이름 시스템 추가 가능

    json components = json::array();

    if (auto transform = obj->GetTransform())
        components.push_back(transform->ToJson());

    if (auto modelRenderer = obj->GetModelRenderer())
        components.push_back(modelRenderer->ToJson());

    j["components"] = components;
    return j;
}

shared_ptr<GameObject> SceneSerializer::JsonToGameObject(const json& j)
{
    auto obj = make_shared<GameObject>();

    for (auto& compJson : j["components"])
    {
        string type = compJson["type"];

        if (type == "Transform")
        {
            auto transform = obj->GetOrAddTransform();
            transform->FromJson(compJson);
        }
        else if (type == "ModelRenderer")
        {
            auto renderer = make_shared<ModelRenderer>(nullptr);
            renderer->FromJson(compJson);
            obj->AddComponent(renderer);
        }
    }

    return obj;
}
