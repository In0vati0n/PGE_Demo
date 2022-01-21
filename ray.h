#pragma once

namespace ray {
    ///////////////////////////////////////////
    // Common Type Define
    ///////////////////////////////////////////
    using u8 = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using i8 = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;

    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args &&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Ref = std::shared_ptr<T>;

    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args &&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    /////////////////////////////////////////////////
    // Base Struct
    /////////////////////////////////////////////////
    enum KeyCode {
        NONE,
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        K0,
        K1,
        K2,
        K3,
        K4,
        K5,
        K6,
        K7,
        K8,
        K9,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        UP,
        DOWN,
        LEFT,
        RIGHT,
        SPACE,
        TAB,
        SHIFT,
        CTRL,
        INS,
        DEL,
        HOME,
        END,
        PGUP,
        PGDN,
        BACK,
        ESCAPE,
        RETURN,
        ENTER,
        PAUSE,
        SCROLL,
        NP0,
        NP1,
        NP2,
        NP3,
        NP4,
        NP5,
        NP6,
        NP7,
        NP8,
        NP9,
        NP_MUL,
        NP_DIV,
        NP_ADD,
        NP_SUB,
        NP_DECIMAL,
        PERIOD,
        EQUALS,
        COMMA,
        MINUS,
        OEM_1,
        OEM_2,
        OEM_3,
        OEM_4,
        OEM_5,
        OEM_6,
        OEM_7,
        OEM_8,
        CAPS_LOCK,
        ENUM_END
    };

    struct Color {
        union {
            u32 _n{};
            struct {
                u8 r, g, b, a;
            };
        };

        Color() : Color(0, 0, 0, 0) {}

        Color(u8 r, u8 g, u8 b, u8 a) : r(r), g(g), b(b), a(a) {}

        Color &operator=(const Color &c) = default;

        inline bool operator==(const Color &c) const {
            return _n == c._n;
        }

        inline bool operator!=(const Color &c) const {
            return _n != c._n;
        }

        inline Color operator*(const float i) const {
            float fR = std::min(255.0f, std::max(0.0f, float(r) * i));
            float fG = std::min(255.0f, std::max(0.0f, float(g) * i));
            float fB = std::min(255.0f, std::max(0.0f, float(b) * i));
            return {uint8_t(fR), uint8_t(fG), uint8_t(fB), a};
        }

        inline Color operator/(const float i) const {
            float fR = std::min(255.0f, std::max(0.0f, float(r) / i));
            float fG = std::min(255.0f, std::max(0.0f, float(g) / i));
            float fB = std::min(255.0f, std::max(0.0f, float(b) / i));
            return {uint8_t(fR), uint8_t(fG), uint8_t(fB), a};
        }

        inline Color &operator*=(const float i) {
            this->r = uint8_t(std::min(255.0f, std::max(0.0f, float(r) * i)));
            this->g = uint8_t(std::min(255.0f, std::max(0.0f, float(g) * i)));
            this->b = uint8_t(std::min(255.0f, std::max(0.0f, float(b) * i)));
            return *this;
        }

        inline Color &operator/=(const float i) {
            this->r = uint8_t(std::min(255.0f, std::max(0.0f, float(r) / i)));
            this->g = uint8_t(std::min(255.0f, std::max(0.0f, float(g) / i)));
            this->b = uint8_t(std::min(255.0f, std::max(0.0f, float(b) / i)));
            return *this;
        }

        inline Color operator+(const Color &p) const {
            uint8_t nR = uint8_t(std::min(255, std::max(0, int(r) + int(p.r))));
            uint8_t nG = uint8_t(std::min(255, std::max(0, int(g) + int(p.g))));
            uint8_t nB = uint8_t(std::min(255, std::max(0, int(b) + int(p.b))));
            return {nR, nG, nB, a};
        }

        inline Color operator-(const Color &p) const {
            uint8_t nR = uint8_t(std::min(255, std::max(0, int(r) - int(p.r))));
            uint8_t nG = uint8_t(std::min(255, std::max(0, int(g) - int(p.g))));
            uint8_t nB = uint8_t(std::min(255, std::max(0, int(b) - int(p.b))));
            return {nR, nG, nB, a};
        }

        inline Color &operator+=(const Color &p) {
            this->r = uint8_t(std::min(255, std::max(0, int(r) + int(p.r))));
            this->g = uint8_t(std::min(255, std::max(0, int(g) + int(p.g))));
            this->b = uint8_t(std::min(255, std::max(0, int(b) + int(p.b))));
            return *this;
        }

