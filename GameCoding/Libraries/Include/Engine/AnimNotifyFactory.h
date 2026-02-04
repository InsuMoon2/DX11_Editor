#pragma once

#include "AnimNotify.h"
#include "AnimNotifyState.h"

class AnimNotifyFactory
{
public:
    // Notify
    using NotifyCreator = function<shared_ptr<AnimNotify>()>;

    static void RegisterNotify(const string& typeName, NotifyCreator creator);
    static shared_ptr<AnimNotify> CreateNotify(const string& typeName);
    static map<string, NotifyCreator>& GetNotifyCreators();

public:
    // Notify State
    using NotifyStateCreator = function < shared_ptr<AnimNotifyState>()>;

    static void RegisterNotifyState(const string& typeName, NotifyStateCreator creator);
    static shared_ptr<AnimNotifyState> CreateNotifyState(const string& typeName);
    static map<string, NotifyStateCreator>& GetNotifyStateCreators();

};

#define REGISTER_ANIM_NOTIFY(TYPE, NAME) \
    static struct Helper_Notify_##TYPE { \
        Helper_Notify_##TYPE() { \
            AnimNotifyFactory::RegisterNotify(NAME, []() { return make_shared<TYPE>(); }); \
        } \
    } helper_notify_##TYPE;

#define REGISTER_ANIM_NOTIFY_STATE(TYPE, NAME) \
    static struct Helper_NotifyState_##TYPE { \
        Helper_NotifyState_##TYPE() { \
            AnimNotifyFactory::RegisterNotifyState(NAME, []() { return make_shared<TYPE>(); }); \
        } \
    } helper_notifyState_##TYPE;

