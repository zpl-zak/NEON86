#pragma once

#include "system.h"

class ENGINE_API CReferenceCounter
{
public:
    CReferenceCounter() = default;

    auto GetRefCount() const -> int { return mRefCount; }

    void AddRef() { mRefCount++; }

    auto DelRef() -> bool
    {
        mRefCount--;
        return mRefCount == 0;
    }

private:
    int mRefCount{1};
};

template <typename T>
class ENGINE_API CAllocable
{
public:
    CAllocable()
    {
        ++gResourceCount;
        gMemUsed += sizeof(T);
        neon_mempeak_update();
    }

    ~CAllocable()
    {
        --gResourceCount;
        gMemUsed -= sizeof(T);
    }
};
