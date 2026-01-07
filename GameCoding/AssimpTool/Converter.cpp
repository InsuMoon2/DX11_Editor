#include "pch.h"
#include "Converter.h"
#include <filesystem>
#include "AsTypes.h"
#include "Utils.h"
#include "tinyxml2.h"
#include "FileUtils.h"

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

void Converter::ReadModelData(aiNode* node, int32 index, int32 parent)
{
    shared_ptr<asBone> bone = make_shared<asBone>();
    bone->index = index;
    bone->parent = parent;
    bone->name = node->mName.C_Str();

    // 첫번째 주소를 넘겨주게 되면 Matrix 의 생성자에서 Float 주소를 하나 받는 버전이 있는데,
    // 이 주소를 받았으면 16개 숫자를 알아서 복사해줄 것이기 때문에 이 방법이 가장 편하다.
    // 숫자를 하나만 넣어주는게 아니라 16개를 복사한다는 것을 주의해야 함.
    Matrix transform(node->mTransformation[0]); // Relative Transform
    bone->transform = transform.Transpose(); // 한번 뒤집어줘야지만 왼쪽 방향으로 행렬이 맞춰진다고 한다.

    // Root (Local)
    Matrix matParent = Matrix::Identity;
    if (parent >= 0)
        matParent = _bones[parent]->transform;

    // Local (Root) Transform 부모를 하나하나 타고 곱해주면 된다.
    bone->transform = bone->transform * matParent;

    _bones.push_back(bone);

    // Mesh
    ReadMeshData(node, index);

    // 재귀
    for (uint32 i = 0; i < node->mNumChildren; i++)
        ReadModelData(node->mChildren[i], _bones.size(), index);
}

void Converter::ReadMeshData(aiNode* node, int32 bone)
{
    if (node->mNumMeshes < 1)
        return;

    shared_ptr<asMesh> mesh = make_shared<asMesh>();
    mesh->name = node->mName.C_Str();
    mesh->boneIndex = bone;

    // 메쉬가 경우에 따라서 여러개 있는 경우가 있다.
    for (uint32 i = 0; i < node->mNumMeshes; i++)
    {
        uint32 index = node->mMeshes[i];
        const aiMesh* srcMesh = _scene->mMeshes[index];

        // Material Name
        const aiMaterial* material = _scene->mMaterials[srcMesh->mMaterialIndex];
        mesh->materialName = material->GetName().C_Str();

        const uint32 startVertex = mesh->verticies.size();

        for (uint32 v = 0; v < srcMesh->mNumVertices; v++)
        {
            // Vertex
            VertexType vertex;
            ::memcpy(&vertex.position, &srcMesh->mVertices[v], sizeof(Vec3));

            // UV
            if (srcMesh->HasTextureCoords(0))
                ::memcpy(&vertex.uv, &srcMesh->mTextureCoords[0][v], sizeof(Vec2));

            // Normal
            if (srcMesh->HasNormals())
                ::memcpy(&vertex.normal, &srcMesh->mNormals[v], sizeof(Vec3));

            mesh->verticies.push_back(vertex);
        }

        // Index -> 0~3312, 3312 ~ 678, 3990 ~ 109~~~ 이렇게 인덱스 번호가 겹치지 않게끔 만들기 위함
        for (uint32 f = 0; f < srcMesh->mNumFaces; f++)
        {
            aiFace& face = srcMesh->mFaces[f];

            for (uint32 k = 0; k < face.mNumIndices; k++)
            {
                mesh->indices.push_back(face.mIndices[k] + startVertex);
            }
        }
    }

    _meshes.push_back(mesh);

}

void Converter::WriteModelFile(wstring finalPath)
{
    // 바이너리 파일로 만들어서 관리를 하게 될 것이다.
    
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
    // 파일 이름 추출
    string fileName = filesystem::path(file).filename().string();
    // 폴더 이름 추출
    string folderName = filesystem::path(saveFolder).filename().string();

    // 경우에 따라서 fbx파일에 머티리얼이 들어가 있는 경우가 있는데, 웬만하면 따로 나뉘어져있는 걸 사용하길 권장
    const aiTexture* srcTexture = _scene->GetEmbeddedTexture(file.c_str());
    if (srcTexture)
    {
        string pathStr = saveFolder + fileName;

        if (srcTexture->mHeight == 0)
        {
            //shared_ptr<FileUtils> file = make_shared<FileUtils>();
            //file->Open(Utils::ToWString(pathStr), FileMode::Write);
            //file->Write(srcTexture->pcData, srcTexture->mWidth);
        }
        else
        {
            D3D11_TEXTURE2D_DESC desc;
            ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
            desc.Width = srcTexture->mWidth;
            desc.Height = srcTexture->mHeight;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Usage = D3D11_USAGE_IMMUTABLE;

            D3D11_SUBRESOURCE_DATA subResource = { 0 };
            subResource.pSysMem = srcTexture->pcData;

            ComPtr<ID3D11Texture2D> texture;
            HRESULT hr = DEVICE->CreateTexture2D(&desc, &subResource, texture.GetAddressOf());
            CHECK(hr);

            DirectX::ScratchImage img;
            ::CaptureTexture(DEVICE.Get(), DC.Get(), texture.Get(), img);

            // Save To File
            hr = DirectX::SaveToDDSFile(*img.GetImages(), DirectX::DDS_FLAGS_NONE, Utils::ToWString(fileName).c_str());
            CHECK(hr);
        }
    }
    else // 텍스처가 내장으로 들어가 있지 않을 때
    {
        string originStr = (filesystem::path(_assetPath) / folderName / file).string();
        Utils::Replace(originStr, "\\", "/");

        string pathStr = (filesystem::path(saveFolder) / fileName).string();
        Utils::Replace(pathStr, "\\", "/");

        ::CopyFileA(originStr.c_str(), pathStr.c_str(), false);
    }

    return fileName;
}
