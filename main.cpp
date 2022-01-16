#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

class Example : public olc::PixelGameEngine
{
public:
    Example()
    {
        sAppName = "Example";
    }

public:
    bool OnUserCreate() override
    {
        // Called once at the start, so create things here
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        // called once per frame
        for (int x = 0; x < ScreenWidth(); x++)
            for (int y = 0; y < ScreenHeight(); y++)
                Draw(x, y, olc::Pixel(rand() % 255, rand() % 255, rand()% 255));	
        return true;
    }
};


int main()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    luaL_dostring(L, "print 'hello lua'");

    Example demo;
    if (demo.Construct(256, 240, 4, 4))
        demo.Start();

    lua_close(L);

    return 0;
}