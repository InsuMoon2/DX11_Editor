#include "pch.h"
#include "InstancingDemo.h"
#include "Camera.h"
#include "GameObject.h"
#include "CameraScript.h"
#include "Material.h"
#include "MeshRenderer.h"

void InstancingDemo::Init()
{
    RESOURCES->Init();
    _shader = make_shared<Shader>(L"19. InstancingDemo.fx");

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

        // Instancing
        _material = material;
    }

    for (int32 i = 0; i < 10000; i++)
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

            // Instancing
            _mesh = mesh;
        }

        _objs.push_back(obj);
    }

    RENDER->Init(_shader);

    // Instancing
    _instanceBuffer = make_shared<VertexBuffer>();

    for (auto& obj : _objs)
    {
        Matrix world = obj->GetTransform()->GetWorldMatrix();
        _worlds.push_back(world);
    }

    _instanceBuffer->Create(_worlds, 1);
}

void InstancingDemo::Update()
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

    _material->Update();

    /* 자기 자신의 World를 넣어주는게 아니라 인스턴스 버퍼를 이용해서 넣어줘야 하기 때문에
     * _instanceBuffer를 사용해야한다.
     */
    //auto world = GetTransform()->GetWorldMatrix();
    //RENDER->PushTransformData(TransformDesc{ world });

    _mesh->GetVertexBuffer()->PushData();
    _instanceBuffer->PushData();
    _mesh->GetIndexBuffer()->PushData();

    //shader->DrawIndexed(0, 0, _mesh->GetIndexBuffer()->GetCount(), 0, 0);
    _shader->DrawIndexedInstanced(0, 0, _mesh->GetIndexBuffer()->GetCount(), _objs.size());

}

void InstancingDemo::Render()
{
    
}
