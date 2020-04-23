#pragma once

#include "system.h"
#include "RenderData.h"

class CMesh;

class CModel
{
public:
    CModel(LPSTR modelPath=NULL);
    void Release();

    void LoadModel(LPCSTR modelPath, UINT texFiltering = TEXF_ANISOTROPIC);
    void Draw(const D3DXMATRIX& wmat);

    inline UINT GetNumMeshGroups() { return mCount; }
    inline CMesh** GetMeshGroups() { return mMeshGroups; }
    CMesh* FindMeshGroup(LPCSTR name);
private:
    CMesh** mMeshGroups;
    UINT mCapacity;
    UINT mCount;

    static D3DXMATRIX identityMat;

    void AddMeshGroup(CMesh* mg);
};