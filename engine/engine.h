#include "system.h"

class CRenderer;
class CInput;
class CFileSystem;
class CLuaMachine;

#define ENGINE CEngine::the()

class ENGINE_API CEngine {
public:
	CEngine(void);

	static CEngine* the();

	BOOL Init(HWND window, RECT resolution);
	BOOL Release();
	VOID Shutdown();
	VOID Resize(RECT resolution);
	VOID Think();
	
	BOOL Fullscreen(BOOL fullscreen);

	CRenderer* GetRenderer() { return mRenderer; }
	CInput* GetInput() { return mInput; }
	CFileSystem* GetFileSystem() { return mFileSystem; }
	CLuaMachine* GetVM() { return mLuaMachine; }

	BOOL IsRunning() const { return mIsRunning; }

	inline VOID SetFPS(FLOAT fps) { if (fps) mUpdateDuration = 1.0f / fps; } 
	inline FLOAT GetFPS() const { return 1.0f / mUpdateDuration; }
protected:
	CRenderer* mRenderer;
	CInput* mInput;
	CFileSystem* mFileSystem;
	CLuaMachine* mLuaMachine;
private:
	VOID Update(FLOAT deltaTime);
	VOID Render();

	static CEngine *sInstance;
	BOOL mIsInitialised;
	BOOL mIsRunning;
	FLOAT mUnprocessedTime;
	FLOAT mLastTime;
	FLOAT mUpdateDuration;
};
