#pragma once

class EditorWindow
{
public:
    EditorWindow(const string& name);
    virtual ~EditorWindow();

public:
    virtual void Init() {}
    virtual void Update() {}
    virtual void OnGui() = 0;

public:
    const string& GetName() const { return _name; }
    bool IsActive() const { return _isActive; }
    void SetActive(bool active) { _isActive = active; }

protected:
    string _name;
    bool _isActive = true;

};

