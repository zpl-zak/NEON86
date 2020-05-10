// engine.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "engine.h"

#include "Renderer.h"
#include "Input.h"
#include "FileSystem.h"
#include "LuaMachine.h"
#include "UserInterface.h"
#include <ctime>

CEngine::CEngine(VOID)
: mIsRunning(FALSE)
{
    srand((size_t)time(0));
    sInstance = this; 
    mIsInitialised = FALSE;
    mRenderer = NULL;
    mInput = NULL;
    mFileSystem = NULL;
    mLuaMachine = NULL;
    mUserInterface = NULL;
    
    SetFPS(60.0f);
    mUnprocessedTime = 0.0f;
    mLastTime = 0.0f;

    return; 
}

BOOL CEngine::Release()
{
    SAFE_RELEASE(mLuaMachine);
    SAFE_RELEASE(mFileSystem);
    SAFE_RELEASE(mUserInterface);
    SAFE_RELEASE(mRenderer);
    SAFE_RELEASE(mInput);

    return TRUE;
}

VOID CEngine::Shutdown()
{
    mIsRunning = FALSE;
}

CEngine* CEngine::the()
{
    if (!sInstance)
        sInstance = new CEngine();

    return sInstance;
}

BOOL CEngine::Init(HWND window, RECT resolution)
{
    if (mIsInitialised)
        return TRUE;

    mRenderer = new CRenderer();
    mInput = new CInput();
    mFileSystem = new CFileSystem();
    mLuaMachine = new CLuaMachine();

    if (mRenderer->CreateDevice(window, resolution) != ERROR_SUCCESS)
    {
        return FALSE;
    }

    mUserInterface = new CUserInterface();

    mIsInitialised = TRUE;
    mIsRunning = TRUE;

    return TRUE;
}

VOID CEngine::Think()
{
    BOOL render = FALSE;
    FLOAT startTime = GetTime();
    FLOAT deltaTime = startTime - mLastTime;
    mLastTime = startTime;

    mUnprocessedTime += deltaTime;

    while (mUnprocessedTime > mUpdateDuration)
    {
        Update(mUpdateDuration);
        render = TRUE;
        mUnprocessedTime -= mUpdateDuration;
    }

    if (render)
        Render();
    else
        Sleep(1); // Let CPU sleep a bit
}

VOID CEngine::ResetApplicationTime()
{
    mLastTime = GetTime(TRUE);
}

LRESULT CEngine::ProcessEvents(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (mUserInterface->ProcessEvents(hWnd, message, wParam, lParam))
        return FALSE;

    switch (message)
    {
#if 0
        case WM_SIZE:
        {
            RECT rect;
            GetClientRect(hWnd, &rect);
            rect.right -= rect.left;
            rect.bottom -= rect.top;

            ENGINE->Resize(rect);
        } break;
#endif

        case WM_DESTROY:
        {
            ENGINE->Shutdown();
            return FALSE;
        } break;

        case WM_LBUTTONDOWN:
        {
            if (wParam & MK_LBUTTON)
            {
                INPUT->SetMouseButton(CInput::MOUSE_LEFT_BUTTON, TRUE);
                INPUT->SetMouseDown(CInput::MOUSE_LEFT_BUTTON, TRUE);
            }

            if (wParam & MK_MBUTTON)
            {
                INPUT->SetMouseButton(CInput::MOUSE_MIDDLE_BUTTON, TRUE);
                INPUT->SetMouseDown(CInput::MOUSE_MIDDLE_BUTTON, TRUE);
            }

            if (wParam & MK_RBUTTON)
            {
                INPUT->SetMouseButton(CInput::MOUSE_RIGHT_BUTTON, TRUE);
                INPUT->SetMouseDown(CInput::MOUSE_RIGHT_BUTTON, TRUE);
            }
        } break;

        case WM_LBUTTONUP:
        {
            if (wParam & MK_LBUTTON)
            {
                INPUT->SetMouseButton(CInput::MOUSE_LEFT_BUTTON, FALSE);
                INPUT->SetMouseUp(CInput::MOUSE_LEFT_BUTTON, TRUE);
            }

            if (wParam & MK_MBUTTON)
            {
                INPUT->SetMouseButton(CInput::MOUSE_MIDDLE_BUTTON, FALSE);
                INPUT->SetMouseUp(CInput::MOUSE_MIDDLE_BUTTON, TRUE);
            }

            if (wParam & MK_RBUTTON)
            {
                INPUT->SetMouseButton(CInput::MOUSE_RIGHT_BUTTON, FALSE);
                INPUT->SetMouseUp(CInput::MOUSE_RIGHT_BUTTON, TRUE);
            }
        } break;

        case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;

        case WM_KEYDOWN:
        {
            if (INPUT->GetKey(wParam))
                break;

            INPUT->SetKey(wParam, TRUE);
            INPUT->SetKeyDown(wParam, TRUE);
        } break;

        case WM_KEYUP:
        {
            INPUT->SetKey(wParam, FALSE);
            INPUT->SetKeyUp(wParam, TRUE);
        } break;
    }

    return TRUE;
}

VOID CEngine::Update(FLOAT deltaTime)
{
    mLuaMachine->Update(deltaTime);
    mUserInterface->Update(deltaTime);
    mInput->Update();
}

VOID CEngine::Render()
{
    mRenderer->GetDevice()->SetRenderState(D3DRS_LIGHTING, (DWORD)mRenderer->IsLightingEnabled());
    mRenderer->GetDevice()->BeginScene();
    mRenderer->ClearBuffer(0xFF000000);
    mLuaMachine->Render();
    mUserInterface->Render();
    mRenderer->Blit();
    mRenderer->GetDevice()->EndScene();
    mRenderer->GetDevice()->Present(NULL, NULL, NULL, NULL);
}

VOID CEngine::Resize(RECT resolution)
{
    if (!mIsInitialised)
        return;
    mLuaMachine->Resize(resolution);
    mRenderer->Resize(resolution);
}

CEngine *CEngine::sInstance = NULL;
