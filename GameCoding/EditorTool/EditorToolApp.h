#pragma once

#include "Engine/IExecute.h"

class Model;

class EditorToolApp : public IExecute
{
public:
    void Init() override;
    void Update() override;
    void Render() override;

    void CreatePlayer();

    void RegisterHierarchy();
    void RegisterContentBrowser();

private:
    shared_ptr<Shader>       _shader;
    shared_ptr<Model>        _model;

    shared_ptr<GameObject>   _player;

    vector<wstring>         _animPaths;
};

