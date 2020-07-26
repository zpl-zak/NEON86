#include "system.h"

class CRenderer;
class CInput;
class CFileSystem;
class CVirtualMachine;
class CUserInterface;
class CAudioSystem;
class CProfiler;

#define ENGINE CEngine::the()

class ENGINE_API CEngine {
public:
	CEngine(VOID);

	static CEngine* the();

	BOOL Init(HWND window, RECT resolution);
	BOOL Release();
	VOID Run();
	VOID Shutdown();
	VOID Resize(RECT resolution);
	VOID Think();
	LRESULT ProcessEvents(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	CRenderer* GetRenderer() { return mRenderer; }
	CInput* GetInput() { return mInput; }
	CFileSystem* GetFileSystem() { return mFileSystem; }
	CVirtualMachine* GetVM() { return mVirtualMachine; }
	CUserInterface* GetUI() { return mDebugUI; }
	CAudioSystem* GetAudioSystem() { return mAudioSystem; }

	BOOL IsRunning() const { return mIsRunning; }

	inline VOID SetFPS(FLOAT fps) { if (fps) mUpdateDuration = 1.0f / fps; }
	inline FLOAT GetFPS() const { return 1.0f / mUpdateDuration; }

	class ENGINE_API CDefaultProfiling {
	public:
		friend class CEngine;

		CDefaultProfiling() {
            mTotalTime = 0.0f;
            mTotalMeasuredTime = 0.0f;
            mFrames = 0;
            mFrameCounter = 0.0f;
            mRunCycle = 0;

			mProfilers.Release();
            mUpdateProfiler = NULL;
            mRenderProfiler = NULL;
            mRender2DProfiler = NULL;
            mWindowProfiler = NULL;
		}
        VOID UpdateProfilers(FLOAT dt);
        VOID IncrementFrame();
        VOID SetupDefaultProfilers();
		VOID PushProfiler(CProfiler* profile);

		inline const CArray<CProfiler*> GetProfilers() const { return mProfilers; }
        inline FLOAT GetTotalRunTime() { return mTotalTime; };
        inline FLOAT GetTotalMeasuredRunTime() { return mTotalMeasuredTime; };
        inline INT GetRunCycleCount() { return mRunCycle; }
		inline CProfiler* INTERNAL_GetRender2DProfiler() { return mRender2DProfiler; }
	protected:
		INT mFrames;
        FLOAT mFrameCounter;
        FLOAT mTotalTime;
        FLOAT mTotalMeasuredTime;
        INT mRunCycle;
        CArray<CProfiler*> mProfilers;

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

	VOID Update(FLOAT deltaTime);
	VOID Render();

	static CEngine *sInstance;
	BOOL mIsInitialised;
	BOOL mIsRunning;
	FLOAT mUnprocessedTime;
	FLOAT mLastTime;
	FLOAT mFrameCounter;
	FLOAT mUpdateDuration;
};
