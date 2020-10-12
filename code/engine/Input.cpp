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
    mMouseDelta = {0, 0};
    mLastMousePos = GetMouseXY();

    #ifdef _DEBUG
    mForceMouseCursor = FALSE;
    #endif
}

VOID CInput::Release(VOID)
{
}

auto CInput::GetMouseXY(VOID) -> POINT
{
    POINT newPos = {0};
    GetCursorPos(&newPos);
    ScreenToClient(RENDERER->GetWindow(), &newPos);

    return newPos;
}

VOID CInput::SetCursor(BOOL state) const
{
    if (GetCursor() == state)
        return;

    #ifdef _DEBUG
    if (mForceMouseCursor)
        return;
    #endif

    ShowCursor(state);
}

auto CInput::GetCursor(VOID) -> BOOL
{
    CURSORINFO pci = {0};
    pci.cbSize = sizeof(CURSORINFO);
    GetCursorInfo(&pci);

    return pci.flags == CURSOR_SHOWING;
}

VOID CInput::SetCursorMode(UCHAR mode)
{
    if (mCursorMode == mode)
        return;

    #ifdef _DEBUG
    if (mForceMouseCursor)
        return;
    #endif

    mCursorMode = mode;

    if (mCursorMode == CURSORMODE_CENTERED || mCursorMode == CURSORMODE_WRAPPED)
    {
        const auto res = RENDERER->GetResolution();
        SetMouseXY(static_cast<SHORT>(res.right / 2.0f), static_cast<SHORT>(res.bottom / 2.0f));
        mLastMousePos = GetMouseXY();
    }
}

VOID CInput::SetMouseXY(SHORT x, SHORT y)
{
    POINT pos = {x, y};
    ClientToScreen(RENDERER->GetWindow(), &pos);
    SetCursorPos(pos.x, pos.y);
}

VOID CInput::Update(VOID)
{
    #ifdef _DEBUG
    if (this->GetKeyDown(VK_F1))
    {
        const auto lastState = mForceMouseCursor;
        if (lastState)
            mForceMouseCursor = FALSE;
        this->SetCursor(!this->GetCursor());
        this->SetCursorMode(1 - this->GetCursorMode());
        if (lastState == FALSE)
            mForceMouseCursor = TRUE;
    }
    #endif

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

    auto mousePos = GetMouseXY();
    mMouseDelta.x = -(mLastMousePos.x - mousePos.x);
    mMouseDelta.y = -(mLastMousePos.y - mousePos.y);

    switch (mCursorMode)
    {
    case CURSORMODE_CENTERED:
        {
            const auto res = RENDERER->GetResolution();
            const POINT pos = {
                static_cast<LONG>(res.right / 2.0f),
                static_cast<LONG>(res.bottom / 2.0f)
            };
            SetMouseXY(static_cast<SHORT>(pos.x), static_cast<SHORT>(pos.y));
            mousePos = pos;
        }
        break;
    case CURSORMODE_WRAPPED:
        {
            const auto res = RENDERER->GetLocalCoordinates();
            auto pos = mousePos;
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

            SetMouseXY(static_cast<SHORT>(pos.x), static_cast<SHORT>(pos.y));
            mousePos = pos;
        }
        break;
    default:
        break;
    }

    mLastMousePos = mousePos;
}
