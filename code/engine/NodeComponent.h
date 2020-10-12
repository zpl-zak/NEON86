#pragma once

#include "system.h"

class CNode;

class ENGINE_API CNodeComponent
{
public:
    CNodeComponent()
    {
        mName = "(unknown)";
        mOwner = nullptr;
    }

    virtual ~CNodeComponent()
    = default;

    virtual auto GetKind() -> LPCSTR { return "Unknown"; }

    VOID SetName(CString name) { mName = name; }
    auto GetName() const -> CString { return mName; }

    VOID SetOwner(CNode* node) { mOwner = node; }
    auto GetOwner() const -> CNode* { return mOwner; }

private:
    CString mName;
    CNode* mOwner;
};
