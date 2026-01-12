#include "pch.h"
#include "EventManager.h"

void EventManager::Publish(shared_ptr<Event> event)
{
    _eventQueue.push(event);
}

void EventManager::Subscribe(EventType type, EventCallback callback)
{
    _subscribers[type].push_back(callback);
}

void EventManager::ProcessEvents()
{
    while (!_eventQueue.empty())
    {
        // 큐 맨 앞의 이벤트 꺼내기
        auto event = _eventQueue.front();
        _eventQueue.pop();

        EventType type = event->GetType();

        // 해당 타입을 구독한 콜백이 있는지 확인
        if (_subscribers.find(type) != _subscribers.end())
        {
            // 구독자들에게 이벤트 전달
            for (auto& callback : _subscribers[type])
            {
                callback(*event); // 콜백 실행

                // handled가 true면 다른 구독자에게 전달 안함
                if (event->handled)
                    break;
            }
        }
    }
}
