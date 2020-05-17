#pragma once

#include "system.h"

#include <Assimp/Importer.hpp>

class CNode;

class CNodeComponent
{
public:
    CNodeComponent() 
    {
        mName = aiString("(unknown)");
        mOwner = NULL;
    }

    virtual ~CNodeComponent() {}

    inline VOID SetName(aiString name) { mName = name; }
    inline aiString GetName() { return mName; }

    inline VOID SetOwner(CNode* node) { mOwner = node; }
    inline CNode* GetOwner() { return mOwner; }

private:
    aiString mName;
    CNode* mOwner;
};
