#pragma once

#include "system.h"
#include "RenderData.h"

class CMeshGroup;

class CModel
{
public:
    CModel(LPSTR modelPath=NULL);
    void Release();

    void LoadModel(LPCSTR modelPath, UINT texFiltering = TEXF_ANISOTROPIC);
    void Draw(const D3DXMATRIX& wmat);

    inline UINT GetNumMeshGroups() { return mCount; }
    inline CMeshGroup** GetMeshGroups() { return mMeshGroups; }
    CMeshGroup* FindMeshGroup(LPCSTR name);
private:
    CMeshGroup** mMeshGroups;
    UINT mCapacity;
    UINT mCount;

    static D3DXMATRIX identityMat;

    void AddMeshGroup(CMeshGroup* mg);
};