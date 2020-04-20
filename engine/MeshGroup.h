#pragma once

#include "system.h"
#include "RenderData.h"
#include "ReferenceManager.h"

#include <Assimp/Importer.hpp>

class CMesh;

class CMeshGroup: public CReference<CMeshGroup>
{
public:
    CMeshGroup(void);
     
    VOID Release(void);
    VOID AddMesh(CMesh*, const D3DXMATRIX&);
    
    VOID Draw(const D3DXMATRIX& wmat);
    VOID Clear(void);

    inline UINT GetNumMeshes() const { return mCount; }
    inline CMesh** GetMeshes() { return mMeshes; }
    inline D3DXMATRIX* GetTransforms() { return mTransforms; }

    inline aiString GetName() { return mName; }
    inline void SetName(aiString name) { mName = name; }

private:
    CMesh** mMeshes;
    D3DXMATRIX* mTransforms;
    UINT mCount;
    UINT mCapacity;
    aiString mName;
};
