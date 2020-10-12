#include "StdAfx.h"

#include "ProfileManager.h"

CProfiler::CProfiler(const CString& name)
{
    mName = name;
    mStartTime = 0.0F;
    Reset();
}

void CProfiler::Reset()
{
    mNumInvocations = 0;
    mTotalTime = 0.0F;
}

void CProfiler::StartInvocation()
{
    mStartTime = GetTime();
}

void CProfiler::StopInvocation()
{
    mNumInvocations++;
    mTotalTime += GetTime() - mStartTime;
    mStartTime = 0.0F;
}

auto CProfiler::DisplayAndReset(float divisor, bool logStats) -> float
{
    divisor = divisor == 0.0F ? static_cast<float>(mNumInvocations) : divisor;
    mDeltaTime = mTotalTime == 0.0F && divisor == 0.0F ? 0.0F : 1000.0F * mTotalTime / divisor;
    Reset();

    if (logStats)
    {
        const auto stats = CString::Format("%s Time: %f ms", mName.Str(), mDeltaTime);
        PushLog(stats.Str(), TRUE);
    }

    return mDeltaTime;
}
