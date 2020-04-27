#pragma once

#include "system.h"
#include "RenderData.h"

class CMesh;

class CModel
{
public:
    CModel(LPSTR modelPath=NULL);
    void Release();

    void LoadModel(LPCSTR modelPath, BOOL loadMaterials = TRUE);
    void Draw(const D3DXMATRIX& wmat);

    inline UINT GetNumMeshes() { return mCount; }
    inline CMesh** GetMeshes() { return mMeshes; }
    CMesh* FindMesh(LPCSTR name);
private:
    CMesh** mMeshes;
    UINT mCapacity;
    UINT mCount;

    static D3DXMATRIX identityMat;

    void AddMesh(CMesh* mg);
};