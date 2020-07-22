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
	CUserInterface* GetUI() { return mGameEditor; }
	CAudioSystem* GetAudioSystem() { return mAudioSystem; }

	BOOL IsRunning() const { return mIsRunning; }

	inline VOID SetFPS(FLOAT fps) { if (fps) mUpdateDuration = 1.0f / fps; } 
	inline FLOAT GetFPS() const { return 1.0f / mUpdateDuration; }
protected:
	CRenderer* mRenderer;
	CInput* mInput;
	CFileSystem* mFileSystem;
	CVirtualMachine* mVirtualMachine;
	CUserInterface* mGameEditor;
	CAudioSystem* mAudioSystem;
private:
	VOID Update(FLOAT deltaTime);
	VOID Render();

	static CEngine *sInstance;
	BOOL mIsInitialised;
	BOOL mIsRunning;
	FLOAT mUnprocessedTime;
	FLOAT mLastTime;
	FLOAT mFrameCounter;
	FLOAT mUpdateDuration;
	INT mFrames;
	INT mRunCycle;

	CProfiler* mUpdateProfiler;
	CProfiler* mRenderProfiler;
	CProfiler* mSleepProfiler;
	CProfiler* mWindowProfiler;
};
