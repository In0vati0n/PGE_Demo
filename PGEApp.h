#pragma once

#include <cassert>
#include <functional>
#include <iostream>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#if defined(BUILD_LUA_AS_CLIB)
extern "C"
{
#endif
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#if defined(BUILD_LUA_AS_CLIB)
}
#endif

#include "LuaBridge.h"

namespace PGEApp
{
    static int MajorVersion = 0;
    static int MinorVersion = 1;
    const char *PGELuaTableName = "PGE";

    namespace _
    {
        ///////////////////////////////////////////
        // Utils
        ///////////////////////////////////////////
        static int Traceback(lua_State *L)
        {
            lua_getglobal(L, "debug");
            lua_getfield(L, -1, "traceback");
            lua_pushvalue(L, 1);
            lua_pushinteger(L, 2);
            lua_call(L, 2, 1);
            return 1;
        }

        static int CallLuaFunc(lua_State *L, const char *name)
        {
            lua_pushcfunction(L, Traceback);
            lua_getglobal(L, name);

            int ret = lua_pcall(L, 0, 0, lua_gettop(L) - 1);
            if (ret != 0)
            {
                const char *error = lua_tostring(L, -1);

                std::cout << "[Call Lua Error] " << error << std::endl;

                lua_getglobal(L, "_pge_error");
                lua_pushstring(L, error);
                lua_pcall(L, 1, 0, 0);
            }

            return ret;
        }

        static int LuaGetTableIntField(lua_State *L, const char *fieldName)
        {
            int fieldType = lua_getfield(L, -1, fieldName);
            assert(fieldType == LUA_TNUMBER);
            int ret = lua_tointeger(L, -1);
            lua_pop(L, 1);
            return ret;
        }

        static int TimerRegisterFunctions(lua_State *L);
        static int GraphicsRegisterFunctions(lua_State *L);
        static int InputRegisterFunctions(lua_State *L);

        /////////////////////////////////////////////////
        // App
        /////////////////////////////////////////////////
        class App : public olc::PixelGameEngine
        {
        public:
            App()
            {
                sAppName = "App";

                L = luaL_newstate();
                InitLua();
            }

            virtual ~App()
            {
                lua_close(L);
                L = nullptr;
            }

        public:
            bool OnUserCreate() override
            {
                CallLuaFunc(L, "_pge_load");
                return true;
            }

            bool OnUserUpdate(float fElapsedTime) override
            {
                AccumulatedTime += fElapsedTime;

                while (AccumulatedTime >= TargetFrameTime)
                {
                    AccumulatedTime -= TargetFrameTime;
                    DeltaTime = TargetFrameTime;
                    CallLuaFunc(L, "_pge_update");
                }

                return true;
            }

            bool OnUserDestroy() override
            {
                CallLuaFunc(L, "_pge_on_destroy");
                return true;
            }

            inline lua_State *GetLuaState() { return L; }

            inline int GetScreenWidth() const { return ScreenWidth; }
            inline int GetScreenHeight() const { return ScreenHeight; }
            inline int GetScreenXScale() const { return ScreenXScale; }
            inline int GetScreenYScale() const { return ScreenYScale; }
            inline float GetDeltaTime() const { return DeltaTime; }

        private:
            bool InitLuaTimer()
            {
                TimerRegisterFunctions(L);
                return true;
            }

            bool InitLuaGraphics()
            {
                GraphicsRegisterFunctions(L);
                return true;
            }

