#pragma once

#include "system.h"

#define VM CEngine::the()->GetVM()

enum PLAYKIND
{
	PLAYKIND_STOPPED,
	PLAYKIND_PLAYING,
	PLAYKIND_PAUSED,
};

struct lua_State;

class ENGINE_API CLuaMachine
{
public:
	CLuaMachine(VOID);
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

	inline VOID CheckVMErrors(INT);
private:
	UCHAR mPlayKind;
	UCHAR mScheduledTermination;
	UCHAR *mMainScript;
	lua_State* mLuaVM;

	VOID InitVM(VOID);
	VOID DestroyVM(VOID);
	inline VOID PrintVMError();
};
