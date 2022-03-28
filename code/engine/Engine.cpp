#include "stdafx.h"
#include "engine.h"

#include "AudioSystem.h"
#include "FileSystem.h"
#include "Input.h"
#include "ProfileManager.h"
#include "Renderer.h"
#include "UserInterface.h"
#include "VM.h"

#include <ctime>

CEngine::CEngine()
    : mIsRunning(FALSE)
{
    sInstance = this;
    mIsInitialised = FALSE;
    mRenderer = nullptr;
    mInput = nullptr;
    mFileSystem = nullptr;
    mVirtualMachine = nullptr;
    mDebugUI = nullptr;
    mAudioSystem = nullptr;

    SetFPS(60);
    mUnprocessedTime = 0.0F;
    mLastTime = 0.0F;
}

auto CEngine::Release() -> bool
{
    SAFE_RELEASE(mVirtualMachine);
    SAFE_RELEASE(mFileSystem);
    SAFE_RELEASE(mDebugUI);
    SAFE_RELEASE(mRenderer);
    SAFE_RELEASE(mInput);
    SAFE_RELEASE(mAudioSystem);

    return TRUE;
}

void CEngine::Run()
{
    while (IsRunning())
    {
        Think();
    }

    mVirtualMachine->Stop();
    Release();
}

void CEngine::Shutdown()
{
    mIsRunning = FALSE;
}

auto CEngine::the() -> CEngine*
{
    if (sInstance == nullptr)
    {
        sInstance = new CEngine();
    }

    return sInstance;
}

auto CEngine::Init(HWND window, RECT resolution) -> bool
{
    if (mIsInitialised)
    {
        return TRUE;
    }

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

void CEngine::Think()
{
    const auto startTime = GetTime();
    const auto deltaTime = startTime - mLastTime;
    mLastTime = startTime;

    mUnprocessedTime += deltaTime;

    DefaultProfiling.UpdateProfilers(deltaTime);

    MSG msg;

    if (mUnprocessedTime > mUpdateDuration)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            CProfileScope scope(DefaultProfiling.mWindowProfiler);
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!IsRunning())
        {
            return;
        }

        FixedUpdate(mUpdateDuration);
        mUnprocessedTime -= mUpdateDuration;
    }

    Update(deltaTime);
    Render(deltaTime);
}

void CEngine::CDefaultProfiling::UpdateProfilers(float dt)
{
    static constexpr auto sFrameWindow = 0.5F;

    mFrameCounter += dt;

    if (mFrameCounter >= sFrameWindow)
    {
        mTotalTime = 1000.0F * mFrameCounter / static_cast<float>(mFrames);
        mTotalMeasuredTime = 0.0F;

        const auto logStats = mVerboseLogging
                                  ? static_cast<int>(mRunCycle % static_cast<int>(sFrameWindow * 120.0F) == 0
                                  )
                                  : FALSE;

        if (logStats != 0)
        {
            PushLog("==================\n", TRUE);
        }

        for (unsigned int i = 0; i < mProfilers.GetCount(); i++)
        {
            mTotalMeasuredTime += mProfilers[i]->DisplayAndReset(static_cast<float>(mFrames), logStats != 0);
        }

        if (logStats != 0)
        {
            PushLog("\n", TRUE);
            PushLog(
                CString::Format("Other Time: %f ms\n", static_cast<DOUBLE>(mTotalTime) - mTotalMeasuredTime).Str(),
                TRUE);
            PushLog(CString::Format("Total Time: %f ms (%f fps)\n", mTotalTime, 1000.0F / mTotalTime).Str(), TRUE);
        }

        UI->PushMS(mTotalTime);

        mFrames = 0;
        mFrameCounter = 0.0F;
        mRunCycle++;
    }
}

void CEngine::CDefaultProfiling::IncrementFrame()
{
    mFrames++;
}

void CEngine::CDefaultProfiling::SetupDefaultProfilers()
{
    mUpdateProfiler = new CProfiler("Update");
    mFixedUpdateProfiler = new CProfiler("FixedUpdate");
    mRenderProfiler = new CProfiler("Render");
    mRender2DProfiler = new CProfiler("RenderUI");
    mWindowProfiler = new CProfiler("Window");

    mProfilers.Push(mUpdateProfiler);
    mProfilers.Push(mFixedUpdateProfiler);
    mProfilers.Push(mRenderProfiler);
    mProfilers.Push(mRender2DProfiler);
    mProfilers.Push(mWindowProfiler);
}

