#pragma once

#include "IExecute.h"

class GameDemo : public IExecute
{
public:
    void Init() override;
    void Update() override;
    void Render() override;

private:
    shared_ptr<Scene>   _startScene;
    shared_ptr<Shader>  _shader;


};

