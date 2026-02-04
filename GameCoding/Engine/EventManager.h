#pragma once

#include "Event.h"

using EventCallback = function<void(Event&)>;

class EventManager
{
    DECLARE_SINGLE(EventManager)

public:
    // ─────────────────────────────────────────────
    // 이벤트 발행 (큐에 추가)
    // ─────────────────────────────────────────────
    void Publish(shared_ptr<Event> event);

    // ─────────────────────────────────────────────
    // 이벤트 구독 (콜백 등록)
    // ─────────────────────────────────────────────
    void Subscribe(EventType type, EventCallback callback);

    // ─────────────────────────────────────────────
    // 큐에 쌓인 이벤트 처리 (프레임 끝에 호출)
    // ─────────────────────────────────────────────
    void ProcessEvents();

private:
    queue<shared_ptr<Event>> _eventQueue;
    unordered_map<EventType, vector<EventCallback>> _subscribers;

};

