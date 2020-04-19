#pragma once

#include "system.h"
#include "RenderData.h"

class CMesh;
struct aiScene;
struct aiMesh;

class CMeshGroup
{
public:
    CMeshGroup(void);
     
    VOID Release(void);
    VOID AddMesh(CMesh*, const D3DXMATRIX&);
    VOID LoadMesh(LPCSTR modelName, UINT texFiltering = TEXF_ANISOTROPIC);
    
    VOID Draw(const D3DXMATRIX& wmat);
    VOID Clear(void);

private:
    CMesh** mMeshes;
    D3DXMATRIX* mTransforms;
    D3DXMATRIX identityMat;
    UINT mCount;
    UINT mCapacity;

    CMesh* LoadNode(const aiScene* scene, const aiMesh* mesh, UINT texFiltering);
};
