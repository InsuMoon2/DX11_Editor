#include "pch.h"
#include "AssimpTool.h"
#include "Converter.h"

void AssimpTool::Init()
{
    {
        shared_ptr<Converter> converter = make_shared<Converter>();

        converter->ReadAssetFile(L"Kachujin/Mesh.fbx");
        converter->ExportMaterialData(L"Kachujin/Kachujin");
        converter->ExportModelData(L"Kachujin/Kachujin");
    }

    {
        shared_ptr<Converter> converter = make_shared<Converter>();

        converter->ReadAssetFile(L"Kachujin/Idle.fbx");
        converter->ExportAnimationData(L"Kachujin/Idle");
    }

    {
        shared_ptr<Converter> converter = make_shared<Converter>();

        converter->ReadAssetFile(L"Kachujin/Run.fbx");
        converter->ExportAnimationData(L"Kachujin/Run");
    }

    {
        shared_ptr<Converter> converter = make_shared<Converter>();

        converter->ReadAssetFile(L"Kachujin/Slash.fbx");
        converter->ExportAnimationData(L"Kachujin/Slash");
    }

    // 데이다라
   /* {
        shared_ptr<Converter> converter = make_shared<Converter>();

        converter->ReadAssetFile(L"Deidara/SK_CHR_Deidara.fbx");
        converter->ExportMaterialData(L"Deidara/Deidara");
        converter->ExportModelData(L"Deidara/Deidara");
    }

    {
        shared_ptr<Converter> converter = make_shared<Converter>();
        converter->ReadAssetFile(L"Deidara/aerial.fbx");
        converter->ExportAnimationData(L"Deidara/aerial");
    }
    {
        shared_ptr<Converter> converter = make_shared<Converter>();
        converter->ReadAssetFile(L"Deidara/aerial2.fbx");
        converter->ExportAnimationData(L"Deidara/aerial2");
    }
    {
        shared_ptr<Converter> converter = make_shared<Converter>();
        converter->ReadAssetFile(L"Deidara/atk.fbx");
        converter->ExportAnimationData(L"Deidara/atk");
    }
    {
        shared_ptr<Converter> converter = make_shared<Converter>();
        converter->ReadAssetFile(L"Deidara/dragon.fbx");
        converter->ExportAnimationData(L"Deidara/dragon");
    }
    {
        shared_ptr<Converter> converter = make_shared<Converter>();
        converter->ReadAssetFile(L"Deidara/dragon2.fbx");
        converter->ExportAnimationData(L"Deidara/dragon2");
    }*/
}

void AssimpTool::Update()
{
    
}

void AssimpTool::Render()
{
    
}
