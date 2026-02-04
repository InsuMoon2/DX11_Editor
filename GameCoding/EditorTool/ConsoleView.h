#pragma once

#include "EditorWindow.h"

class ConsoleView : public EditorWindow
{
public:
    ConsoleView();
    virtual ~ConsoleView() override;

public:
    void Init() override;
    void Update() override;
    void OnGui() override;

private:
    bool _autoScroll = true;
    char _searchBuffer[256] = "";

};

