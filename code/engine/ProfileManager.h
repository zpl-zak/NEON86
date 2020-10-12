#pragma once

#include "system.h"

class ENGINE_API CProfiler
{
public:
    CProfiler(CString name);

    VOID Reset();
    VOID StartInvocation();
    VOID StopInvocation();
    auto DisplayAndReset(FLOAT divisor = 0, BOOL logStats = FALSE) -> FLOAT;

    auto GetDelta() const -> FLOAT { return mDeltaTime; }
    auto GetName() const -> CString { return mName; }
private:
    CString mName;
    INT mNumInvocations;
    FLOAT mStartTime;
    FLOAT mDeltaTime;
    FLOAT mTotalTime;
};

class ENGINE_API CProfileScope
{
public:
    CProfileScope(CProfiler* profile)
    {
        if (profile)
            profile->StartInvocation();
        mProfiler = profile;
    }

    CProfileScope(CProfileScope&) = delete;
    CProfileScope(CProfileScope&&) = delete;

    ~CProfileScope()
    {
        if (mProfiler)
            mProfiler->StopInvocation();
    }

private:
    CProfiler* mProfiler;
};
