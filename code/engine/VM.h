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

class ENGINE_API CVirtualMachine
{
public:
    CVirtualMachine(void);
    void Release(void);

    /// States
    void Play(void);
    void Pause(void);
    void Stop(void);
    void Restart(void);

    /// Events
    void Init(void);
    void Destroy(void);
    void Update(float dt);
    void Render(void);
    void Render2D(void);
    void CharInput(DWORD key);
    void Resize(RECT res);

    inline auto CheckVMErrors(int, bool canFail = FALSE) -> bool;
    inline void PostError(LPCSTR err);
    inline void PostError(CString err);
    auto GetRunTime() const -> float { return mRunTime; }
    void PassTime(float dt) { mRunTime += dt; }
    auto GetStatus() const -> UCHAR { return mPlayKind; }
private:
    UCHAR mPlayKind;
    UCHAR mScheduledTermination;
    UCHAR* mMainScript;
    lua_State* mLuaVM;
    float mRunTime;

    void InitVM(void);
    void DestroyVM(void);
    inline void PrintVMError() const;
};
