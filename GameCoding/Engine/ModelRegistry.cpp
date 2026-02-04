#include "pch.h"
#include "ModelRegistry.h"

ModelRegistry::~ModelRegistry()
{
    Clear();
}

void ModelRegistry::RegisterModel(const wstring& key, shared_ptr<Model> model, shared_ptr<ModelAnimator> animator,
    const vector<wstring>& animPaths)
{
    ModelEntry entry;
    entry.model = model;
    entry.animator = animator;
    entry.animPaths = animPaths;

    _entries[key] = entry;
}

ModelEntry* ModelRegistry::GetEntry(const wstring& key)
{
    auto it = _entries.find(key);
    if (it != _entries.end())
    {
        return &it->second;
    }

    return nullptr;
}

void ModelRegistry::Clear()
{
    _entries.clear();
}


