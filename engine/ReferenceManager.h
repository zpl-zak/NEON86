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

    inline INT GetRefCount() { return mRefCount; }

    inline VOID AddRef() { mRefCount++; }
    inline BOOL DelRef() { mRefCount--; return mRefCount == 0; }
    
private:
    INT mRefCount;
};