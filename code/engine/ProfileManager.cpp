#include "StdAfx.h"

#include "ProfileManager.h"

CProfiler::CProfiler(CString name)
{
    mName = name;
    mStartTime = 0.0f;
    Reset();
}

VOID CProfiler::Reset()
{
    mNumInvocations = 0;
    mTotalTime = 0.0f;
}

VOID CProfiler::StartInvocation()
{
    mStartTime = GetTime();
}

VOID CProfiler::StopInvocation()
{
    mNumInvocations++;
    mTotalTime += (GetTime() - mStartTime);
    mStartTime = 0.0f;
}

FLOAT CProfiler::DisplayAndReset(FLOAT divisor, BOOL logStats)
{
    divisor = (divisor == 0.0f) ? (FLOAT)mNumInvocations : divisor;
    mDeltaTime = (mTotalTime == 0.0f && divisor == 0.0f) ? 0.0f : (1000.0f * mTotalTime) / divisor;
    Reset();

    if (logStats)
    {
        CString stats = CString("%s Time: %f ms", mName.Str(), mDeltaTime);
        OutputDebugStringA(stats.Str());
    }

    return mDeltaTime;
}
