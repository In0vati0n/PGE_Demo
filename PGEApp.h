#pragma once

#include <functional>
#include <iostream>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

namespace PGEApp
{
    const int MajorVersion = 0;
    const int MinorVersion = 1;
    const char *PGELuaTableName = "PGE";

    namespace _
    {
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

                std::cout << "[Call Lua Error]" << error << std::endl;

                lua_getglobal(L, "_pge_error");
                lua_pushstring(L, error);
                lua_pcall(L, 1, 0, 0);
            }

            return ret;
        }

        static int TimerRegisterFunctions(lua_State *L);

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
                fDeltaTime = fElapsedTime;
                CallLuaFunc(L, "_pge_update");
                return true;
            }

            inline float GetDeltaTime()
            {
                return fDeltaTime;
            }

        private:
            bool InitLuaTimer()
            {
                TimerRegisterFunctions(L);
            }

            bool InitLuaLibs()
            {
                InitLuaTimer();
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
            }

        private:
            lua_State *L;
            float fDeltaTime;
        };

        static App *instance = nullptr;

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

        ////////////////////////////////////////
        // Timer
        ///////////////////////////////////////
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

#undef DEFINE_LUA_FUNC
    }

    bool Run()
    {
        _::instance = new _::App();
        if (_::instance->Construct(256, 240, 4, 4))
            _::instance->Start();

        delete _::instance;
        _::instance = nullptr;
        return true;
    }
}