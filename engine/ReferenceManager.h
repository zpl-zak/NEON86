#pragma once

#include "system.h"

#include <vector>

class CReferenceCounter 
{
public:
    CReferenceCounter() :
        mRefCount(1)
    {

    }

    inline UINT GetRefCount() { return mRefCount; }

    inline VOID AddRef() { mRefCount++; }
    inline BOOL DelRef() { mRefCount--; return mRefCount == 0; }
    
private:
    UINT mRefCount;
};