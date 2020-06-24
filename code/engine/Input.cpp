#include "stdafx.h"
#include "Input.h"
#include "engine.h"
#include "Renderer.h"

CInput::CInput(VOID)
{
    ZeroMemory(mKeys, NUM_KEYS * sizeof(BOOL));
    ZeroMemory(mKeysDown, NUM_KEYS * sizeof(BOOL));
    ZeroMemory(mKeysUp, NUM_KEYS * sizeof(BOOL));

    ZeroMemory(mMouseInputs, NUM_MOUSEBUTTONS * sizeof(BOOL));
    ZeroMemory(mMouseDown, NUM_MOUSEBUTTONS * sizeof(BOOL));
    ZeroMemory(mMouseUp, NUM_MOUSEBUTTONS * sizeof(BOOL));

    mLastKey = -1;
    mCursorMode = CURSORMODE_DEFAULT;
    mMouseDelta = { 0,0 };
    mLastMousePos = GetMouseXY();
}

VOID CInput::Release(VOID)
{

}

POINT CInput::GetMouseXY(VOID) const
{
    POINT newPos = { 0 };
    GetCursorPos(&newPos);
    ScreenToClient(RENDERER->GetWindow(), &newPos);

    return newPos;
}

VOID CInput::SetCursor(BOOL state)
{
    if (GetCursor() == state)
        return;

    ShowCursor(state);
}

BOOL CInput::GetCursor(VOID)
{
    CURSORINFO pci = { 0 };
    pci.cbSize = sizeof(CURSORINFO);
    GetCursorInfo(&pci);

    return pci.flags == CURSOR_SHOWING;
}

VOID CInput::SetCursorMode(UCHAR mode)
{
    if (mCursorMode == mode)
        return;

    mCursorMode = mode;

    if (mCursorMode == CURSORMODE_CENTERED || mCursorMode == CURSORMODE_WRAPPED)
    {
        RECT res = RENDERER->GetResolution();
        SetMouseXY((SHORT)(res.right / 2.0f), (SHORT)(res.bottom / 2.0f));
        mLastMousePos = GetMouseXY();
    }
}

VOID CInput::SetMouseXY(SHORT x, SHORT y)
{
    POINT pos = { x,y };
    ClientToScreen(RENDERER->GetWindow(), &pos);
    SetCursorPos(pos.x, pos.y);
}

VOID CInput::Update(VOID)
{
    for (UINT i = 0; i < NUM_MOUSEBUTTONS; i++)
    {
        SetMouseDown(i, FALSE);
        SetMouseUp(i, FALSE);
    }

    for (UINT i = 0; i < NUM_KEYS; i++)
    {
        SetKeyDown(i, FALSE);
        SetKeyUp(i, FALSE);
    }

    ClearKey();

    POINT mousePos = GetMouseXY();
    mMouseDelta.x = -(mLastMousePos.x - mousePos.x);
    mMouseDelta.y = -(mLastMousePos.y - mousePos.y);

    switch (mCursorMode)
    {
        case CURSORMODE_CENTERED:
        {
            RECT res = RENDERER->GetResolution();
            POINT pos = {
                (LONG)(res.right/2.0f),
                (LONG)(res.bottom/2.0f)
            };
            SetMouseXY((SHORT)pos.x, (SHORT)pos.y);
            mousePos = pos;
        } break;
        case CURSORMODE_WRAPPED:
        {
            RECT res = RENDERER->GetLocalCoordinates();
            POINT pos = mousePos;
            ClientToScreen(RENDERER->GetWindow(), &pos);

            while (pos.x > res.right)
                pos.x -= res.right;

            while (pos.y > res.bottom)
                pos.y -= res.bottom;

            while (pos.x < res.left)
                pos.x += res.left;

            while (pos.y < res.top)
                pos.y += res.top;

            ScreenToClient(RENDERER->GetWindow(), &pos);

            SetMouseXY((SHORT)pos.x, (SHORT)pos.y);
            mousePos = pos;
        }
            break;
        default:
            break;
    }

    mLastMousePos = mousePos;
}