            bool InitLuaInput()
            {
                InputRegisterFunctions(L);

                auto ns = luabridge::getGlobalNamespace(L)
                              .beginNamespace(PGELuaTableName)
                              .beginNamespace("input")
                              .endNamespace()
                              .endNamespace();

                lua_getglobal(L, PGELuaTableName);
                lua_getfield(L, -1, "input");

                lua_pushstring(L, "Key");
                lua_newtable(L);
                lua_rawset(L, -3);

                lua_getfield(L, -1, "Key");

                lua_pushinteger(L, olc::Key::A);
                lua_setfield(L, -2, "A");
                lua_pushinteger(L, olc::Key::B);
                lua_setfield(L, -2, "B");
                lua_pushinteger(L, olc::Key::C);
                lua_setfield(L, -2, "C");
                lua_pushinteger(L, olc::Key::D);
                lua_setfield(L, -2, "D");
                lua_pushinteger(L, olc::Key::E);
                lua_setfield(L, -2, "E");
                lua_pushinteger(L, olc::Key::F);
                lua_setfield(L, -2, "F");
                lua_pushinteger(L, olc::Key::G);
                lua_setfield(L, -2, "G");
                lua_pushinteger(L, olc::Key::H);
                lua_setfield(L, -2, "H");
                lua_pushinteger(L, olc::Key::I);
                lua_setfield(L, -2, "I");
                lua_pushinteger(L, olc::Key::J);
                lua_setfield(L, -2, "J");
                lua_pushinteger(L, olc::Key::K);
                lua_setfield(L, -2, "K");
                lua_pushinteger(L, olc::Key::L);
                lua_setfield(L, -2, "L");
                lua_pushinteger(L, olc::Key::M);
                lua_setfield(L, -2, "M");
                lua_pushinteger(L, olc::Key::N);
                lua_setfield(L, -2, "N");
                lua_pushinteger(L, olc::Key::O);
                lua_setfield(L, -2, "O");
                lua_pushinteger(L, olc::Key::P);
                lua_setfield(L, -2, "P");
                lua_pushinteger(L, olc::Key::Q);
                lua_setfield(L, -2, "Q");
                lua_pushinteger(L, olc::Key::R);
                lua_setfield(L, -2, "R");
                lua_pushinteger(L, olc::Key::S);
                lua_setfield(L, -2, "S");
                lua_pushinteger(L, olc::Key::T);
                lua_setfield(L, -2, "T");
                lua_pushinteger(L, olc::Key::U);
                lua_setfield(L, -2, "U");
                lua_pushinteger(L, olc::Key::V);
                lua_setfield(L, -2, "V");
                lua_pushinteger(L, olc::Key::W);
                lua_setfield(L, -2, "W");
                lua_pushinteger(L, olc::Key::X);
                lua_setfield(L, -2, "X");
                lua_pushinteger(L, olc::Key::Y);
                lua_setfield(L, -2, "Y");
                lua_pushinteger(L, olc::Key::Z);
                lua_setfield(L, -2, "Z");

                lua_pushinteger(L, olc::Key::K0);
                lua_setfield(L, -2, "K0");
                lua_pushinteger(L, olc::Key::K1);
                lua_setfield(L, -2, "K1");
                lua_pushinteger(L, olc::Key::K2);
                lua_setfield(L, -2, "K2");
                lua_pushinteger(L, olc::Key::K3);
                lua_setfield(L, -2, "K3");
                lua_pushinteger(L, olc::Key::K4);
                lua_setfield(L, -2, "K4");
                lua_pushinteger(L, olc::Key::K5);
                lua_setfield(L, -2, "K5");
                lua_pushinteger(L, olc::Key::K6);
                lua_setfield(L, -2, "K6");
                lua_pushinteger(L, olc::Key::K7);
                lua_setfield(L, -2, "K7");
                lua_pushinteger(L, olc::Key::K8);
                lua_setfield(L, -2, "K8");
                lua_pushinteger(L, olc::Key::K9);
                lua_setfield(L, -2, "K9");

                lua_pushinteger(L, olc::Key::F1);
                lua_setfield(L, -2, "F1");
                lua_pushinteger(L, olc::Key::F2);
                lua_setfield(L, -2, "F2");
                lua_pushinteger(L, olc::Key::F3);
                lua_setfield(L, -2, "F3");
                lua_pushinteger(L, olc::Key::F4);
                lua_setfield(L, -2, "F4");
                lua_pushinteger(L, olc::Key::F5);
                lua_setfield(L, -2, "F5");
                lua_pushinteger(L, olc::Key::F6);
                lua_setfield(L, -2, "F6");
                lua_pushinteger(L, olc::Key::F7);
                lua_setfield(L, -2, "F7");
                lua_pushinteger(L, olc::Key::F8);
                lua_setfield(L, -2, "F8");
                lua_pushinteger(L, olc::Key::F9);
                lua_setfield(L, -2, "F9");
                lua_pushinteger(L, olc::Key::F10);
                lua_setfield(L, -2, "F10");
                lua_pushinteger(L, olc::Key::F11);
                lua_setfield(L, -2, "F11");
                lua_pushinteger(L, olc::Key::F12);
                lua_setfield(L, -2, "F12");

                lua_pushinteger(L, olc::Key::UP);
                lua_setfield(L, -2, "UP");
                lua_pushinteger(L, olc::Key::DOWN);
                lua_setfield(L, -2, "DOWN");
                lua_pushinteger(L, olc::Key::LEFT);
                lua_setfield(L, -2, "LEFT");
                lua_pushinteger(L, olc::Key::RIGHT);
                lua_setfield(L, -2, "RIGHT");

                lua_pop(L, 3);

                return true;
            }

            bool InitLuaLibs()
            {
                InitLuaTimer();
                InitLuaGraphics();
                InitLuaInput();
                return true;
            }

