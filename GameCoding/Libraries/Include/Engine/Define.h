#pragma once

#define DECLARE_SINGLE(classname)			\
private:									\
	classname() { }							\
public:										\
	static classname* GetInstance()			\
	{										\
		static classname s_instance;		\
		return &s_instance;					\
	}

#define REGISTER_GAMEOBJECT(TYPE) \
    static struct Helper_##TYPE { \
        Helper_##TYPE() { \
            HierarchyView::RegisterCreationType(#TYPE, []() { return make_shared<TYPE>(); }); \
        } \
    } helper_##TYPE;

#define U8(str) reinterpret_cast<const char*>(u8##str)

#define GET_SINGLE(classname)	classname::GetInstance()

#define CHECK(p)	        assert(SUCCEEDED(p))
#define GAME		        GET_SINGLE(Game)		
#define GRAPHICS	        GET_SINGLE(Graphics)
#define DEVICE		        GRAPHICS->GetDevice()
#define ENGINE_DC			GRAPHICS->GetDeviceContext()
#define INPUT		        GET_SINGLE(InputManager)
#define TIME		        GET_SINGLE(TimeManager)
#define DT			        TIME->GetDeltaTime()
#define RESOURCES	        GET_SINGLE(ResourceManager)
#define RENDER		        GET_SINGLE(RenderManager)
#define INSTANCING		    GET_SINGLE(InstancingManager)
#define SCENE               GET_SINGLE(SceneManager)
#define CUR_SCENE           SCENE->GetCurrentScene()

#define GUI                 GET_SINGLE(ImGuiManager)
#define EVENTS              GET_SINGLE(EventManager)
