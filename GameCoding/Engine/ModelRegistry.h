#pragma once

#include "Model.h"

class ModelAnimator;

struct ModelEntry
{
    shared_ptr<Model> model;
    shared_ptr<ModelAnimator> animator;
    vector<wstring> animPaths;
};

class ModelRegistry
{
    DECLARE_SINGLE(ModelRegistry)

public:
    ~ModelRegistry();

    void RegisterModel(const wstring& key,
        shared_ptr<Model> model,
        shared_ptr<ModelAnimator> animator,
        const vector<wstring>& animPaths);

    ModelEntry* GetEntry(const wstring& key);

    map<wstring, ModelEntry>& GetAll() { return _entries; }

    void Clear();

private:
    map<wstring, ModelEntry> _entries;

};

