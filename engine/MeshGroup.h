#pragma once

#include "system.h"
#include "Mesh.h"

class CMeshGroup
{
public:
    CMeshGroup(void);

    VOID Release(void);
    VOID AddMesh(CMesh*, const D3DXMATRIX&);
    VOID Draw(const D3DXMATRIX& wmat);
    VOID Clear(void);

private:
    CMesh** mMeshes;
    D3DXMATRIX* mTransforms;
    UINT mCount;
    UINT mCapacity;
};
