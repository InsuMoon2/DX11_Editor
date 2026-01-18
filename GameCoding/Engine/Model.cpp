#include "pch.h"
#include "Model.h"
#include "tinyxml2.h"
#include <filesystem>
#include "FileUtils.h"
#include "Utils.h"
#include "Material.h"
#include "ModelAnimation.h"
#include "ModelMesh.h"
#include "ResourceManager.h"

Model::Model()
{

}

Model::~Model()
{

}

void Model::ReadMaterial(wstring filename)
{
    wstring fullPath = _texturePath + filename + L".xml";
    auto parentPath = filesystem::path(fullPath).parent_path();

    tinyxml2::XMLDocument* document = new tinyxml2::XMLDocument();
    tinyxml2::XMLError error = document->LoadFile(Utils::ToString(fullPath).c_str());
    assert(error == tinyxml2::XML_SUCCESS);

    tinyxml2::XMLElement* root = document->FirstChildElement();
    tinyxml2::XMLElement* materialNode = root->FirstChildElement();

    while (materialNode)
    {
        shared_ptr<Material> material = make_shared<Material>();

        tinyxml2::XMLElement* node = nullptr;

        node = materialNode->FirstChildElement();
        material->SetName(Utils::ToWString(node->GetText()));

        // Diffuse Texture
        node = node->NextSiblingElement();
        if (node->GetText())
        {
            wstring textureStr = Utils::ToWString(node->GetText());
            if (textureStr.length() > 0)
            {
                auto texture = RESOURCES->GetOrAddTexture(textureStr, (parentPath / textureStr).wstring());
                material->SetDiffuseMap(texture);
            }
        }

        // Specular Texture
        node = node->NextSiblingElement();
        if (node->GetText())
        {
            wstring texture = Utils::ToWString(node->GetText());
            if (texture.length() > 0)
            {
                wstring textureStr = Utils::ToWString(node->GetText());
                if (textureStr.length() > 0)
                {
                    auto texture = RESOURCES->GetOrAddTexture(textureStr, (parentPath / textureStr).wstring());
                    material->SetSpecularMap(texture);
                }
            }
        }

        // Normal Texture
        node = node->NextSiblingElement();
        if (node->GetText())
        {
            wstring textureStr = Utils::ToWString(node->GetText());
            if (textureStr.length() > 0)
            {
                auto texture = RESOURCES->GetOrAddTexture(textureStr, (parentPath / textureStr).wstring());
                material->SetNormalMap(texture);
            }
        }

        // Ambient
        {
            node = node->NextSiblingElement();

            Color color;
            color.x = node->FloatAttribute("R");
            color.y = node->FloatAttribute("G");
            color.z = node->FloatAttribute("B");
            color.w = node->FloatAttribute("A");
            material->GetMaterialDesc().ambient = color;
        }

        // Diffuse
        {
            node = node->NextSiblingElement();

            Color color;
            color.x = node->FloatAttribute("R");
            color.y = node->FloatAttribute("G");
            color.z = node->FloatAttribute("B");
            color.w = node->FloatAttribute("A");
            material->GetMaterialDesc().diffuse = color;
        }

        // Specular
        {
            node = node->NextSiblingElement();

            Color color;
            color.x = node->FloatAttribute("R");
            color.y = node->FloatAttribute("G");
            color.z = node->FloatAttribute("B");
            color.w = node->FloatAttribute("A");
            material->GetMaterialDesc().specular = color;
        }

        // Emissive
        {
            node = node->NextSiblingElement();

            Color color;
            color.x = node->FloatAttribute("R");
            color.y = node->FloatAttribute("G");
            color.z = node->FloatAttribute("B");
            color.w = node->FloatAttribute("A");
            material->GetMaterialDesc().emissive = color;
        }

        _materials.push_back(material);

        // Next Material
        materialNode = materialNode->NextSiblingElement();
    }

    BindCacheInfo();
}

void Model::ReadModel(wstring filename)
{
    _fileName = filename;

    wstring fullPath = _modelPath + filename + L".mesh";

    shared_ptr<FileUtils> file = make_shared<FileUtils>();
    file->Open(fullPath, FileMode::Read);

    // Bones
    {
        const uint32 count = file->Read<uint32>();

        for (uint32 i = 0; i < count; i++)
        {
            shared_ptr<ModelBone> bone = make_shared<ModelBone>();
            bone->index = file->Read<int32>();
            bone->name = Utils::ToWString(file->Read<string>());
            bone->parentIndex = file->Read<int32>();
            bone->transform = file->Read<Matrix>();

            _bones.push_back(bone);
        }
    }

    // Mesh
    {
        const uint32 count = file->Read<uint32>();

        for (uint32 i = 0; i < count; i++)
        {
            shared_ptr<ModelMesh> mesh = make_shared<ModelMesh>();

            mesh->name = Utils::ToWString(file->Read<string>());
            mesh->boneIndex = file->Read<int32>();

            // Material
            mesh->materialName = Utils::ToWString(file->Read<string>());

            //VertexData
            {
                const uint32 count = file->Read<uint32>();
                vector<ModelVertexType> vertices;
                vertices.resize(count);

                void* data = vertices.data();
                file->Read(&data, sizeof(ModelVertexType) * count);
                mesh->geometry->AddVertices(vertices);
            }

            //IndexData
            {
                const uint32 count = file->Read<uint32>();

                vector<uint32> indices;
                indices.resize(count);

                void* data = indices.data();
                file->Read(&data, sizeof(uint32) * count);
                mesh->geometry->AddIndices(indices);
            }

            mesh->CreateBuffers();

            _meshes.push_back(mesh);
        }
    }

    BindCacheInfo();
}

