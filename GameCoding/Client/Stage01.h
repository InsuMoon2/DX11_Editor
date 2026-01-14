#pragma once

#include "Scene.h"

class Player;

class Stage01 : public Scene
{
public:
    Stage01();
    ~Stage01();

public:
    void Init() override;
    void Update() override;

    shared_ptr<Player> GetPlayer() { return _player; }

private:
    shared_ptr<Player> _player;
};

