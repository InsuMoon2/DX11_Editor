#pragma once

enum class EventType
{
    None = 0,
    GameObject_Created,
    GameObject_Destroyed,
    Scene_Changed,

    /* 추후 추가 */
};

class Event
{
public:
    Event() = default;
    virtual ~Event() = default;
    virtual EventType GetType() const = 0;
    bool handled = false;
};

// 이벤트 타입 선언 자동화 매크로
#define EVENT_TYPE(type)                                                    \
    static EventType GetStaticType() { return EventType::type; }            \
    virtual EventType GetType() const override { return GetStaticType(); }

