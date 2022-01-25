#pragma once

#include <cassert>
#include <functional>
#include <iostream>

#include "ray.h"

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

namespace PGEApp {
    static int MajorVersion = 0;
    static int MinorVersion = 1;
    const char *PGELuaTableName = "PGE";

    namespace _ {
        ///////////////////////////////////////////
        // Common Lua Utils
        ///////////////////////////////////////////
        static int Traceback(lua_State *L) {
            lua_getglobal(L, "debug");
            lua_getfield(L, -1, "traceback");
            lua_pushvalue(L, 1);
            lua_pushinteger(L, 2);
            lua_call(L, 2, 1);
            return 1;
        }

        static int CallLuaFunc(lua_State *L, const char *name) {
            lua_pushcfunction(L, Traceback);
            lua_getglobal(L, name);

            int ret = lua_pcall(L, 0, 0, lua_gettop(L) - 1);
            if (ret != 0) {
                const char *error = lua_tostring(L, -1);

                std::cout << "[Call Lua Error] " << error << std::endl;

                lua_getglobal(L, "_pge_error");
                lua_pushstring(L, error);
                lua_pcall(L, 1, 0, 0);
            }

            return ret;
        }

        /////////////////////////////////////////////////
        // PGEImplEngine
        /////////////////////////////////////////////////
        using namespace ray;

#define COLOR2PIXEL(c) \
    (*(olc::Pixel *)(&(c)))

        class Decal : public Drawable {
        public:
            Decal(Scope<olc::Decal> decal) : _decal(std::move(decal)) {}

            olc::Decal *Get() {
                return _decal.get();
            }

        private:
            Scope<olc::Decal> _decal;
        };

        class PGEGraphics final : public Graphics {
        public:
            PGEGraphics(std::shared_ptr<olc::PixelGameEngine> pge) : _pge(pge) {
            }

            void Clear(Color c) override {
                _pge->Clear(COLOR2PIXEL(c));
            }

            void Draw(i32 x, i32 y, Color c) override {
                _pge->Draw(x, y, COLOR2PIXEL(c));
            }

            void DrawLine(i32 x1, i32 y1, i32 x2, i32 y2, Color c) override {
                _pge->DrawLine(x1, y1, x2, y2, COLOR2PIXEL(c));
            }

            void DrawCircle(i32 x, i32 y, i32 r, Color c) override {
                _pge->DrawCircle(x, y, r, COLOR2PIXEL(c));
            }

            void FillCircle(i32 x, i32 y, i32 r, Color c) override {
                _pge->FillCircle(x, y, r, COLOR2PIXEL(c));
            }

            void DrawRect(i32 x, i32 y, i32 w, i32 h, Color c) override {
                _pge->DrawRect(x, y, w, h, COLOR2PIXEL(c));
            }

            void FillRect(i32 x, i32 y, i32 w, i32 h, Color c) override {
                _pge->FillRect(x, y, w, h, COLOR2PIXEL(c));
            }

            void DrawTriangle(i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3, Color c) override {
                _pge->DrawTriangle(x1, y1, x2, y2, x3, y3, COLOR2PIXEL(c));
            }

            void FillTriangle(i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3, Color c) override {
                _pge->FillTriangle(x1, y1, x2, y2, x3, y3, COLOR2PIXEL(c));
            }

            void Draw(Drawable *drawable) override {
                //TODO!
            }

            void DrawDecal(Decal *d) {
            }

        private:
            std::shared_ptr<olc::PixelGameEngine> _pge;
        };

        /////////////////////////////////////////////////
        // LuaEngine
        /////////////////////////////////////////////////
        static int TimerRegisterFunctions(lua_State *L);

        static int WindowRegisterFunctions(lua_State *L);

        static int GraphicsRegisterFunctions(lua_State *L);

        static int InputRegisterFunctions(lua_State *L);

        class App : public olc::PixelGameEngine {
        public:
            App() {
                sAppName = "App";

                L = luaL_newstate();
                InitLua();
            }

            virtual ~App() {
                lua_close(L);
                L = nullptr;
            }

        public:
            bool OnUserCreate() override {
                CallLuaFunc(L, "_pge_load");
                return true;
            }

            bool OnUserUpdate(float fElapsedTime) override {
                DeltaTime = fElapsedTime;
                CallLuaFunc(L, "_pge_update");

                return true;
            }