void Model::ReadAnimation(wstring filename)
{
    wstring fullPath = _modelPath + filename + L".clip";

    shared_ptr<FileUtils> file = make_shared<FileUtils>();
    file->Open(fullPath, FileMode::Read);

    shared_ptr<ModelAnimation> animation = make_shared<ModelAnimation>();

    //animation->name = Utils::ToWString(file->Read<string>());
    file->Read<string>();
    animation->name = FileUtils::PathToAnimName(filename);
    animation->duration = file->Read<float>();
    animation->frameRate = file->Read<float>();
    animation->frameCount = file->Read<uint32>();

    uint32 keyframesCount = file->Read<uint32>();

    for (uint32 i = 0; i < keyframesCount; i++)
    {
        shared_ptr<ModelKeyframe> keyframe = make_shared<ModelKeyframe>();
        keyframe->boneName = Utils::ToWString(file->Read<string>());

        uint32 size = file->Read<uint32>();

        if (size > 0)
        {
            keyframe->transforms.resize(size);
            void* ptr = &keyframe->transforms[0];
            file->Read(&ptr, sizeof(ModelKeyframeData) * size);
        }

        animation->keyframes[keyframe->boneName] = keyframe;
    }

    // 애니메이션 재생 관련 추가
    animation->playRate = file->Read<float>();
    animation->enableRootMotion = (file->Read<uint8>() != 0);

    _animations.push_back(animation);
}

shared_ptr<Material> Model::GetMaterialByName(const wstring& name)
{
    for (auto& material : _materials)
    {
        if (material->GetName() == name)
            return material;
    }

    return nullptr;
}

shared_ptr<ModelMesh> Model::GetMeshByName(const wstring& name)
{
    for (auto& mesh : _meshes)
    {
        if (mesh->name == name)
            return mesh;
    }

    return nullptr;
}

shared_ptr<ModelBone> Model::GetBoneByName(const wstring& name)
{
    for (auto& bone : _bones)
    {
        if (bone->name == name)
            return bone;
    }

    return nullptr;
}

shared_ptr<ModelAnimation> Model::GetAnimationByName(wstring name)
{
    for (auto& animation : _animations)
    {
        if (animation->name == name)
            return animation;
    }

    return nullptr;
}

int32 Model::GetAnimationIndex(const wstring& name)
{
    for (uint32 i = 0; i < _animations.size(); ++i)
    {
        if (_animations[i]->name == name)
            return static_cast<int32>(i);
    }
    return -1;  // 못 찾음
}

void Model::SaveAnimation(int32 animIndex, const wstring& filename)
{
    if (animIndex < 0 || animIndex >= (int32)_animations.size())
        return;

    auto& animation = _animations[animIndex];

    wstring fullPath = _modelPath + filename + L".clip";
    shared_ptr<FileUtils> file = make_shared<FileUtils>();

    file->Open(fullPath, FileMode::Write);

    // ─────────────────────────────────────────────
    // 기존 데이터 (ReadAnimation 역순)
    // ─────────────────────────────────────────────
    file->Write<string>(Utils::ToString(animation->name));
    file->Write<float>(animation->duration);
    file->Write<float>(animation->frameRate);
    file->Write<uint32>(animation->frameCount);

    file->Write<uint32>(animation->keyframes.size());

    for (auto& [boneName, keyframe] : animation->keyframes)
    {
        file->Write<string>(Utils::ToString(keyframe->boneName));
        file->Write<uint32>(keyframe->transforms.size());

        if (keyframe->transforms.size() > 0)
        {
            file->Write(&keyframe->transforms[0],
                sizeof(ModelKeyframeData) * keyframe->transforms.size());
        }
    }

    file->Write<float>(animation->playRate);
    file->Write<uint8>(animation->enableRootMotion ? 1 : 0);
}

void Model::BindCacheInfo()
{
    // 지금까지 만든 머티리얼이나 본에 대해서 하나씩 하나씩
    // 순회해서 Cache와 관련된 부분들을 채워주려는 것이다.

    // Mesh에 Material 캐싱
    for (const auto& mesh : _meshes)
    {
        // 이미 찾았으면 스킵
        if (mesh->material != nullptr)
            continue;

        mesh->material = GetMaterialByName(mesh->materialName);
    }

    // Mesh에 Bone 캐싱
    for (const auto& mesh : _meshes)
    {
        // 이미 찾았으면 스킵
        if (mesh->bone != nullptr)
            continue;

        mesh->bone = GetBoneByIndex(mesh->boneIndex);
    }

    // Bone 계층 정보 채우기
    if (_root == nullptr && _bones.size() > 0)
    {
        _root = _bones[0];

        for (const auto& bone : _bones)
        {
            if (bone->parentIndex >= 0)
            {
                bone->parent = _bones[bone->parentIndex];
                bone->parent->children.push_back(bone);
            }
            else
            {
                bone->parent = nullptr;
            }
        }
    }

}
