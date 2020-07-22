#pragma once

#include "system.h"

class ENGINE_API CProfiler {
public:
    CProfiler(CString name);

    VOID Reset();
    VOID StartInvocation();
    VOID StopInvocation();
    FLOAT DisplayAndReset(FLOAT divisor = 0, BOOL logStats = FALSE);

private:
    CString mName;
    INT mNumInvocations;
    FLOAT mStartTime;
    FLOAT mTotalTime;
};

enum {
    NEON_PROFILER_UPDATE,
    NEON_PROFILER_RENDER,
    NEON_PROFILER_WINDOW,
    NEON_PROFILER_SLEEP,
    MAX_NEON_PROFILERS
};
