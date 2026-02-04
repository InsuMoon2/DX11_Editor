#include "pch.h"
#include "ModelRenderer.h"
#include "Material.h"
#include "ModelMesh.h"
#include "Model.h"
#include "Utils.h"

REGISTER_COMPONENT(ModelRenderer, ComponentType::ModelRenderer);

ModelRenderer::ModelRenderer()
    : Super(ComponentType::ModelRenderer)
{

}

ModelRenderer::ModelRenderer(shared_ptr<Shader> shader)
    : Super(ComponentType::ModelRenderer), _shader(shader)
{

}

ModelRenderer::~ModelRenderer()
{

}

//void ModelRenderer::Update()
//{
//    if (_model == nullptr)
//        return;
//
//    // Bones
//    //BoneDesc boneDesc;
//    //
//    //const uint32 boneCount = _model->GetBoneCount();
//    //for (uint32 i = 0; i < boneCount; i++)
//    //{
//    //    shared_ptr<ModelBone> bone = _model->GetBoneByIndex(i);
//    //    boneDesc.transforms[i] = bone->transform;
//    //}
//    //RENDER->PushBoneData(boneDesc);
//
//    // Transform
//    auto world = GetTransform()->GetWorldMatrix();
//    RENDER->PushTransformData(TransformDesc{ world });
//
//    const auto& meshes = _model->GetMeshes();
//    for (auto& mesh : meshes)
//    {
//        if (mesh->material)
//            mesh->material->Update();
//
//        // BoneIndex
//        _shader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);
//
//        uint32 stride = mesh->vertexBuffer->GetStride();
//        uint32 offset = mesh->vertexBuffer->GetOffset();
//
//        DC->IASetVertexBuffers(0, 1, mesh->vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
//        DC->IASetIndexBuffer(mesh->indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);
//
//        _shader->DrawIndexed(0, _pass, mesh->indexBuffer->GetCount(), 0, 0);
//    }
//
//}

//void ModelRenderer::Update()
//{
//    if (_model == nullptr)
//        return;
//
//    // Bones
//    BoneDesc boneDesc;
//    
//    const uint32 boneCount = _model->GetBoneCount();
//    for (uint32 i = 0; i < boneCount; i++)
//    {
//        shared_ptr<ModelBone> bone = _model->GetBoneByIndex(i);
//        boneDesc.transforms[i] = bone->transform;
//    }
//    RENDER->PushBoneData(boneDesc);
//
//    // Transform
//    auto world = GetTransform()->GetWorldMatrix();
//    RENDER->PushTransformData(TransformDesc{ world });
//
//    const auto& meshes = _model->GetMeshes();
//    for (auto& mesh : meshes)
//    {
//        if (mesh->material)
//            mesh->material->Update();
//
//        // BoneIndex
//        _shader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);
//
//        uint32 stride = mesh->vertexBuffer->GetStride();
//        uint32 offset = mesh->vertexBuffer->GetOffset();
//
//        ENGINE_DC->IASetVertexBuffers(0, 1, mesh->vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
//        ENGINE_DC->IASetIndexBuffer(mesh->indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);
//
//        _shader->DrawIndexed(0, _pass, mesh->indexBuffer->GetCount(), 0, 0);
//    }
//
//}

void ModelRenderer::SetModel(shared_ptr<Model> model)
{
    _model = model;

    const auto& materials = model->GetMaterials();
    for (auto& material : materials)
    {
        material->SetShader(_shader);
    }
}

void ModelRenderer::RenderInstancing(shared_ptr<InstancingBuffer>& buffer)
{
    if (_model == nullptr)
        return;

    // Bones 모두 동일한 구조를 사용하겠다고 가정.
    BoneDesc boneDesc;

    const uint32 boneCount = _model->GetBoneCount();
    for (uint32 i = 0; i < boneCount; i++)
    {
        shared_ptr<ModelBone> bone = _model->GetBoneByIndex(i);
        boneDesc.transforms[i] = bone->transform;
    }
    RENDER->PushBoneData(boneDesc);

    const auto& meshes = _model->GetMeshes();
    for (auto& mesh : meshes)
    {
        if (mesh->material)
            mesh->material->Update();

        // BoneIndex
        _shader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);

        uint32 stride = mesh->vertexBuffer->GetStride();
        uint32 offset = mesh->vertexBuffer->GetOffset();

        // IA
        mesh->vertexBuffer->PushData();
        mesh->indexBuffer->PushData();

        buffer->PushData();

        _shader->DrawIndexedInstanced(0, _pass, mesh->indexBuffer->GetCount(), buffer->GetCount());
    }
}

InstanceID ModelRenderer::GetInstanceID()
{
    /* 같은 모델, 같은 셰이더를 쓰는지만 판단하면 된다. */
    return make_pair((uint64)_model.get(), (uint64)_shader.get());
}

shared_ptr<Component> ModelRenderer::Clone() const
{
    auto clone = make_shared<ModelRenderer>(_shader);

    clone->SetModel(_model);
    clone->SetPass(_pass);

    return clone;
}

json ModelRenderer::ToJson() const
{
    json j;
    j["type"] = "ModelRenderer";
    j["pass"] = _pass;

    if (_model)
        j["modelFile"] = Utils::ToString(_model->GetFileName());

    // 셰이더 경로도 저장
    if (_shader)
        j["shaderFile"] = "23. RenderDemo.fx";

    return j;
}

void ModelRenderer::FromJson(const json& j)
{
    // 셰이더 먼저 로드
    wstring shaderFile = L"23. RenderDemo.fx";

    if (j.contains("shaderFile"))
        shaderFile = Utils::ToWString(j["shaderFile"].get<string>());

    _shader = make_shared<Shader>(shaderFile);

    if (j.contains("pass"))
        _pass = j["pass"];

    if (j.contains("modelFile"))
    {
        wstring fileName = Utils::ToWString(j["modelFile"].get<string>());

        auto model = make_shared<Model>();
        model->ReadModel(fileName);
        model->ReadMaterial(fileName);
        SetModel(model);
    }
}
