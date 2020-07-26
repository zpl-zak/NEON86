#pragma once

#include "system.h"

class ENGINE_API CProfiler {
public:
    CProfiler(CString name);

    VOID Reset();
    VOID StartInvocation();
    VOID StopInvocation();
    FLOAT DisplayAndReset(FLOAT divisor = 0, BOOL logStats = FALSE);

    inline FLOAT GetDelta() { return mDeltaTime; }
    inline CString GetName() { return mName; }
private:
    CString mName;
    INT mNumInvocations;
    FLOAT mStartTime;
    FLOAT mDeltaTime;
    FLOAT mTotalTime;
};

class ENGINE_API CProfileScope {
public:
    CProfileScope(CProfiler* profile) {
        if (profile)
            profile->StartInvocation();
        mProfiler = profile;
    };

    ~CProfileScope() {
        if (mProfiler)
            mProfiler->StopInvocation();
    }

private:
    CProfiler* mProfiler;
};
