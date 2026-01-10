#pragma once

#include "Engine/IExecute.h"

class EditorToolApp : public IExecute
{
public:
    void Init() override;
    void Update() override;
    void Render() override;

    void CreatePlayer();
    void RegisterHierarchy();

private:
    shared_ptr<Shader>       _shader;
    shared_ptr<GameObject>  _player;
};

