#pragma once

#include "system.h"

class ENGINE_API CReferenceCounter
{
public:
    CReferenceCounter() :
        mRefCount(1)
    {

    }

    inline INT GetRefCount() { return mRefCount; }

    inline VOID AddRef() { mRefCount++; }
    inline BOOL DelRef() { mRefCount--; return mRefCount == 0; }

private:
    INT mRefCount;
};

template <typename T>
class ENGINE_API CAllocable
{
public:
    CAllocable() { ++gResourceCount; gMemUsed += sizeof(T); neon_mempeak_update(); }
    ~CAllocable() { --gResourceCount; gMemUsed -= sizeof(T); }
};
