#pragma once

#include "system.h"
#include "Node.h"
#include "ReferenceManager.h"
#include <assimp/matrix4x4.h>

#include <d3dx9.h>

class CTarget: public CNode, public CReference<CTarget>
{
public:
    CTarget(aiMatrix4x4 mat, aiString name)
    {
        SetName(name);
        mTransform = *(D3DXMATRIX*)&mat;
    }

    VOID Release() {}

    ~CTarget() {}

    inline D3DXMATRIX GetTransform() { return mTransform; }

private:
    D3DXMATRIX mTransform;
};