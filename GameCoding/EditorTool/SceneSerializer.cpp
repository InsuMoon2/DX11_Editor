#include "pch.h"
#include "SceneSerializer.h"
#include "GameObject.h"
#include "Transform.h"
#include "ModelRenderer.h"
#include <fstream>
#include <filesystem>
#include "Utils.h"

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
    j["name"] = Utils::ToString(obj->GetName());

    json components = json::array();

    // Fixed Component 순회
    for (auto& component : obj->GetComponents())
    {
        if (component)
        {
            json compJson = component->ToJson();
            if (!compJson.empty())
                components.push_back(compJson);
        }
    }

    // 추후 Script도 해줄지 말지?

    j["components"] = components;

    return j;
}

shared_ptr<GameObject> SceneSerializer::JsonToGameObject(const json& j)
{
    // 이름 파싱 및 객체 생성
    string nameStr = j["name"];
    shared_ptr<GameObject> obj = nullptr;

    // 이름 그대로 생성 ex) "Player", "MainCamera", "Light"
    obj = GameObjectFactory::CreateObject(nameStr);

    // 실패 시, 뒤에 숫자 "_1" 제외하고 생성 시도
    if (obj == nullptr)
    {
        size_t underscorePos = nameStr.find_last_of("_");
        if (underscorePos != string::npos)
        {
            string typeName = nameStr.substr(0, underscorePos);
            obj = GameObjectFactory::CreateObject(typeName);
        }
    }

    // 그래도 실패하면, 기본 GameObject 로 생성? 을해야하나
    if (obj == nullptr)
        obj = make_shared<GameObject>();

    // 이름 다시 복구
    obj->SetName(Utils::ToWString(nameStr));

    // 컴포넌트 로드
    for (auto& compJson : j["components"])
    {
        ComponentFactory::LoadComponent(obj, compJson);
    }

    return obj;
}
