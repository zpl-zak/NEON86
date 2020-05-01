#include "system.h"

class CRenderer;
class CInput;
class CFileSystem;
class CLuaMachine;
class CUserInterface;

#define ENGINE CEngine::the()

class ENGINE_API CEngine {
public:
	CEngine(VOID);

	static CEngine* the();

	BOOL Init(HWND window, RECT resolution);
	BOOL Release();
	VOID Shutdown();
	VOID Resize(RECT resolution);
	VOID Think();
	VOID ResetApplicationTime();
	LRESULT ProcessEvents(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	CRenderer* GetRenderer() { return mRenderer; }
	CInput* GetInput() { return mInput; }
	CFileSystem* GetFileSystem() { return mFileSystem; }
	CLuaMachine* GetVM() { return mLuaMachine; }
	CUserInterface* GetUI() { return mUserInterface; }

	BOOL IsRunning() const { return mIsRunning; }

	inline VOID SetFPS(FLOAT fps) { if (fps) mUpdateDuration = 1.0f / fps; } 
	inline FLOAT GetFPS() const { return 1.0f / mUpdateDuration; }
protected:
	CRenderer* mRenderer;
	CInput* mInput;
	CFileSystem* mFileSystem;
	CLuaMachine* mLuaMachine;
	CUserInterface* mUserInterface;
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
