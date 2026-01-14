#pragma once

#include "AnimNotify.h"

class AnimNotifyFactory
{
public:
    using NotifyCreator = function<shared_ptr<AnimNotify>()>;

    // 생성자 등록/조회
    static void Register(const string& typeName, NotifyCreator creator);
    static shared_ptr<AnimNotify> CreateNotify(const string& typeName);

    // 등록된 모든 생성자 맵
    static map<string, NotifyCreator>& GetCreators();
};

#define REGISTER_ANIM_NOTIFY(TYPE) \
    static struct Helper_Notify_##TYPE { \
        Helper_Notify_##TYPE() { \
            AnimNotifyFactory::Register(#TYPE, []() { return make_shared<TYPE>(); }); \
        } \
    } helper_notify_##TYPE;

