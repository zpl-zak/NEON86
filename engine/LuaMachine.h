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
	CLuaMachine(void);
	VOID Release(void);

	/// States
	VOID Play(void);
	VOID Pause(void);
	VOID Stop(void);
	VOID Restart(void);

	/// Events
	VOID Init(void);
	VOID Destroy(void);
	VOID Update(FLOAT dt);
	VOID Render(void);
	VOID Resize(RECT res);

private:
	UCHAR mPlayKind;
	UCHAR *mMainScript;
	lua_State* mLuaVM;

	VOID InitVM(void);
	VOID DestroyVM(void);
	inline void PrintVMError();
	inline void CheckVMErrors(INT);
};
