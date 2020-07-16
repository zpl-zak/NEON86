#pragma once

#include "system.h"

class CNode;

class ENGINE_API CNodeComponent
{
public:
    CNodeComponent() 
    {
        mName = CString("(unknown)");
        mOwner = NULL;
    }

    virtual ~CNodeComponent() {}

    virtual LPCSTR GetKind() { return "Unknown"; }

    inline VOID SetName(CString name) { mName = name; }
    inline CString GetName() { return mName; }

    inline VOID SetOwner(CNode* node) { mOwner = node; }
    inline CNode* GetOwner() { return mOwner; }

private:
    CString mName;
    CNode* mOwner;
};
