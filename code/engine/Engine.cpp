// engine.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "engine.h"

#include "Renderer.h"
#include "Input.h"
#include "FileSystem.h"
#include "VM.h"
#include "UserInterface.h"
#include "AudioSystem.h"
#include "ProfileManager.h"

#include <ctime>

CEngine::CEngine(VOID)
: mIsRunning(FALSE)
{
    srand((UINT)time(0));
    sInstance = this; 
    mIsInitialised = FALSE;
    mRenderer = NULL;
    mInput = NULL;
    mFileSystem = NULL;
    mVirtualMachine = NULL;
    mGameEditor = NULL;
    mAudioSystem = NULL;
    
    SetFPS(60.0f);
    mUnprocessedTime = 0.0f;
    mLastTime = 0.0f;
    mTotalTime = 0.0f;
    mTotalMeasuredTime = 0.0f;
    mFrames = 0;
    mFrameCounter = 0.0f;
    mRunCycle = 0;

    mUpdateProfiler = new CProfiler("Update");
    mRenderProfiler = new CProfiler("Render");
    mRender2DProfiler = new CProfiler("RenderUI");
    mWindowProfiler = new CProfiler("Window");
    mSleepProfiler = new CProfiler("Sleep");
    

    mProfilers[NEON_PROFILER_RENDER] = mRenderProfiler;
    mProfilers[NEON_PROFILER_RENDER2D] = mRender2DProfiler;
    mProfilers[NEON_PROFILER_UPDATE] = mUpdateProfiler;
    mProfilers[NEON_PROFILER_SLEEP] = mSleepProfiler;
    mProfilers[NEON_PROFILER_WINDOW] = mWindowProfiler;
}

BOOL CEngine::Release()
{
    SAFE_RELEASE(mVirtualMachine);
    SAFE_RELEASE(mFileSystem);
    SAFE_RELEASE(mGameEditor);
    SAFE_RELEASE(mRenderer);
    SAFE_RELEASE(mInput);
    SAFE_RELEASE(mAudioSystem);

    SAFE_DELETE(mUpdateProfiler);
    SAFE_DELETE(mRenderProfiler);
    SAFE_DELETE(mRender2DProfiler);
    SAFE_DELETE(mSleepProfiler);
    SAFE_DELETE(mWindowProfiler);
    ZeroMemory(mProfilers, MAX_NEON_PROFILERS);

    return TRUE;
}

VOID CEngine::Run()
{
    MSG msg;

    GetTime();
    Sleep(50);

    while (IsRunning())
    {
        mWindowProfiler->StartInvocation();
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        mWindowProfiler->StopInvocation();

        if (!IsRunning())
            break;

        Think();
    }

    mVirtualMachine->Stop();
    Release();
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
    mVirtualMachine = new CVirtualMachine();

    if (mRenderer->CreateDevice(window, resolution) != ERROR_SUCCESS)
    {
        MessageBoxA(window, "Failed to initialize the renderer!", "Renderer error", MB_OK);
        return FALSE;
    }

    mGameEditor = new CUserInterface();
    mAudioSystem = new CAudioSystem();

    if (mAudioSystem->CreateDevice(window) != ERROR_SUCCESS)
    {
        MessageBoxA(window, "Failed to initialize the audio system!", "Audio error", MB_OK);
        return FALSE;
    }

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
    mFrameCounter += deltaTime;

    if (mFrameCounter >= 1.0f) 
    {
        mTotalTime = ((1000.0f * mFrameCounter) / ((FLOAT)mFrames));
        mTotalMeasuredTime = 0.0f;
        BOOL logStats = mRunCycle % 10 == 0;

        if (logStats) OutputDebugStringA("==================\n");

        for (INT i=0; i<MAX_NEON_PROFILERS; i++)
        {
            mTotalMeasuredTime += mProfilers[i]->DisplayAndReset(FLOAT(mFrames), logStats);
        }

        if (logStats)
        {
            OutputDebugStringA("\n");
            OutputDebugStringA(std::string("Other Time: " + std::to_string(mTotalTime - mTotalMeasuredTime) + " ms\n").c_str());
            OutputDebugStringA(std::string("Total Time: " + std::to_string(mTotalTime) + " ms (" + std::to_string(1000.0f / mTotalTime) + " fps) \n").c_str());
        }

        mFrames = 0;
        mFrameCounter = 0.0f;
        mRunCycle++;
    }

    while (mUnprocessedTime > mUpdateDuration)
    {
        Update(mUpdateDuration);
        render = TRUE;
        mUnprocessedTime -= mUpdateDuration;
    }

    if (render)
    {
        Render();
        mFrames++;
    }
    else
    {
        mSleepProfiler->StartInvocation();
        Sleep(1); // Let CPU sleep a bit
        mSleepProfiler->StopInvocation();
    }
}

