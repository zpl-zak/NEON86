#pragma once

#include "system.h"

#include <Assimp/Importer.hpp>

class CNode;

class ENGINE_API CNodeComponent
{
public:
    CNodeComponent() 
    {
        mName = new aiString("(unknown)");
        mOwner = NULL;
    }

    virtual ~CNodeComponent() {
        SAFE_DELETE(mName);
    }

    virtual LPCSTR GetKind() { return "Unknown"; }

    inline VOID SetName(aiString name) { mName = new aiString(name); }
    inline aiString GetName() { return *mName; }

    inline VOID SetOwner(CNode* node) { mOwner = node; }
    inline CNode* GetOwner() { return mOwner; }

private:
    aiString* mName;
    CNode* mOwner;
};
