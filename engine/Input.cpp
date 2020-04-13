#include "stdafx.h"
#include "Input.h"
#include "engine.h"
#include "Renderer.h"

CInput::CInput(void)
{
    ZeroMemory(mKeys, NUM_KEYS * sizeof(BOOL));
    ZeroMemory(mKeysDown, NUM_KEYS * sizeof(BOOL));
    ZeroMemory(mKeysUp, NUM_KEYS * sizeof(BOOL));

    ZeroMemory(mMouseInputs, NUM_MOUSEBUTTONS * sizeof(BOOL));
    ZeroMemory(mMouseDown, NUM_MOUSEBUTTONS * sizeof(BOOL));
    ZeroMemory(mMouseUp, NUM_MOUSEBUTTONS * sizeof(BOOL));

    mLastKey = -1;
}

VOID CInput::Release(void)
{

}

POINT CInput::GetMouseXY(void) const
{
    POINT newPos = { 0 };
    GetCursorPos(&newPos);
    ScreenToClient(RENDERER->GetWindow(), &newPos);

    return newPos;
}

VOID CInput::SetCursor(BOOL state)
{
    ShowCursor(state);
}

BOOL CInput::GetCursor(void)
{
    CURSORINFO pci = { 0 };
    pci.cbSize = sizeof(CURSORINFO);
    GetCursorInfo(&pci);

    return pci.flags == CURSOR_SHOWING;
}

VOID CInput::SetMouseXY(SHORT x, SHORT y)
{
    POINT pos = { x,y };
    ClientToScreen(RENDERER->GetWindow(), &pos);

    SetCursorPos(pos.x, pos.y);
}

VOID CInput::Update(void)
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
}