LRESULT CEngine::ProcessEvents(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (mGameEditor->ProcessEvents(hWnd, message, wParam, lParam))
        return FALSE;

    CInput* input = INPUT;

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
            input->SetMouseButton(CInput::MOUSE_LEFT_BUTTON, TRUE);
            input->SetMouseDown(CInput::MOUSE_LEFT_BUTTON, TRUE);
        } break;

        case WM_MBUTTONDOWN:
        {
            input->SetMouseButton(CInput::MOUSE_MIDDLE_BUTTON, TRUE);
            input->SetMouseDown(CInput::MOUSE_MIDDLE_BUTTON, TRUE);
        } break;

        case WM_RBUTTONDOWN:
        {
            input->SetMouseButton(CInput::MOUSE_RIGHT_BUTTON, TRUE);
            input->SetMouseDown(CInput::MOUSE_RIGHT_BUTTON, TRUE);
        } break;

        case WM_LBUTTONUP:
        {
            input->SetMouseButton(CInput::MOUSE_LEFT_BUTTON, FALSE);
            input->SetMouseUp(CInput::MOUSE_LEFT_BUTTON, TRUE);
        } break;

        case WM_MBUTTONUP:
        {
            input->SetMouseButton(CInput::MOUSE_MIDDLE_BUTTON, FALSE);
            input->SetMouseUp(CInput::MOUSE_MIDDLE_BUTTON, TRUE);
        } break;

        case WM_RBUTTONUP:
        {
            input->SetMouseButton(CInput::MOUSE_RIGHT_BUTTON, FALSE);
            input->SetMouseUp(CInput::MOUSE_RIGHT_BUTTON, TRUE);
        } break;

        case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;

        case WM_CHAR:
        {
            if (wParam >= 0x20) 
            {
                VM->CharInput((DWORD)wParam);
            }
        } break;

        case WM_KEYDOWN:
        {
            if (INPUT->GetKey((DWORD)wParam))
                break;

            INPUT->SetKey((DWORD)wParam, TRUE);
            INPUT->SetKeyDown((DWORD)wParam, TRUE);
        } break;

        case WM_KEYUP:
        {
            INPUT->SetKey((DWORD)wParam, FALSE);
            INPUT->SetKeyUp((DWORD)wParam, TRUE);
        } break;
    }

    return TRUE;
}

VOID CEngine::Update(FLOAT deltaTime)
{
    mUpdateProfiler->StartInvocation();
    mVirtualMachine->Update(deltaTime);
    mUpdateProfiler->StopInvocation();
    mGameEditor->Update(deltaTime);
    mInput->Update();
}

VOID CEngine::Render()
{
    mRenderer->BeginRender();
    mRenderProfiler->StartInvocation();
    mVirtualMachine->Render();
    mRenderProfiler->StopInvocation();

    mRender2DProfiler->StartInvocation();
    mGameEditor->Render();
    mRender2DProfiler->StopInvocation();

    mWindowProfiler->StartInvocation();
    mRenderer->EndRender();
    mWindowProfiler->StopInvocation();
}

VOID CEngine::Resize(RECT resolution)
{
    if (!mIsInitialised)
        return;
    mVirtualMachine->Resize(resolution);
    mRenderer->Resize(resolution);
}

CEngine *CEngine::sInstance = NULL;
