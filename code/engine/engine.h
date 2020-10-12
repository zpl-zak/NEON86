#include "system.h"

class CRenderer;
class CInput;
class CFileSystem;
class CVirtualMachine;
class CUserInterface;
class CAudioSystem;
class CProfiler;

#define ENGINE CEngine::the()

class ENGINE_API CEngine
{
public:
    CEngine(VOID);

    // ReSharper disable once CppInconsistentNaming
    static CEngine* the();

    BOOL Init(HWND window, RECT resolution);
    BOOL Release();
    VOID Run();
    VOID Shutdown();
    VOID Resize(RECT resolution) const;
    VOID Think();
    LRESULT ProcessEvents(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) const;

    CRenderer* GetRenderer() const { return mRenderer; }
    CInput* GetInput() const { return mInput; }
    CFileSystem* GetFileSystem() const { return mFileSystem; }
    CVirtualMachine* GetVM() const { return mVirtualMachine; }
    CUserInterface* GetUI() const { return mDebugUI; }
    CAudioSystem* GetAudioSystem() const { return mAudioSystem; }

    BOOL IsRunning() const { return mIsRunning; }

    VOID SetFPS(FLOAT fps) { if (fps) mUpdateDuration = 1.0f / fps; }
    FLOAT GetFPS() const { return 1.0f / mUpdateDuration; }

    class ENGINE_API CDefaultProfiling
    {
    public:
        friend class CEngine;

        CDefaultProfiling()
        {
            mTotalTime = 0.0f;
            mTotalMeasuredTime = 0.0f;
            mFrames = 0;
            mFrameCounter = 0.0f;
            mRunCycle = 0;
            mVerboseLogging = FALSE;

            mProfilers.Release();
            mUpdateProfiler = nullptr;
            mRenderProfiler = nullptr;
            mRender2DProfiler = nullptr;
            mWindowProfiler = nullptr;
        }

        VOID UpdateProfilers(FLOAT dt);
        VOID IncrementFrame();
        VOID SetupDefaultProfilers();
        VOID PushProfiler(CProfiler* profile);
        VOID SetVerboseLogging(BOOL state) { mVerboseLogging = state; }

        const CArray<CProfiler*> GetProfilers() const { return mProfilers; }
        FLOAT GetTotalRunTime() const { return mTotalTime; };
        FLOAT GetTotalMeasuredRunTime() const { return mTotalMeasuredTime; };
        INT GetRunCycleCount() const { return mRunCycle; }
        CProfiler* INTERNAL_GetRender2DProfiler() const { return mRender2DProfiler; }
    protected:
        INT mFrames;
        FLOAT mFrameCounter;
        FLOAT mTotalTime;
        FLOAT mTotalMeasuredTime;
        INT mRunCycle;
        CArray<CProfiler*> mProfilers;
        BOOL mVerboseLogging;

        /// Internal profilers
        CProfiler* mUpdateProfiler;
        CProfiler* mRenderProfiler;
        CProfiler* mRender2DProfiler;
        CProfiler* mWindowProfiler;
    } DefaultProfiling;

protected:
    CRenderer* mRenderer;
    CInput* mInput;
    CFileSystem* mFileSystem;
    CVirtualMachine* mVirtualMachine;
    CUserInterface* mDebugUI;
    CAudioSystem* mAudioSystem;

    VOID Update(FLOAT deltaTime) const;
    VOID Render() const;

    static CEngine* sInstance;
    BOOL mIsInitialised;
    BOOL mIsRunning;
    FLOAT mUnprocessedTime;
    FLOAT mLastTime;
    FLOAT mFrameCounter;
    FLOAT mUpdateDuration;
};
