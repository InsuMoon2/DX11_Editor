#pragma once

#include "Model.h"

class ModelRegistry
{
    DECLARE_SINGLE(ModelRegistry)

public:
    ~ModelRegistry();

    // 모델을 레지스터에 등록
    void RegisterModel(const wstring& key, shared_ptr<Model> model);
    shared_ptr<Model> GetModel(const wstring& key);

    map<wstring, shared_ptr<Model>>& GetAllModel() { return _models; }

    void Clear();

private:
    map<wstring, shared_ptr<Model>> _models;

};

