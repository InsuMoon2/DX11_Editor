#pragma once

class ModelAnimator;

class AnimNotify
{
public:
    AnimNotify();
    virtual ~AnimNotify() = default;

    virtual wstring GetDisplayName() const = 0;
    virtual void OnNotify(ModelAnimator* animator) = 0;
    virtual void OnGui();

public:
    virtual void Serialize(json& j) const;
    virtual void Deserialize(const json& j);

public:
    int  GetFrame() { return _frame; }
    void SetFrame(int frame) { _frame = frame; }

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

        // 컴파일 타임 조건 분기
        if constexpr (std::is_enum_v<T>)
        {
            // enum -> int로 저장
            prop.serialize = [ptr, name](json& j)
                {
                    j[name] = static_cast<int>(*ptr);
                };
            prop.deserialize = [ptr, name](const json& j)
                {
                    if (j.contains(name))
                    {
                        *ptr = static_cast<T>(j[name].get<int>());
                    }
                };
        }
        else
        {
            // 일반 타입
            prop.serialize = [ptr, name](json& j)
                {
                    j[name] = *ptr;
                };
            prop.deserialize = [ptr, name](const json& j)
                {
                    *ptr = j[name].get<T>();
                };
        }

        _properties.push_back(prop);
    }

protected:
    // 공통 데이터
    int _frame = 0;
};

