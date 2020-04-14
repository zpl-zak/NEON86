#pragma once

#include "system.h"

#define INPUT CEngine::the()->GetInput()

enum {
	CURSORMODE_DEFAULT,
	CURSORMODE_CENTERED,
	CURSORMODE_WRAPPED, // todo
};

class ENGINE_API CInput
{
public:
	CInput(void);
	VOID Release(void);

	inline BOOL GetKey(DWORD code) const { return mKeys[code]; }
	inline BOOL GetKeyDown(DWORD code) const { return mKeysDown[code]; }
	inline BOOL GetKeyUp(DWORD code) const { return mKeysUp[code]; }
	inline BOOL GetMouse(DWORD code) const { return mMouseInputs[code]; }
	inline BOOL GetMouseDown(DWORD code) const { return mMouseDown[code]; }
	inline BOOL GetMouseUp(DWORD code) const { return mMouseUp[code]; }
	inline BOOL GetLastKey(void) const { return mLastKey; }
	POINT GetMouseXY(void) const;
	inline POINT GetMouseDelta(void) const { return mMouseDelta; };

	VOID SetCursor(BOOL state);
	BOOL GetCursor(void);
	inline UCHAR GetCursorMode() { return mCursorMode; }
	inline VOID SetCursorMode(UCHAR mode) { mCursorMode = mode; }
	
	inline VOID SetKey(DWORD code, BOOL state) { mKeys[code] = state; }
	inline VOID SetKeyDown(DWORD code, BOOL state) { mKeysDown[code] = state; }
	inline VOID SetKeyUp(DWORD code, BOOL state) { mKeysUp[code] = state; }

	inline VOID SetMouseButton(DWORD code, BOOL state) { mMouseInputs[code] = state; }
	inline VOID SetMouseDown(DWORD code, BOOL state) { mMouseDown[code] = state; }
	inline VOID SetMouseUp(DWORD code, BOOL state) { mMouseUp[code] = state; }
	VOID SetMouseXY(SHORT x, SHORT y);

	inline VOID ClearKey(void) { mLastKey = -1; }
	VOID Update(void);

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

	BOOL mMouseInputs[NUM_MOUSEBUTTONS], mMouseDown[NUM_MOUSEBUTTONS], mMouseUp[NUM_MOUSEBUTTONS];
	UCHAR mCursorMode;

	POINT mMouseDelta;
	POINT mLastMousePos;
};
