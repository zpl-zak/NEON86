#pragma once

#include "system.h"

#define INPUT CEngine::the()->GetInput()

enum
{
    CURSORMODE_DEFAULT,
    CURSORMODE_CENTERED,
    CURSORMODE_WRAPPED,
};

class ENGINE_API CInput
{
public:
    CInput(void);
    void Release(void);

    auto GetKey(DWORD code) const -> bool { return mKeys[code]; }
    auto GetKeyDown(DWORD code) const -> bool { return mKeysDown[code]; }
    auto GetKeyUp(DWORD code) const -> bool { return mKeysUp[code]; }
    auto GetMouse(DWORD code) const -> bool { return mMouseInputs[code]; }
    auto GetMouseDown(DWORD code) const -> bool { return mMouseDown[code]; }
    auto GetMouseUp(DWORD code) const -> bool { return mMouseUp[code]; }
    auto GetLastKey(void) const -> bool { return mLastKey; }
    static auto GetMouseXY(void) -> POINT;
    auto GetMouseDelta(void) const -> POINT { return mMouseDelta; };

    void SetCursor(bool state) const;
    static auto GetCursor(void) -> bool;
    auto GetCursorMode() const -> UCHAR { return mCursorMode; }
    void SetCursorMode(UCHAR mode);

    void SetKey(DWORD code, bool state) { mKeys[code] = state; }
    void SetKeyDown(DWORD code, bool state) { mKeysDown[code] = state; }
    void SetKeyUp(DWORD code, bool state) { mKeysUp[code] = state; }

    void SetMouseButton(DWORD code, bool state)
    {
        mMouseInputs[code] = state;
    }

    void SetMouseDown(DWORD code, bool state) { mMouseDown[code] = state; }
    void SetMouseUp(DWORD code, bool state) { mMouseUp[code] = state; }
    void SetMouseXY(short x, short y);

    void ClearKey(void) { mLastKey = -1; }
    void Update(void);

    enum
    {
        MOUSE_LEFT_BUTTON = 1,
        MOUSE_MIDDLE_BUTTON = 2,
        MOUSE_RIGHT_BUTTON = 3,
        MOUSE_WHEEL_UP = 4,
        MOUSE_WHEEL_DOWN = 5
    };

    static const int NUM_KEYS = 512;
    static const int NUM_MOUSEBUTTONS = 256;

private:
    bool mKeys[NUM_KEYS], mKeysDown[NUM_KEYS], mKeysUp[NUM_KEYS];
    DWORD mLastKey;

    bool mMouseInputs[NUM_MOUSEBUTTONS], mMouseDown[NUM_MOUSEBUTTONS],
         mMouseUp[NUM_MOUSEBUTTONS];
    UCHAR mCursorMode;

    POINT mMouseDelta;
    POINT mLastMousePos;

    #ifdef _DEBUG
    bool mForceMouseCursor;
    #endif
};
