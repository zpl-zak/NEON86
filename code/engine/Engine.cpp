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
    mDebugUI = NULL;
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
    SAFE_RELEASE(mDebugUI);
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
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            CProfileScope scope(mWindowProfiler);
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
     
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

    mDebugUI = new CUserInterface();
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

    UpdateProfilers(deltaTime);

    if (mUnprocessedTime > mUpdateDuration)
    {
        Update(mUpdateDuration);
        render = TRUE;
        mUnprocessedTime -= mUpdateDuration;
    }

    if (render)
    {
        Render();
    }
    else
    {
        CProfileScope scope(mSleepProfiler);
        Sleep(1); // Let CPU sleep a bit
    }
}

VOID CEngine::UpdateProfilers(FLOAT dt)
{
    static constexpr FLOAT sFrameWindow = 0.5f;

    mFrameCounter += dt;

    if (mFrameCounter >= sFrameWindow)
    {
        mTotalTime = ((1000.0f * mFrameCounter) / ((FLOAT)mFrames));
        mTotalMeasuredTime = 0.0f;
        BOOL logStats = mRunCycle % (INT(sFrameWindow * 10.0f)) == 0;

        if (logStats) OutputDebugStringA("==================\n");

        for (INT i = 0; i < MAX_NEON_PROFILERS; i++)
        {
            mTotalMeasuredTime += mProfilers[i]->DisplayAndReset(FLOAT(mFrames), logStats);
        }

        if (logStats)
        {
            OutputDebugStringA("\n");
            OutputDebugStringA(std::string("Other Time: " + std::to_string(mTotalTime - mTotalMeasuredTime) + " ms\n").c_str());
            OutputDebugStringA(std::string("Total Time: " + std::to_string(mTotalTime) + " ms (" + std::to_string(1000.0f / mTotalTime) + " fps) \n").c_str());
        }

        UI->PushMS(mTotalTime);

        mFrames = 0;
        mFrameCounter = 0.0f;
        mRunCycle++;
    }
}

VOID CEngine::IncrementFrame()
{
    mFrames++;
}

LRESULT CEngine::ProcessEvents(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (mDebugUI->ProcessEvents(hWnd, message, wParam, lParam))
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
    {
        CProfileScope scope(mUpdateProfiler);
        mVirtualMachine->Update(deltaTime);
    }
 
    mDebugUI->Update(deltaTime);
    mInput->Update();
}

VOID CEngine::Render()
{
    mRenderer->BeginRender();
    
    {
        CProfileScope scope(mRenderProfiler);
        mVirtualMachine->Render();
    }

    {
        CProfileScope scope(mRender2DProfiler);
        mDebugUI->Render();
    }

    {
        CProfileScope scope(mWindowProfiler);
        mRenderer->EndRender();
    }
}

VOID CEngine::Resize(RECT resolution)
{
    if (!mIsInitialised)
        return;
    mVirtualMachine->Resize(resolution);
    mRenderer->Resize(resolution);
}

CEngine *CEngine::sInstance = NULL;
