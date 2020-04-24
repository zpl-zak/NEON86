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
    inline CMesh** GetMeshes() { return mMeshGroups; }
    CMesh* FindMeshGroup(LPCSTR name);
private:
    CMesh** mMeshGroups;
    UINT mCapacity;
    UINT mCount;

    static D3DXMATRIX identityMat;

    void AddMeshGroup(CMesh* mg);
};