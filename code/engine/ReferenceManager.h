#pragma once

#include "system.h"

class ENGINE_API CReferenceCounter
{
public:
    CReferenceCounter() = default;

    auto GetRefCount() const -> INT { return mRefCount; }

    VOID AddRef() { mRefCount++; }

    auto DelRef() -> BOOL
    {
        mRefCount--;
        return mRefCount == 0;
    }

private:
    INT mRefCount{1};
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
