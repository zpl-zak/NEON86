#include "StdAfx.h"

#include "ProfileManager.h"

CProfiler::CProfiler(CString name)
{
    mName = name;
    mStartTime = 0.0f;
    Reset();
}

void CProfiler::Reset()
{
    mNumInvocations = 0;
    mTotalTime = 0.0f;
}

void CProfiler::StartInvocation()
{
    mStartTime = GetTime();
}

void CProfiler::StopInvocation()
{
    mNumInvocations++;
    mTotalTime += GetTime() - mStartTime;
    mStartTime = 0.0f;
}

auto CProfiler::DisplayAndReset(float divisor, bool logStats) -> float
{
    divisor = divisor == 0.0f ? static_cast<float>(mNumInvocations) : divisor;
    mDeltaTime = mTotalTime == 0.0f && divisor == 0.0f ? 0.0f : 1000.0f * mTotalTime / divisor;
    Reset();

    if (logStats)
    {
        const auto stats = CString::Format("%s Time: %f ms", mName.Str(), mDeltaTime);
        PushLog(stats.Str(), TRUE);
    }

    return mDeltaTime;
}
