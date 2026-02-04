#include "pch.h"
#include "MeshInstancingDemo.h"
#include "Camera.h"
#include "GameObject.h"
#include "CameraScript.h"
#include "Material.h"
#include "MeshRenderer.h"

void MeshInstancingDemo::Init()
{
    RESOURCES->Init();
    _shader = make_shared<Shader>(L"20. MeshInstancingDemo.fx");

    // Camera
    _camera = make_shared<GameObject>();
    _camera->GetOrAddTransform()->SetPosition(Vec3{ 0.f, 0.f, -5.f });
    _camera->AddComponent(make_shared<Camera>());
    _camera->AddComponent(make_shared<CameraScript>());


    // Material
    {
        shared_ptr<Material> material = make_shared<Material>();
        material->SetShader(_shader);
        auto texture = RESOURCES->Load<Texture>(L"Veigar", L"..\\Resources\\Textures\\veigar.jpg");
        material->SetDiffuseMap(texture);
        MaterialDesc& desc = material->GetMaterialDesc();
        desc.ambient = Vec4(1.f);
        desc.diffuse = Vec4(1.f);
        desc.specular = Vec4(1.f);
        RESOURCES->Add(L"Veigar", material);

    }

    for (int32 i = 0; i < 500; i++)
    {
        auto obj = make_shared<GameObject>();
        obj->GetOrAddTransform()->SetPosition(Vec3(rand() % 100, 0, rand() % 100));
        obj->AddComponent(make_shared<MeshRenderer>());
        {
            obj->GetMeshRenderer()->SetMaterial(RESOURCES->Get<Material>(L"Veigar"));
        }
        {
            auto mesh = RESOURCES->Get<Mesh>(L"Sphere");
            obj->GetMeshRenderer()->SetMesh(mesh);
        }

        _objs.push_back(obj);
    }

    RENDER->Init(_shader);


}

void MeshInstancingDemo::Update()
{
    _camera->Update();
    RENDER->Update();

    {
        LightDesc lightDesc;
        lightDesc.ambient = Vec4(0.4f);
        lightDesc.diffuse = Vec4(1.f);
        lightDesc.specular = Vec4(0.1f);
        lightDesc.direction = Vec3(1.f, 0.f, 1.f);
        RENDER->PushLightData(lightDesc);
    }

    /* 원래는 이렇게 모든 오브젝트를 순회해서 Update하던걸 바꿔줄 필요가 있다. */
    /*for (auto& obj : _objs)
    {
        obj->Update();
    }*/

    INSTANCING->Render(_objs);

}

void MeshInstancingDemo::Render()
{
    
}