            bool InitConfig()
            {
                auto luaConfigFunc = luabridge::getGlobal(L, "_pge_config");
                auto config = luaConfigFunc()[0];

                luabridge::getGlobalNamespace(L)
                    .beginNamespace(PGELuaTableName)
                    .addProperty("config", &config, false)
                    .endNamespace();

                sAppName = (const char *)config["title"];

                ScreenWidth = (int)config["screen_width"];
                ScreenHeight = (int)config["screen_height"];
                ScreenXScale = (int)config["screen_x_scale"];
                ScreenYScale = (int)config["screen_y_scale"];
                TargetFrame = (int)config["target_frame"];
                TargetFrameTime = 1.0f / TargetFrame;

                return true;
            }

            bool InitLua()
            {
                luaL_openlibs(L);

                luabridge::getGlobalNamespace(L)
                    .beginNamespace(PGELuaTableName)
                    .addProperty("MajorVersion", &MajorVersion, false)
                    .addProperty("MinorVersion", &MinorVersion, false)
                    .endNamespace();

                // init lua libs
                InitLuaLibs();

                // load pge engine lua code
                luaL_dostring(L, "require('_pge')");

                // require gameplay code
                luaL_dostring(L, "require('game')");

                InitConfig();

                return true;
            }

        private:
            lua_State *L;

            float DeltaTime;

            int TargetFrame = 60;
            float TargetFrameTime = 0;
            float AccumulatedTime = 0;

            int ScreenWidth = 200;
            int ScreenHeight = 200;
            int ScreenXScale = 2;
            int ScreenYScale = 2;
        };

        static App *instance = nullptr;

        ///////////////////////////////////////////////
        // Lua Modules
        ///////////////////////////////////////////////

#define DEFINE_LUA_FUNC(name) \
    static int name(lua_State *L)

        static int RegisterLuaModule(lua_State *L, const char *moduleName, const luaL_Reg functions[])
        {
            auto ns = luabridge::getGlobalNamespace(L)
                          .beginNamespace(PGELuaTableName)
                          .beginNamespace(moduleName);

            for (const luaL_Reg *reg = functions; reg->func; reg++)
                ns.addFunction(reg->name, reg->func);

            ns.endNamespace()
                .endNamespace();

            return 0;
        }

        ///////////////////////////////////////////////
        // Timer
        ///////////////////////////////////////////////

        DEFINE_LUA_FUNC(Timer_GetDeltaTime)
        {
            lua_pushnumber(L, instance->GetDeltaTime());
            return 1;
        }

        static const luaL_Reg TimerFunctions[] = {
            {"get_delta_time", Timer_GetDeltaTime},
            {NULL, NULL}};

        static int TimerRegisterFunctions(lua_State *L)
        {
            return RegisterLuaModule(L, "timer", TimerFunctions);
        }

        ///////////////////////////////////////////////
        // Graphics
        ///////////////////////////////////////////////

        static olc::Pixel GetPixelFromLuaStack(lua_State *L, int top)
        {
            uint8_t r = (uint8_t)lua_tointeger(L, top + 0);
            uint8_t g = (uint8_t)lua_tointeger(L, top + 1);
            uint8_t b = (uint8_t)lua_tointeger(L, top + 2);
            return {r, g, b};
        }

        DEFINE_LUA_FUNC(Graphics_Clear)
        {
            // TODO: Check arguments count

            auto pixel = GetPixelFromLuaStack(L, 1);
            instance->Clear(pixel);
            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_DrawLine)
        {
            // TODO: Check arguments count

            int32_t x1 = (int32_t)lua_tonumber(L, 1);
            int32_t y1 = (int32_t)lua_tonumber(L, 2);

            int32_t x2 = (int32_t)lua_tonumber(L, 3);
            int32_t y2 = (int32_t)lua_tonumber(L, 4);

            auto pixel = GetPixelFromLuaStack(L, 5);

            instance->DrawLine(x1, y1, x2, y2, pixel);

            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_FillRect)
        {
            // TODO: Check arguments count

            int32_t x = (int32_t)lua_tonumber(L, 1);
            int32_t y = (int32_t)lua_tonumber(L, 2);

            int32_t w = (int32_t)lua_tonumber(L, 3);
            int32_t h = (int32_t)lua_tonumber(L, 4);

            auto pixel = GetPixelFromLuaStack(L, 5);

            instance->FillRect(x, y, w, h, pixel);

            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_FillCircle)
        {
            // TODO: Check arguments count

            int32_t x = (int32_t)lua_tonumber(L, 1);
            int32_t y = (int32_t)lua_tonumber(L, 2);

            int32_t r = (int32_t)lua_tonumber(L, 3);

            auto pixel = GetPixelFromLuaStack(L, 4);

            instance->FillCircle(x, y, r, pixel);

            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_ScreenWidth)
        {
            lua_pushinteger(L, instance->GetScreenWidth());
            return 1;
        }

        DEFINE_LUA_FUNC(Graphics_ScreenHeight)
        {
            lua_pushinteger(L, instance->GetScreenHeight());
            return 1;
        }