            bool OnUserDestroy() override {
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
            bool InitTimerModule() {
                TimerRegisterFunctions(L);
                return true;
            }

            bool InitWindowModule() {
                WindowRegisterFunctions(L);
                return true;
            }

            bool InitGraphicsModule() {
                GraphicsRegisterFunctions(L);

                luabridge::getGlobalNamespace(L)
                        .beginNamespace(PGELuaTableName)
                        .beginNamespace("graphics")
                        .endNamespace()
                        .endNamespace();

                lua_getglobal(L, PGELuaTableName);
                lua_getfield(L, -1, "graphics");

                lua_pushstring(L, "PixelMode");
                lua_newtable(L);
                lua_rawset(L, -3);

                lua_getfield(L, -1, "PixelMode");

                lua_pushinteger(L, olc::Pixel::NORMAL);
                lua_setfield(L, -2, "Normal");

                lua_pushinteger(L, olc::Pixel::MASK);
                lua_setfield(L, -2, "Mask");

                lua_pushinteger(L, olc::Pixel::ALPHA);
                lua_setfield(L, -2, "Alpha");

                lua_pushinteger(L, olc::Pixel::CUSTOM);
                lua_setfield(L, -2, "Custom");

                return true;
            }

            bool InitInputModule() {
                InputRegisterFunctions(L);

                luabridge::getGlobalNamespace(L)
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

            bool InitModules() {
                InitTimerModule();
                InitWindowModule();
                InitGraphicsModule();
                InitInputModule();
                return true;
            }

            bool InitConfig() {
                auto luaConfigFunc = luabridge::getGlobal(L, "_pge_config");
                auto config = luaConfigFunc()[0];

                luabridge::getGlobalNamespace(L)
                        .beginNamespace(PGELuaTableName)
                        .addProperty("config", &config, false)
                        .endNamespace();

                sAppName = std::string((const char *) config["title"]);

                ScreenWidth = (int) config["screen_width"];
                ScreenHeight = (int) config["screen_height"];
                ScreenXScale = (int) config["screen_x_scale"];
                ScreenYScale = (int) config["screen_y_scale"];

                return true;
            }

            bool InitLua() {
                luaL_openlibs(L);

                luabridge::getGlobalNamespace(L)
                        .beginNamespace(PGELuaTableName)
                        .addProperty("MajorVersion", &MajorVersion, false)
                        .addProperty("MinorVersion", &MinorVersion, false)
                        .endNamespace();

                // init lua libs
                InitModules();

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

        static int RegisterLuaModule(lua_State *L, const char *moduleName, const luaL_Reg functions[]) {
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

        DEFINE_LUA_FUNC(Timer_GetDeltaTime) {
            lua_pushnumber(L, instance->GetDeltaTime());
            return 1;
        }

        static const luaL_Reg TimerFunctions[] = {
                {"get_delta_time", Timer_GetDeltaTime},
                {NULL, NULL}};

        static int TimerRegisterFunctions(lua_State *L) {
            return RegisterLuaModule(L, "timer", TimerFunctions);
        }

        ///////////////////////////////////////////////
        // Window
        ///////////////////////////////////////////////

        DEFINE_LUA_FUNC(Window_ScreenWidth) {
            lua_pushinteger(L, instance->GetScreenWidth());
            return 1;
        }

        DEFINE_LUA_FUNC(Window_ScreenHeight) {
            lua_pushinteger(L, instance->GetScreenHeight());
            return 1;
        }

        DEFINE_LUA_FUNC(Window_IsFocused) {
            lua_pushboolean(L, instance->IsFocused());
            return 1;
        }

        static const luaL_Reg WindowFunctions[] = {
                {"screen_width",  Window_ScreenWidth},
                {"screen_height", Window_ScreenHeight},
                {"is_focus",      Window_IsFocused},
                {nullptr,         nullptr}};

        static int WindowRegisterFunctions(lua_State *L) {
            return RegisterLuaModule(L, "window", WindowFunctions);
        }

        ///////////////////////////////////////////////
        // Graphics
        ///////////////////////////////////////////////

        static olc::Pixel GetPixelFromLuaStack(lua_State *L, int top) {
            auto r = (uint8_t) lua_tointeger(L, top + 0);
            auto g = (uint8_t) lua_tointeger(L, top + 1);
            auto b = (uint8_t) lua_tointeger(L, top + 2);

            uint8_t a = 255;
            if (lua_gettop(L) >= top + 3)
                a = (uint8_t) lua_tointeger(L, top + 3);

            return {r, g, b, a};
        }

        DEFINE_LUA_FUNC(Graphics_SetDrawTarget) {
            auto sprite = (olc::Sprite *) lua_topointer(L, 1);
            assert(sprite);

            instance->SetDrawTarget(sprite);

            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_GetDrawTargetWidth) {
            auto width = instance->GetDrawTargetWidth();
            lua_pushinteger(L, (lua_Integer) width);
            return 1;
        }

        DEFINE_LUA_FUNC(Graphics_GetDrawTargetHeight) {
            auto height = instance->GetDrawTargetHeight();
            lua_pushinteger(L, (lua_Integer) height);
            return 1;
        }

        DEFINE_LUA_FUNC(Graphics_GetDrawTarget) {
            auto sprite = instance->GetDrawTarget();
            lua_pushlightuserdata(L, sprite);
            return 1;
        }

        DEFINE_LUA_FUNC(Graphics_Clear) {
            // TODO: Check arguments count

            auto pixel = GetPixelFromLuaStack(L, 1);
            instance->Clear(pixel);
            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_Draw) {
            // TODO: Check arguments count

            int32_t x = (int32_t) lua_tonumber(L, 1);
            int32_t y = (int32_t) lua_tonumber(L, 2);

            auto pixel = GetPixelFromLuaStack(L, 3);

            instance->Draw(x, y, pixel);

            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_DrawLine) {
            // TODO: Check arguments count

            int32_t x1 = (int32_t) lua_tonumber(L, 1);
            int32_t y1 = (int32_t) lua_tonumber(L, 2);

            int32_t x2 = (int32_t) lua_tonumber(L, 3);
            int32_t y2 = (int32_t) lua_tonumber(L, 4);

            auto pixel = GetPixelFromLuaStack(L, 5);

            instance->DrawLine(x1, y1, x2, y2, pixel);

            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_DrawCircle) {
            // TODO: Check arguments count

            int32_t x = (int32_t) lua_tonumber(L, 1);
            int32_t y = (int32_t) lua_tonumber(L, 2);

            int32_t r = (int32_t) lua_tonumber(L, 3);

            auto pixel = GetPixelFromLuaStack(L, 4);

            instance->DrawCircle(x, y, r, pixel);

            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_FillCircle) {
            // TODO: Check arguments count

            int32_t x = (int32_t) lua_tonumber(L, 1);
            int32_t y = (int32_t) lua_tonumber(L, 2);

            int32_t r = (int32_t) lua_tonumber(L, 3);

            auto pixel = GetPixelFromLuaStack(L, 4);

            instance->FillCircle(x, y, r, pixel);

            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_DrawRect) {
            // TODO: Check arguments count

            int32_t x = (int32_t) lua_tonumber(L, 1);
            int32_t y = (int32_t) lua_tonumber(L, 2);

            int32_t w = (int32_t) lua_tonumber(L, 3);
            int32_t h = (int32_t) lua_tonumber(L, 4);

            auto pixel = GetPixelFromLuaStack(L, 5);

            instance->DrawRect(x, y, w, h, pixel);

            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_FillRect) {
            // TODO: Check arguments count

            int32_t x = (int32_t) lua_tonumber(L, 1);
            int32_t y = (int32_t) lua_tonumber(L, 2);

            int32_t w = (int32_t) lua_tonumber(L, 3);
            int32_t h = (int32_t) lua_tonumber(L, 4);

            auto pixel = GetPixelFromLuaStack(L, 5);

            instance->FillRect(x, y, w, h, pixel);

            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_DrawTriangle) {
            // TODO: Check arguments count

            int32_t x1 = (int32_t) lua_tonumber(L, 1);
            int32_t y1 = (int32_t) lua_tonumber(L, 2);

            int32_t x2 = (int32_t) lua_tonumber(L, 3);
            int32_t y2 = (int32_t) lua_tonumber(L, 4);

            int32_t x3 = (int32_t) lua_tonumber(L, 5);
            int32_t y3 = (int32_t) lua_tonumber(L, 6);

            auto pixel = GetPixelFromLuaStack(L, 7);

            instance->DrawTriangle(x1, y1, x2, y2, x3, y3, pixel);

            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_FillTriangle) {
            // TODO: Check arguments count

            int32_t x1 = (int32_t) lua_tonumber(L, 1);
            int32_t y1 = (int32_t) lua_tonumber(L, 2);

            int32_t x2 = (int32_t) lua_tonumber(L, 3);
            int32_t y2 = (int32_t) lua_tonumber(L, 4);

            int32_t x3 = (int32_t) lua_tonumber(L, 5);
            int32_t y3 = (int32_t) lua_tonumber(L, 6);

            auto pixel = GetPixelFromLuaStack(L, 7);

            instance->FillTriangle(x1, y1, x2, y2, x3, y3, pixel);

            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_LoadSprite) {
            auto path = lua_tostring(L, 1);

            auto sprite = new olc::Sprite(std::string(path));
            assert(sprite);

            lua_pushlightuserdata(L, sprite);
            return 1;
        }

        DEFINE_LUA_FUNC(Graphics_UnloadSprite) {
            auto sprite = (olc::Sprite *) lua_topointer(L, 1);
            assert(sprite);
            delete sprite;
            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_CreateDecal) {
            auto sprite = (olc::Sprite *) lua_topointer(L, 1);
            assert(sprite);

            auto decal = new olc::Decal(sprite);
            assert(decal);

            lua_pushlightuserdata(L, decal);

            return 1;
        }

        DEFINE_LUA_FUNC(Graphics_DestroyDecal) {
            auto decal = (olc::Decal *) lua_topointer(L, 1);
            assert(decal);
            delete decal;
            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_DrawSprite) {
            // TODO: Check arguments count

            int32_t x = (int32_t) lua_tonumber(L, 1);
            int32_t y = (int32_t) lua_tonumber(L, 2);

            auto sprite = (olc::Sprite *) lua_topointer(L, 3);
            assert(sprite);

            uint32_t scale = 1;
            if (lua_gettop(L) >= 4)
                scale = (uint32_t) lua_tointeger(L, 4);

            instance->DrawSprite(x, y, sprite, scale);

            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_DrawPartialSprite) {
            // TODO: Check arguments count

            int32_t x = (int32_t) lua_tonumber(L, 1);
            int32_t y = (int32_t) lua_tonumber(L, 2);

            auto sprite = (olc::Sprite *) lua_topointer(L, 3);
            assert(sprite);

            int32_t xOffset = (int32_t) lua_tonumber(L, 4);
            int32_t yOffset = (int32_t) lua_tonumber(L, 5);

            int32_t width = (int32_t) lua_tonumber(L, 6);
            int32_t height = (int32_t) lua_tonumber(L, 7);

            uint32_t scale = 1;
            if (lua_gettop(L) >= 8)
                scale = (uint32_t) lua_tointeger(L, 8);

            instance->DrawPartialSprite(x, y, sprite, xOffset, yOffset, width, height, scale);

            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_DrawString) {
            // TODO: Check arguments count

            int32_t x = (int32_t) lua_tonumber(L, 1);
            int32_t y = (int32_t) lua_tonumber(L, 2);

            auto text = std::string(lua_tostring(L, 3));
            auto pixel = GetPixelFromLuaStack(L, 4);

            uint32_t scale = 1;
            if (lua_gettop(L) >= 5)
                scale = (uint32_t) lua_tointeger(L, 5);

            instance->DrawString(x, y, text, pixel, scale);

            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_DrawDecal) {
            // TODO: Check arguments count

            float x = (float) lua_tonumber(L, 1);
            float y = (float) lua_tonumber(L, 2);

            auto decal = (olc::Decal *) lua_topointer(L, 3);
            assert(decal);

            instance->DrawDecal({x, y}, decal);

            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_DrawRotatedDecal) {
            // TODO: Check arguments count

            float x = (float) lua_tonumber(L, 1);
            float y = (float) lua_tonumber(L, 2);

            auto decal = (olc::Decal *) lua_topointer(L, 3);
            // assert(decal);

            float angle = (float) lua_tonumber(L, 4);

            float xCenter = (float) lua_tonumber(L, 5);
            float yCenter = (float) lua_tonumber(L, 6);

            float xScale = (float) lua_tonumber(L, 7);
            float yScale = (float) lua_tonumber(L, 8);

            auto tint = GetPixelFromLuaStack(L, 9);

            instance->DrawRotatedDecal({x, y}, decal, angle, {xCenter, yCenter}, {xScale, yScale}, tint);

            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_SetPixelBlend) {
            float blend = (float) lua_tonumber(L, 1);
            instance->SetPixelBlend(blend);

            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_SetPixelMode) {
            olc::Pixel::Mode mode = (olc::Pixel::Mode) lua_tointeger(L, 1);
            instance->SetPixelMode(mode);

            return 0;
        }

        DEFINE_LUA_FUNC(Graphics_GetPixelMode) {
            auto mode = instance->GetPixelMode();

            lua_pushinteger(L, (lua_Integer) mode);

            return 1;
        }

        static const luaL_Reg GraphicsFunctions[] = {
                {"set_draw_target",        Graphics_SetDrawTarget},
                {"get_draw_target_width",  Graphics_GetDrawTargetWidth},
                {"get_draw_target_height", Graphics_GetDrawTargetHeight},
                {"get_draw_target",        Graphics_GetDrawTarget},

                {"set_pixel_blend",        Graphics_SetPixelBlend},
                {"set_pixel_mode",         Graphics_SetPixelMode},
                {"get_pixel_mode",         Graphics_GetPixelMode},

                {"clear",                  Graphics_Clear},

                {"draw",                   Graphics_Draw},
                {"draw_line",              Graphics_DrawLine},
                {"draw_circle",            Graphics_DrawCircle},
                {"fill_circle",            Graphics_FillCircle},
                {"draw_rect",              Graphics_DrawRect},
                {"fill_rect",              Graphics_FillRect},
                {"draw_triangle",          Graphics_DrawTriangle},
                {"fill_triangle",          Graphics_FillTriangle},

                {"load_sprite",            Graphics_LoadSprite},
                {"unload_sprite",          Graphics_UnloadSprite},
                {"create_decal",           Graphics_CreateDecal},
                {"destroy_decal",          Graphics_DestroyDecal},

                {"draw_sprite",            Graphics_DrawSprite},
                {"draw_partial_sprite",    Graphics_DrawPartialSprite},
                {"draw_decal",             Graphics_DrawDecal},
                {"draw_rotated_decal",     Graphics_DrawRotatedDecal},

                {"draw_string",            Graphics_DrawString},

                {NULL, NULL}};

        static int GraphicsRegisterFunctions(lua_State *L) {
            return RegisterLuaModule(L, "graphics", GraphicsFunctions);
        }

        ///////////////////////////////////////////////
        // Input
        ///////////////////////////////////////////////

        DEFINE_LUA_FUNC(Input_IsKeyPressed) {
            olc::Key keycode = (olc::Key) lua_tointeger(L, 1);
            bool pressed = instance->GetKey(keycode).bPressed;
            lua_pushboolean(L, pressed);
            return 1;
        }

        DEFINE_LUA_FUNC(Input_IsKeyHeld) {
            olc::Key keycode = (olc::Key) lua_tointeger(L, 1);
            bool held = instance->GetKey(keycode).bHeld;
            lua_pushboolean(L, held);
            return 1;
        }

        DEFINE_LUA_FUNC(Input_IsKeyReleased) {
            olc::Key keycode = (olc::Key) lua_tointeger(L, 1);
            bool released = instance->GetKey(keycode).bReleased;
            lua_pushboolean(L, released);
            return 1;
        }

        DEFINE_LUA_FUNC(Input_IsMousePressed) {
            auto mouseId = (uint32_t) lua_tointeger(L, 1);
            lua_pushboolean(L, instance->GetMouse(mouseId).bPressed);
            return 1;
        }

        DEFINE_LUA_FUNC(Input_IsMouseHeld) {
            auto mouseId = (uint32_t) lua_tointeger(L, 1);
            lua_pushboolean(L, instance->GetMouse(mouseId).bHeld);
            return 1;
        }

        DEFINE_LUA_FUNC(Input_IsMouseReleased) {
            auto mouseId = lua_tointeger(L, 1);
            lua_pushboolean(L, instance->GetMouse(mouseId).bReleased);
            return 1;
        }

        DEFINE_LUA_FUNC(Input_GetMouseX) {
            lua_pushnumber(L, instance->GetMouseX());
            return 1;
        }

        DEFINE_LUA_FUNC(Input_GetMouseY) {
            lua_pushnumber(L, instance->GetMouseY());
            return 1;
        }

        DEFINE_LUA_FUNC(Input_GetMouseWheel) {
            lua_pushnumber(L, instance->GetMouseWheel());
            return 1;
        }

        static const luaL_Reg InputFunctions[] = {
                {"is_key_pressed",    Input_IsKeyPressed},
                {"is_key_held",       Input_IsKeyHeld},
                {"is_key_released",   Input_IsKeyReleased},
                {"is_mouse_pressed",  Input_IsMousePressed},
                {"is_mouse_held",     Input_IsMouseHeld},
                {"is_mouse_released", Input_IsMouseReleased},
                {"mouse_x",           Input_GetMouseX},
                {"mouse_y",           Input_GetMouseY},
                {"mouse_wheel",       Input_GetMouseWheel},
                {NULL, NULL}};

        static int InputRegisterFunctions(lua_State *L) {
            return RegisterLuaModule(L, "input", InputFunctions);
        }

#undef DEFINE_LUA_FUNC
    }

    bool Run() {
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