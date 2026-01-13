#pragma once

#include "Scene.h"

class Player;

class Stage01 : public Scene
{
public:
    Stage01();
    ~Stage01();

public:
    void Init(shared_ptr<Shader> shader);

private:
    shared_ptr<Player> _player;
};

