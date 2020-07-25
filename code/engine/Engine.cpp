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
}

BOOL CEngine::Release()
{
    SAFE_RELEASE(mVirtualMachine);
    SAFE_RELEASE(mFileSystem);
    SAFE_RELEASE(mDebugUI);
    SAFE_RELEASE(mRenderer);
    SAFE_RELEASE(mInput);
    SAFE_RELEASE(mAudioSystem);

    return TRUE;
}

VOID CEngine::Run()
{
    while (IsRunning())
        Think();

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

    DefaultProfiling.UpdateProfilers(deltaTime);

    MSG msg;

    if (mUnprocessedTime > mUpdateDuration)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            CProfileScope scope(DefaultProfiling.mWindowProfiler);
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!IsRunning())
            return;

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
        CProfileScope scope(DefaultProfiling.mWindowProfiler);
        Sleep(1);
    }
}

VOID CEngine::CDefaultProfiling::UpdateProfilers(FLOAT dt)
{
    static constexpr FLOAT sFrameWindow = 0.5f;

    mFrameCounter += dt;

    if (mFrameCounter >= sFrameWindow)
    {
        mTotalTime = ((1000.0f * mFrameCounter) / ((FLOAT)mFrames));
        mTotalMeasuredTime = 0.0f;
        BOOL logStats = mRunCycle % (INT(sFrameWindow * 10.0f)) == 0;

        if (logStats) PushLog("==================\n", TRUE);

        for (UINT i = 0; i < mProfilers.GetCount(); i++)
        {
            mTotalMeasuredTime += mProfilers[i]->DisplayAndReset(FLOAT(mFrames), logStats);
        }

        if (logStats)
        {
            PushLog("\n", TRUE);
            PushLog(CString::Format("Other Time: %f ms\n", ((DOUBLE)mTotalTime - mTotalMeasuredTime)).Str(), TRUE);
            PushLog(CString::Format("Total Time: %f ms (%f fps)\n", mTotalTime, (1000.0f / mTotalTime)).Str(), TRUE);
        }

        UI->PushMS(mTotalTime);

        mFrames = 0;
        mFrameCounter = 0.0f;
        mRunCycle++;
    }
}

VOID CEngine::CDefaultProfiling::IncrementFrame()
{
    mFrames++;
}

VOID CEngine::CDefaultProfiling::SetupDefaultProfilers()
{
    mUpdateProfiler = new CProfiler("Update");
    mRenderProfiler = new CProfiler("Render");
    mRender2DProfiler = new CProfiler("RenderUI");
    mWindowProfiler = new CProfiler("Window");

    mProfilers.Push(mUpdateProfiler);
    mProfilers.Push(mRenderProfiler);
    mProfilers.Push(mRender2DProfiler);
    mProfilers.Push(mWindowProfiler);
}

VOID CEngine::CDefaultProfiling::PushProfiler(CProfiler* profile)
{
    mProfilers.Push(profile);
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
        CProfileScope scope(DefaultProfiling.mUpdateProfiler);
        mVirtualMachine->Update(deltaTime);
    }
 
    mDebugUI->Update(deltaTime);
    mInput->Update();
}

VOID CEngine::Render()
{
    mRenderer->BeginRender();
    
    {
        CProfileScope scope(DefaultProfiling.mRenderProfiler);
        mVirtualMachine->Render();
    }

    mDebugUI->Render();

    {
        CProfileScope scope(DefaultProfiling.mWindowProfiler);
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
