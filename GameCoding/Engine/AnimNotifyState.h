#pragma once

class ModelAnimator;

class AnimNotifyState
{
public:
    AnimNotifyState();
    virtual ~AnimNotifyState() = default;

    virtual wstring GetDisplayName() const = 0;

    virtual void OnNotifyBegin(ModelAnimator* animator) = 0;
    virtual void OnNotifyTick(ModelAnimator* animator, float deltaTime) = 0;
    virtual void OnNotifyEnd(ModelAnimator* animator) = 0;

public:
    virtual void Serialize(json& j) const;
    virtual void Deserialize(const json& j);

public:
    int  GetStartFrame() { return _startFrame; }
    int  GetEndFrame() { return _endFrame; }

    void SetStartFrame(int frame) { _startFrame = frame; }
    void SetEndFrame(int frame) { _endFrame = frame; }

protected:
    struct PropertyBase
    {
        function<void(json&)> serialize;
        function<void(const json&)> deserialize;
    };

    vector<PropertyBase> _properties;
    template<typename T>
    void Register(const string& name, T* ptr)
    {
        PropertyBase prop;
        if constexpr (std::is_enum_v<T>)
        {
            prop.serialize = [ptr, name](json& j)
            {
                j[name] = static_cast<int>(*ptr);
                };
            prop.deserialize = [ptr, name](const json& j)
            {
                if (j.contains(name))
                    *ptr = static_cast<T>(j[name].get<int>());
                };
        }
        else
        {
            prop.serialize = [ptr, name](json& j)
            {
                j[name] = *ptr;
                };
            prop.deserialize = [ptr, name](const json& j)
            {
                if (j.contains(name))
                    *ptr = j[name].get<T>();
                };
        }
        _properties.push_back(prop);
    }

protected:
    int _startFrame = 0;
    int _endFrame = 0;

};

