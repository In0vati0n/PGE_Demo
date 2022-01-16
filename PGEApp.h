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

namespace PGEApp
{
    const int MajorVersion = 0;
    const int MinorVersion = 1;
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
                DeltaTime = fElapsedTime;
                CallLuaFunc(L, "_pge_update");
                return true;
            }

            bool OnUserDestroy() override
            {
                CallLuaFunc(L, "_pge_on_destroy");
                return true;
            }

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

            bool InitLuaLibs()
            {
                InitLuaTimer();
                InitLuaGraphics();
                return true;
            }

            bool InitConfig()
            {
                assert(lua_getglobal(L, "_pge_config"));
                lua_pop(L, 1);
                CallLuaFunc(L, "_pge_config");
                lua_getglobal(L, PGELuaTableName);
                lua_getfield(L, -1, "config");

                ScreenWidth = LuaGetTableIntField(L, "screen_width");
                ScreenHeight = LuaGetTableIntField(L, "screen_height");
                ScreenXScale = LuaGetTableIntField(L, "screen_x_scale");
                ScreenYScale = LuaGetTableIntField(L, "screen_y_scale");

                lua_pop(L, 2);
                return true;
            }

            bool InitLua()
            {
                luaL_openlibs(L);

                // create PGE table
                lua_newtable(L);

                lua_pushinteger(L, MajorVersion);
                lua_setfield(L, -2, "MajorVersion");

                lua_pushinteger(L, MinorVersion);
                lua_setfield(L, -2, "MinorVersion");

                lua_setglobal(L, PGELuaTableName);

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
            lua_newtable(L);
            for (const luaL_Reg *reg = functions; reg->func; reg++)
            {
                lua_pushcfunction(L, reg->func);
                lua_setfield(L, -2, reg->name);
            }

            lua_getglobal(L, PGELuaTableName);
            lua_pushvalue(L, -2);
            lua_setfield(L, -2, moduleName);
            lua_pop(L, 2);

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

            int32_t x1 = (int32_t)lua_tointeger(L, 1);
            int32_t y1 = (int32_t)lua_tointeger(L, 2);

            int32_t x2 = (int32_t)lua_tointeger(L, 3);
            int32_t y2 = (int32_t)lua_tointeger(L, 4);

            auto pixel = GetPixelFromLuaStack(L, 5);

            instance->DrawLine(x1, y1, x2, y2, pixel);

            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_FillRect)
        {
            // TODO: Check arguments count

            int32_t x = (int32_t)lua_tointeger(L, 1);
            int32_t y = (int32_t)lua_tointeger(L, 2);

            int32_t w = (int32_t)lua_tointeger(L, 3);
            int32_t h = (int32_t)lua_tointeger(L, 4);

            auto pixel = GetPixelFromLuaStack(L, 5);

            instance->FillRect(x, y, w, h, pixel);

            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_FillCircle)
        {
            // TODO: Check arguments count

            int32_t x = (int32_t)lua_tointeger(L, 1);
            int32_t y = (int32_t)lua_tointeger(L, 2);

            int32_t r = (int32_t)lua_tointeger(L, 3);

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

        static const luaL_Reg GraphicsFunctions[] = {
            {"clear", Graphics_Clear},
            {"draw_line", Graphics_DrawLine},
            {"fill_rect", Graphics_FillRect},
            {"fill_circle", Graphics_FillCircle},
            {"screen_width", Graphics_ScreenWidth},
            {"screen_height", Graphics_ScreenHeight},
            {NULL, NULL}};

        static int GraphicsRegisterFunctions(lua_State *L)
        {
            return RegisterLuaModule(L, "graphics", GraphicsFunctions);
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