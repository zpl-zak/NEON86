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

	inline BOOL GetKey(CHAR code) const { return mKeys[code]; }
	inline BOOL GetKeyDown(CHAR code) const { return mKeysDown[code]; }
	inline BOOL GetKeyUp(CHAR code) const { return mKeysUp[code]; }
	inline BOOL GetMouse(CHAR code) const { return mMouseInputs[code]; }
	inline BOOL GetMouseDown(CHAR code) const { return mMouseDown[code]; }
	inline BOOL GetMouseUp(CHAR code) const { return mMouseUp[code]; }
	inline BOOL GetLastKey(void) const { return mLastKey; }
	POINT GetMouseXY(void) const;
	inline POINT GetMouseDelta(void) const { return mMouseDelta; };

	VOID SetCursor(BOOL state);
	BOOL GetCursor(void);
	inline UCHAR GetCursorMode() { return mCursorMode; }
	inline VOID SetCursorMode(UCHAR mode) { mCursorMode = mode; }
	
	inline VOID SetKey(CHAR code, BOOL state) { mKeys[code] = state; }
	inline VOID SetKeyDown(CHAR code, BOOL state) { mKeysDown[code] = state; }
	inline VOID SetKeyUp(CHAR code, BOOL state) { mKeysUp[code] = state; }

	inline VOID SetMouseButton(CHAR code, BOOL state) { mMouseInputs[code] = state; }
	inline VOID SetMouseDown(CHAR code, BOOL state) { mMouseDown[code] = state; }
	inline VOID SetMouseUp(CHAR code, BOOL state) { mMouseUp[code] = state; }
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

	static constexpr int NUM_KEYS = 512;
	static constexpr int NUM_MOUSEBUTTONS = 256;
private:
	BOOL mKeys[NUM_KEYS], mKeysDown[NUM_KEYS], mKeysUp[NUM_KEYS];
	CHAR mLastKey;

	BOOL mMouseInputs[NUM_MOUSEBUTTONS], mMouseDown[NUM_MOUSEBUTTONS], mMouseUp[NUM_MOUSEBUTTONS];
	UCHAR mCursorMode;

	POINT mMouseDelta;
	POINT mLastMousePos;
};
