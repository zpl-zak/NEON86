#include "stdafx.h"
#include "Input.h"
#include "engine.h"
#include "Renderer.h"

CInput::CInput()
{
    ZeroMemory(mKeys, NUM_KEYS * sizeof(bool));
    ZeroMemory(mKeysDown, NUM_KEYS * sizeof(bool));
    ZeroMemory(mKeysUp, NUM_KEYS * sizeof(bool));

    ZeroMemory(mMouseInputs, NUM_MOUSEBUTTONS * sizeof(bool));
    ZeroMemory(mMouseDown, NUM_MOUSEBUTTONS * sizeof(bool));
    ZeroMemory(mMouseUp, NUM_MOUSEBUTTONS * sizeof(bool));

    mLastKey = -1;
    mCursorMode = CURSORMODE_DEFAULT;
    mMouseDelta = {0, 0};
    mLastMousePos = GetMouseXY();

    #ifdef _DEBUG
    mForceMouseCursor = FALSE;
    #endif
}

void CInput::Release()
{
}

auto CInput::GetMouseXY() -> POINT
{
    POINT newPos = {0};
    GetCursorPos(&newPos);
    ScreenToClient(RENDERER->GetWindow(), &newPos);

    return newPos;
}

void CInput::SetCursor(bool state) const
{
    if (GetCursor() == state)
    {
        return;
    }

    #ifdef _DEBUG
    if (mForceMouseCursor)
    {
        return;
    }
    #endif

    ShowCursor(static_cast<BOOL>(state));
}

auto CInput::GetCursor() -> bool
{
    CURSORINFO pci = {0};
    pci.cbSize = sizeof(CURSORINFO);
    GetCursorInfo(&pci);

    return pci.flags == CURSOR_SHOWING;
}

void CInput::SetCursorMode(UCHAR mode)
{
    if (mCursorMode == mode)
    {
        return;
    }

    #ifdef _DEBUG
    if (mForceMouseCursor)
    {
        return;
    }
    #endif

    mCursorMode = mode;

    if (mCursorMode == CURSORMODE_CENTERED || mCursorMode == CURSORMODE_WRAPPED)
    {
        const auto res = RENDERER->GetResolution();
        SetMouseXY(static_cast<short>(res.right / 2.0F), static_cast<short>(res.bottom / 2.0F));
        mLastMousePos = GetMouseXY();
    }
}

void CInput::SetMouseXY(short x, short y)
{
    POINT pos = {x, y};
    ClientToScreen(RENDERER->GetWindow(), &pos);
    SetCursorPos(pos.x, pos.y);
}

void CInput::Update()
{
    #ifdef _DEBUG
    if (this->GetKeyDown(VK_F1))
    {
        const auto lastState = mForceMouseCursor;
        if (lastState)
        {
            mForceMouseCursor = FALSE;
        }
        this->SetCursor(!this->GetCursor());
        this->SetCursorMode(1 - this->GetCursorMode());
        if (static_cast<int>(lastState) == FALSE)
        {
            mForceMouseCursor = TRUE;
        }
    }
    #endif

    for (unsigned int i = 0; i < NUM_MOUSEBUTTONS; i++)
    {
        SetMouseDown(i, FALSE);
        SetMouseUp(i, FALSE);
    }

    for (unsigned int i = 0; i < NUM_KEYS; i++)
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
                static_cast<LONG>(res.right / 2.0F),
                static_cast<LONG>(res.bottom / 2.0F)
            };
            SetMouseXY(static_cast<short>(pos.x), static_cast<short>(pos.y));
            mousePos = pos;
        }
        break;
    case CURSORMODE_WRAPPED:
        {
            const auto res = RENDERER->GetLocalCoordinates();
            auto pos = mousePos;
            ClientToScreen(RENDERER->GetWindow(), &pos);

            while (pos.x > res.right)
            {
                pos.x -= res.right;
            }

            while (pos.y > res.bottom)
            {
                pos.y -= res.bottom;
            }

            while (pos.x < res.left)
            {
                pos.x += res.left;
            }

            while (pos.y < res.top)
            {
                pos.y += res.top;
            }

            ScreenToClient(RENDERER->GetWindow(), &pos);

            SetMouseXY(static_cast<short>(pos.x), static_cast<short>(pos.y));
            mousePos = pos;
        }
        break;
    default:
        break;
    }

    mLastMousePos = mousePos;
}
