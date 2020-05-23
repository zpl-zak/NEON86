#pragma once

#include "system.h"

#include <string>

#define VM CEngine::the()->GetVM()

enum PLAYKIND
{
	PLAYKIND_STOPPED,
	PLAYKIND_PLAYING,
	PLAYKIND_PAUSED,
};

struct lua_State;

class ENGINE_API CVirtualMachine
{
public:
	CVirtualMachine(VOID);
	VOID Release(VOID);

	/// States
	VOID Play(VOID);
	VOID Pause(VOID);
	VOID Stop(VOID);
	VOID Restart(VOID);

	/// Events
	VOID Init(VOID);
	VOID Destroy(VOID);
	VOID Update(FLOAT dt);
	VOID Render(VOID);
	VOID Render2D(VOID);
	VOID Resize(RECT res);

	inline BOOL CheckVMErrors(INT, BOOL canFail=FALSE);
	inline VOID PostError(LPCSTR err);
	inline VOID PostError(std::string err);
	inline FLOAT GetRunTime() { return mRunTime; }
private:
	UCHAR mPlayKind;
	UCHAR mScheduledTermination;
	UCHAR *mMainScript;
	lua_State* mLuaVM;
	FLOAT mRunTime;

	VOID InitVM(VOID);
	VOID DestroyVM(VOID);
	inline VOID PrintVMError();
};