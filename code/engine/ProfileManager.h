#pragma once

#include "system.h"

class ENGINE_API CProfiler
{
public:
    CProfiler(const CString& name);

    void Reset();
    void StartInvocation();
    void StopInvocation();
    auto DisplayAndReset(float divisor = 0, bool logStats = FALSE) -> float;

    auto GetDelta() const -> float { return mDeltaTime; }
    auto GetName() const -> CString { return mName; }
private:
    CString mName;
    int mNumInvocations;
    float mStartTime;
    float mDeltaTime;
    float mTotalTime;
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
