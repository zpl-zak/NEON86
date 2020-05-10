#pragma once

#include "system.h"

#include <Assimp/Importer.hpp>

class CNode
{
public:
    CNode() {}
    virtual ~CNode() {}

    inline VOID SetName(aiString name) { mName = name; }
    inline aiString GetName() { return mName; }

private:
    aiString mName;
};
