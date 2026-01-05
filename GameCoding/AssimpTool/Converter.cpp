#include "pch.h"
#include "Converter.h"
#include <filesystem>

#include "AsTypes.h"
#include "Utils.h"
#include "tinyxml2.h"

Converter::Converter()
{
    _importer = make_shared<Assimp::Importer>();
}

Converter::~Converter()
{
}

void Converter::ReadAssetFile(wstring file)
{
    wstring fileStr = _assetPath + file;

    auto p = std::filesystem::path(fileStr);
    assert(std::filesystem::exists(p)); // 파일이 있는지 확인

    _scene = _importer->ReadFile(
        Utils::ToString(fileStr),       // 1. 읽어올 파일의 경로

        // ▼ 모델 데이터 후처리 옵션들 (비트 OR 연산으로 조합)
        aiProcess_ConvertToLeftHanded | // 2. 좌표계를 DirectX용(왼손 좌표계)으로 변환
        aiProcess_Triangulate |         // 3. 모든 면(Face)을 삼각형으로 쪼갬 (사각형 등 -> 삼각형)
        aiProcess_GenUVCoords |         // 4. 매핑 방식이 다른 텍스처 좌표를 UV 좌표로 변환
        aiProcess_GenNormals |          // 5. 노멀(법선) 벡터가 없다면 자동으로 생성 (조명 연산용)
        aiProcess_CalcTangentSpace      // 6. 탄젠트/바이탄젠트 생성 (노멀 매핑용)
    );
    
    assert(_scene != nullptr);
}

void Converter::ExportModelData(wstring savePath)
{
    // 받은 데이터를 추출해서 별도의 우리만의 파일로 만들어줘라.
    wstring finalPath = _modelPath + savePath + L".mesh";

    // 최상위 루트는 -1, -1이라고 번호를 지정해주고 그 다음에 타고 가서
    // 다음 자식들을 0, 1, 2 이렇게 계층적인 번호를 만들어주도록 할 것이다.
    ReadModelData(_scene->mRootNode, -1, -1);
    // 메모리에 있는 데이터를 최종 파일로 만들기
    WriteModelFile(finalPath);
}

void Converter::ExportMaterialData(wstring savePath)
{
    wstring finalPath = _texturePath + savePath + L".xml";
    ReadMaterialData();
    WriteMaterialData(finalPath);
}

void Converter::ReadMeshData(aiNode* node, int32 bone)
{

}

void Converter::WriteModelFile(wstring finalPath)
{

}

void Converter::ReadModelData(aiNode* node, int32 index, int32 parent)
{

}

void Converter::ReadMaterialData()
{
    for (uint32 i = 0; i < _scene->mNumMaterials; i++)
    {
        aiMaterial* srcMaterial = _scene->mMaterials[i];

        shared_ptr<asMaterial> material = make_shared<asMaterial>();
        material->name = srcMaterial->GetName().C_Str();

        aiColor3D color;
        // Ambient
        srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
        material->ambient = Color(color.r, color.g, color.b, 1.f);

        // Diffuse Texture (Albedo)
        srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        material->diffuse = Color(color.r, color.g, color.b, 1.f);

        // Specular Texture
        srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
        material->specular = Color(color.r, color.g, color.b, 1.f);
        srcMaterial->Get(AI_MATKEY_SHININESS, material->specular.w);

        // Emissive
        srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        material->emissive = Color(color.r, color.g, color.b, 1.f);

        aiString file;

        // Diffuse Texture
        srcMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
        material->diffuseFile = file.C_Str();

        // Specular Texture
        srcMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
        material->specularFile = file.C_Str();

        // Normal Texture
        srcMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);
        material->normalFile = file.C_Str();

        _materials.push_back(material);
    }
}

void Converter::WriteMaterialData(wstring finalPath)
{
    // ~~/House
    auto path = filesystem::path(finalPath);

    // 폴더가 없으면 만든다.
    filesystem::create_directory(path.parent_path());

    string folder = path.parent_path().string();

    shared_ptr<tinyxml2::XMLDocument> document = make_shared<tinyxml2::XMLDocument>();

    tinyxml2::XMLDeclaration* decl = document->NewDeclaration();
    document->LinkEndChild(decl);

    tinyxml2::XMLElement* root = document->NewElement("Materials");
    document->LinkEndChild(root);

    // 모든 material들을 하나씩 순회하면서 이름을 지어줘야 한다.
    for (shared_ptr<asMaterial> material : _materials)
    {
        tinyxml2::XMLElement* node = document->NewElement("Material");
        root->LinkEndChild(node);

        tinyxml2::XMLElement* element = nullptr;

        element = document->NewElement("Name");
        element->SetText(material->name.c_str());
        node->LinkEndChild(element);

        element = document->NewElement("DiffuseFile");
        element->SetText(WriteTexture(folder, material->diffuseFile).c_str());
        node->LinkEndChild(element);

        element = document->NewElement("SpecularFile");
        element->SetText(WriteTexture(folder, material->specularFile).c_str());
        node->LinkEndChild(element);

        element = document->NewElement("NormalFile");
        element->SetText(WriteTexture(folder, material->normalFile).c_str());
        node->LinkEndChild(element);

        element = document->NewElement("Ambient");
        element->SetAttribute("R", material->ambient.x);
        element->SetAttribute("G", material->ambient.y);
        element->SetAttribute("B", material->ambient.z);
        element->SetAttribute("A", material->ambient.w);
        node->LinkEndChild(element);

        element = document->NewElement("Diffuse");
        element->SetAttribute("R", material->diffuse.x);
        element->SetAttribute("G", material->diffuse.y);
        element->SetAttribute("B", material->diffuse.z);
        element->SetAttribute("A", material->diffuse.w);
        node->LinkEndChild(element);

        element = document->NewElement("Specular");
        element->SetAttribute("R", material->specular.x);
        element->SetAttribute("G", material->specular.y);
        element->SetAttribute("B", material->specular.z);
        element->SetAttribute("A", material->specular.w);
        node->LinkEndChild(element);

        element = document->NewElement("Emissive");
        element->SetAttribute("R", material->emissive.x);
        element->SetAttribute("G", material->emissive.y);
        element->SetAttribute("B", material->emissive.z);
        element->SetAttribute("A", material->emissive.w);
        node->LinkEndChild(element);
    }

    document->SaveFile(Utils::ToString(finalPath).c_str());
}

string Converter::WriteTexture(string saveFolder, string file)
{
    return "";
}