void CEngine::CDefaultProfiling::PushProfiler(CProfiler* profile)
{
    mProfilers.Push(profile);
}

auto CEngine::ProcessEvents(HWND hWnd, unsigned int message, WPARAM wParam, LPARAM lParam) const -> LRESULT
{
    if (CUserInterface::ProcessEvents(hWnd, message, wParam, lParam) != 0)
    {
        return FALSE;
    }

    auto* input = INPUT;

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
        }

    case WM_LBUTTONDOWN:
        {
            input->SetMouseButton(CInput::MOUSE_LEFT_BUTTON, TRUE);
            input->SetMouseDown(CInput::MOUSE_LEFT_BUTTON, TRUE);
        }
        break;

    case WM_MBUTTONDOWN:
        {
            input->SetMouseButton(CInput::MOUSE_MIDDLE_BUTTON, TRUE);
            input->SetMouseDown(CInput::MOUSE_MIDDLE_BUTTON, TRUE);
        }
        break;

    case WM_RBUTTONDOWN:
        {
            input->SetMouseButton(CInput::MOUSE_RIGHT_BUTTON, TRUE);
            input->SetMouseDown(CInput::MOUSE_RIGHT_BUTTON, TRUE);
        }
        break;

    case WM_LBUTTONUP:
        {
            input->SetMouseButton(CInput::MOUSE_LEFT_BUTTON, FALSE);
            input->SetMouseUp(CInput::MOUSE_LEFT_BUTTON, TRUE);
        }
        break;

    case WM_MBUTTONUP:
        {
            input->SetMouseButton(CInput::MOUSE_MIDDLE_BUTTON, FALSE);
            input->SetMouseUp(CInput::MOUSE_MIDDLE_BUTTON, TRUE);
        }
        break;

    case WM_RBUTTONUP:
        {
            input->SetMouseButton(CInput::MOUSE_RIGHT_BUTTON, FALSE);
            input->SetMouseUp(CInput::MOUSE_RIGHT_BUTTON, TRUE);
        }
        break;

    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
        {
            // Disable ALT application menu
            return 0;
        }
        break;

    case WM_CHAR:
        {
            if (wParam >= 0x20)
            {
                VM->CharInput(static_cast<DWORD>(wParam));
            }
        }
        break;

    case WM_KEYDOWN:
        {
            if (INPUT->GetKey(static_cast<DWORD>(wParam)))
            {
                break;
            }

            INPUT->SetKey(static_cast<DWORD>(wParam), TRUE);
            INPUT->SetKeyDown(static_cast<DWORD>(wParam), TRUE);
        }
        break;

    case WM_KEYUP:
        {
            INPUT->SetKey(static_cast<DWORD>(wParam), FALSE);
            INPUT->SetKeyUp(static_cast<DWORD>(wParam), TRUE);
        }
        break;
    }

    return TRUE;
}

void CEngine::Update(float deltaTime) const
{
    {
        CProfileScope scope(DefaultProfiling.mUpdateProfiler);
        mVirtualMachine->Update(deltaTime);
        mAudioSystem->Update();
    }
}

void CEngine::FixedUpdate(float deltaTime) const
{
    {
        CProfileScope scope(DefaultProfiling.mFixedUpdateProfiler);
        mVirtualMachine->FixedUpdate(deltaTime);
    }

    mInput->Update();
}

void CEngine::Render(float renderTime) const
{
    mRenderer->BeginRender();

    {
        CProfileScope scope(DefaultProfiling.mRenderProfiler);
        mVirtualMachine->Render(renderTime);

        if (mDebugUI->IsInError())
        {
            mRenderer->ClearBuffer(0x0);
        }
    }

    mDebugUI->Render();

    {
        CProfileScope scope(DefaultProfiling.mWindowProfiler);
        mRenderer->EndRender();
    }
}

void CEngine::Resize(RECT resolution) const
{
    if (!mIsInitialised)
    {
        return;
    }
    mVirtualMachine->Resize(resolution);
    mRenderer->Resize(resolution);
}

CEngine* CEngine::sInstance = nullptr;