        DEFINE_LUA_FUNC(Graphics_LoadSprite)
        {
            auto path = lua_tostring(L, 1);

            auto sprite = new olc::Sprite(std::string(path));
            assert(sprite);

            lua_pushlightuserdata(L, sprite);
            return 1;
        }

        DEFINE_LUA_FUNC(Graphics_UnloadSprite)
        {
            auto sprite = (olc::Sprite *)lua_topointer(L, 1);
            delete sprite;
            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_DrawSprite)
        {
            // TODO: Check arguments count

            int32_t x = (int32_t)lua_tonumber(L, 1);
            int32_t y = (int32_t)lua_tonumber(L, 2);

            auto sprite = (olc::Sprite *)lua_topointer(L, 3);
            assert(sprite);

            instance->DrawSprite(x, y, sprite);

            return 0;
        }

        static const luaL_Reg GraphicsFunctions[] = {
            {"clear", Graphics_Clear},

            {"draw_line", Graphics_DrawLine},
            {"fill_rect", Graphics_FillRect},
            {"fill_circle", Graphics_FillCircle},

            {"screen_width", Graphics_ScreenWidth},
            {"screen_height", Graphics_ScreenHeight},

            {"load_sprite", Graphics_LoadSprite},
            {"unload_sprite", Graphics_UnloadSprite},
            {"draw_sprite", Graphics_DrawSprite},

            {NULL, NULL}};

        static int GraphicsRegisterFunctions(lua_State *L)
        {
            return RegisterLuaModule(L, "graphics", GraphicsFunctions);
        }

        ///////////////////////////////////////////////
        // Input
        ///////////////////////////////////////////////

        DEFINE_LUA_FUNC(Input_IsKeyPressed)
        {
            olc::Key keycode = (olc::Key)lua_tointeger(L, 1);
            bool pressed = instance->GetKey(keycode).bPressed;
            lua_pushboolean(L, pressed);
            return 1;
        }

        DEFINE_LUA_FUNC(Input_IsKeyHeld)
        {
            olc::Key keycode = (olc::Key)lua_tointeger(L, 1);
            bool held = instance->GetKey(keycode).bHeld;
            lua_pushboolean(L, held);
            return 1;
        }

        DEFINE_LUA_FUNC(Input_IsKeyReleased)
        {
            olc::Key keycode = (olc::Key)lua_tointeger(L, 1);
            bool released = instance->GetKey(keycode).bReleased;
            lua_pushboolean(L, released);
            return 1;
        }

        DEFINE_LUA_FUNC(Input_IsMousePressed)
        {
            auto mouseId = lua_tointeger(L, 1);
            lua_pushboolean(L, instance->GetMouse(mouseId).bPressed);
            return 1;
        }

        DEFINE_LUA_FUNC(Input_IsMouseHeld)
        {
            auto mouseId = lua_tointeger(L, 1);
            lua_pushboolean(L, instance->GetMouse(mouseId).bHeld);
            return 1;
        }

        DEFINE_LUA_FUNC(Input_IsMouseReleased)
        {
            auto mouseId = lua_tointeger(L, 1);
            lua_pushboolean(L, instance->GetMouse(mouseId).bReleased);
            return 1;
        }

        DEFINE_LUA_FUNC(Input_GetMouseX)
        {
            lua_pushnumber(L, instance->GetMouseX());
            return 1;
        }

        DEFINE_LUA_FUNC(Input_GetMouseY)
        {
            lua_pushnumber(L, instance->GetMouseY());
            return 1;
        }

        DEFINE_LUA_FUNC(Input_GetMouseWheel)
        {
            lua_pushnumber(L, instance->GetMouseWheel());
            return 1;
        }

        static const luaL_Reg InputFunctions[] = {
            {"is_key_pressed", Input_IsKeyPressed},
            {"is_key_held", Input_IsKeyHeld},
            {"is_key_released", Input_IsKeyReleased},
            {"is_mouse_pressed", Input_IsMousePressed},
            {"is_mouse_held", Input_IsMouseHeld},
            {"is_mouse_released", Input_IsMouseReleased},
            {"mouse_x", Input_GetMouseX},
            {"mouse_y", Input_GetMouseY},
            {"mouse_wheel", Input_GetMouseWheel},
            {NULL, NULL}};

        static int InputRegisterFunctions(lua_State *L)
        {
            return RegisterLuaModule(L, "input", InputFunctions);
        }

#undef DEFINE_LUA_FUNC
    }

    bool Run()
    {
        _::instance = new _::App();
        auto app = _::instance;

        if (app->Construct(app->GetScreenWidth(),
                           app->GetScreenHeight(),
                           app->GetScreenXScale(),
                           app->GetScreenYScale()))
            app->Start();

        delete _::instance;
        _::instance = nullptr;
        return true;
    }
}