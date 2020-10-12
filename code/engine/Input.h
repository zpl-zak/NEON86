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
    CInput(VOID);
    VOID Release(VOID);

    auto GetKey(DWORD code) const -> BOOL { return mKeys[code]; }
    auto GetKeyDown(DWORD code) const -> BOOL { return mKeysDown[code]; }
    auto GetKeyUp(DWORD code) const -> BOOL { return mKeysUp[code]; }
    auto GetMouse(DWORD code) const -> BOOL { return mMouseInputs[code]; }
    auto GetMouseDown(DWORD code) const -> BOOL { return mMouseDown[code]; }
    auto GetMouseUp(DWORD code) const -> BOOL { return mMouseUp[code]; }
    auto GetLastKey(VOID) const -> BOOL { return mLastKey; }
    static auto GetMouseXY(VOID) -> POINT;
    auto GetMouseDelta(VOID) const -> POINT { return mMouseDelta; };

    VOID SetCursor(BOOL state) const;
    static auto GetCursor(VOID) -> BOOL;
    auto GetCursorMode() const -> UCHAR { return mCursorMode; }
    VOID SetCursorMode(UCHAR mode);

    VOID SetKey(DWORD code, BOOL state) { mKeys[code] = state; }
    VOID SetKeyDown(DWORD code, BOOL state) { mKeysDown[code] = state; }
    VOID SetKeyUp(DWORD code, BOOL state) { mKeysUp[code] = state; }

    VOID SetMouseButton(DWORD code, BOOL state)
    {
        mMouseInputs[code] = state;
    }

    VOID SetMouseDown(DWORD code, BOOL state) { mMouseDown[code] = state; }
    VOID SetMouseUp(DWORD code, BOOL state) { mMouseUp[code] = state; }
    VOID SetMouseXY(SHORT x, SHORT y);

    VOID ClearKey(VOID) { mLastKey = -1; }
    VOID Update(VOID);

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
    BOOL mKeys[NUM_KEYS], mKeysDown[NUM_KEYS], mKeysUp[NUM_KEYS];
    DWORD mLastKey;

    BOOL mMouseInputs[NUM_MOUSEBUTTONS], mMouseDown[NUM_MOUSEBUTTONS],
         mMouseUp[NUM_MOUSEBUTTONS];
    UCHAR mCursorMode;

    POINT mMouseDelta;
    POINT mLastMousePos;

    #ifdef _DEBUG
    BOOL mForceMouseCursor;
    #endif
};
