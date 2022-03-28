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
    CEngine(void);

    // ReSharper disable once CppInconsistentNaming
    static CEngine* the();

    bool Init(HWND window, RECT resolution);
    bool Release();
    void Run();
    void Shutdown();
    void Resize(RECT resolution) const;
    void Think();
    LRESULT ProcessEvents(HWND hWnd, unsigned int message, WPARAM wParam, LPARAM lParam) const;

    CRenderer* GetRenderer() const { return mRenderer; }
    CInput* GetInput() const { return mInput; }
    CFileSystem* GetFileSystem() const { return mFileSystem; }
    CVirtualMachine* GetVM() const { return mVirtualMachine; }
    CUserInterface* GetUI() const { return mDebugUI; }
    CAudioSystem* GetAudioSystem() const { return mAudioSystem; }

    bool IsRunning() const { return mIsRunning; }

    void SetFPS(float fps) { if (fps) mUpdateDuration = 1.0f / fps; }
    float GetFPS() const { return 1.0f / mUpdateDuration; }

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
            mFixedUpdateProfiler = nullptr;
        }

        void UpdateProfilers(float dt);
        void IncrementFrame();
        void SetupDefaultProfilers();
        void PushProfiler(CProfiler* profile);
        void SetVerboseLogging(bool state) { mVerboseLogging = state; }

        const CArray<CProfiler*> GetProfilers() const { return mProfilers; }
        float GetTotalRunTime() const { return mTotalTime; };
        float GetTotalMeasuredRunTime() const { return mTotalMeasuredTime; };
        int GetRunCycleCount() const { return mRunCycle; }
        CProfiler* INTERNAL_GetRender2DProfiler() const { return mRender2DProfiler; }
    protected:
        int mFrames;
        float mFrameCounter;
        float mTotalTime;
        float mTotalMeasuredTime;
        int mRunCycle;
        CArray<CProfiler*> mProfilers;
        bool mVerboseLogging;

        /// Internal profilers
        CProfiler* mUpdateProfiler;
        CProfiler* mFixedUpdateProfiler;
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

    void Update(float deltaTime) const;
    void FixedUpdate(float deltaTime) const;
    void Render(float renderTime) const;

    static CEngine* sInstance;
    bool mIsInitialised;
    bool mIsRunning;
    float mUnprocessedTime;
    float mLastTime;
    float mFrameCounter{};
    float mUpdateDuration{};
};
