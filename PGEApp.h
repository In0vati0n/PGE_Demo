#pragma once

#include <functional>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

namespace PGEApp
{
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
            return true;
        }

        bool OnUserUpdate(float fElapsedTime) override
        {
            return true;
        }

    private:
        bool InitLua()
        {
            luaL_openlibs(L);
            luaL_dostring(L, "print 'hello lua'");
        }

    private:
        lua_State* L;
    };

    bool run()
    {
        App app;
        if (app.Construct(256, 240, 4, 4))
            app.Start();

        return true;
    }
}