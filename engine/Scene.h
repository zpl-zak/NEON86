#pragma once

#include "system.h"
#include "RenderData.h"

class CMesh;

class CScene
{
public:
    CScene(LPSTR modelPath=NULL);
    VOID Release();

    VOID LoadModel(LPCSTR modelPath, BOOL loadMaterials = TRUE);
    VOID Draw(const D3DXMATRIX& wmat);
    VOID DrawSubset(UINT subset, const D3DXMATRIX& wmat);

    inline UINT GetNumMeshes() { return mCount; }
    inline CMesh** GetMeshes() { return mMeshes; }
    CMesh* FindMesh(LPCSTR name);
    VOID AddMesh(CMesh* mg);

private:
    CMesh** mMeshes;
    UINT mCapacity;
    UINT mCount;

    static D3DXMATRIX identityMat;
};