        inline Color &operator-=(const Color &p) {
            this->r = uint8_t(std::min(255, std::max(0, int(r) - int(p.r))));
            this->g = uint8_t(std::min(255, std::max(0, int(g) - int(p.g))));
            this->b = uint8_t(std::min(255, std::max(0, int(b) - int(p.b))));
            return *this;
        }
    };

    // forward declaration
    class Drawable;

    class Graphics {
    public:
        Graphics() = default;

        virtual ~Graphics() = default;

    public:
        virtual void Clear(Color c) = 0;

        virtual void Draw(i32 x, i32 y, Color c) = 0;

        virtual void DrawLine(i32 x1, i32 y1, i32 x2, i32 y2, Color c) = 0;

        virtual void DrawCircle(i32 x, i32 y, i32 r, Color c) = 0;

        virtual void FillCircle(i32 x, i32 y, i32 r, Color c) = 0;

        virtual void DrawRect(i32 x, i32 y, i32 w, i32 h, Color c) = 0;

        virtual void FillRect(i32 x, i32 y, i32 w, i32 h, Color c) = 0;

        virtual void DrawTriangle(i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3, Color c) = 0;

        virtual void FillTriangle(i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3, Color c) = 0;

        virtual void Draw(Drawable *drawable) = 0;
    };

    class Drawable {
    public:
        Drawable() = default;

        virtual ~Drawable() = default;

        virtual void Draw(Graphics *gfx) = 0;
    };

    class Sprite : public Drawable {
    };

    /////////////////////////////////////////////////
    // Base App
    /////////////////////////////////////////////////
    class Engine {
    public:
        Engine() = default;

        virtual ~Engine() = default;

        bool Init() {
            _gfx = OnCreateGfx();
            return OnInit();
        }

        bool Update(float dt) {
            return OnUpdate(dt);
        }

        bool Destroy() {
            return OnDestroy();
        }

    public:
        /////////////////////////////////////////////////
        // Window
        /////////////////////////////////////////////////
        virtual int GetScreenWidth() = 0;

        virtual int GetScreenHeight() = 0;

        virtual int GetScreenXScale() = 0;

        virtual int GetScreenYScale() = 0;

        virtual bool IsFocus() = 0;

        /////////////////////////////////////////////////
        // Draw
        /////////////////////////////////////////////////
        void Clear(Color c) {
            _gfx->Clear(c);
        }

        void Draw(i32 x, i32 y, Color c) {
            _gfx->Draw(x, y, c);
        }

        void DrawLine(i32 x1, i32 y1, i32 x2, i32 y2, Color c) {
            _gfx->DrawLine(x1, y1, x2, y2, c);
        }

        void DrawCircle(i32 x, i32 y, i32 r, Color c) {
            _gfx->DrawCircle(x, y, r, c);
        }

        void FillCircle(i32 x, i32 y, i32 r, Color c) {
            _gfx->FillCircle(x, y, r, c);
        }

        void DrawRect(i32 x, i32 y, i32 w, i32 h, Color c) {
            _gfx->DrawRect(x, y, w, h, c);
        }

        void FillRect(i32 x, i32 y, i32 w, i32 h, Color c) {
            _gfx->FillRect(x, y, w, h, c);
        }

        void DrawTriangle(i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3, Color c) {
            _gfx->DrawTriangle(x1, y1, x2, y2, x3, y3, c);
        }

        void FillTriangle(i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3, Color c) {
            _gfx->FillTriangle(x1, y1, x2, y2, x3, y3, c);
        }

        void Draw(Drawable *drawable) {
            _gfx->Draw(drawable);
        }

        /////////////////////////////////////////////////
        // Input
        /////////////////////////////////////////////////
        virtual bool IsKeyPressed(KeyCode code) = 0;

        virtual bool IsKeyHeld(KeyCode code) = 0;

        virtual bool IsKeyReleased(KeyCode code) = 0;

        virtual bool IsMousePressed(int button) = 0;

        virtual bool IsMouseHeld(int button) = 0;

        virtual bool IsMouseReleased(int button) = 0;

        virtual float GetMouseX() = 0;

        virtual float GetMouseY() = 0;

        virtual float GetMouseWheel() = 0;

    protected:
        virtual bool OnInit() = 0;

        virtual std::unique_ptr<Graphics> OnCreateGfx() = 0;

        virtual bool OnUpdate(float dt) = 0;

        virtual bool OnDestroy() = 0;

    protected:
        std::unique_ptr<Graphics> _gfx;
    };
}