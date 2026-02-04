#pragma once

class GameObject;

class SceneSerializer
{
private:
    SceneSerializer() = default;
    ~SceneSerializer() = default;

public:
    static void SaveScene(const wstring& filePath,
        const vector<shared_ptr<GameObject>>& gameObjects);

    static vector<shared_ptr<GameObject>> LoadScene(const wstring& filePath);

private:
    static json GameObjectToJson(shared_ptr<GameObject> obj);
    static shared_ptr<GameObject> JsonToGameObject(const json& j);

};

