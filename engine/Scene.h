#pragma once

#include "system.h"
#include "RenderData.h"

class CMesh;
class CLight;

class CScene
{
public:
    CScene(LPSTR modelPath=NULL);
    VOID Release();

    VOID LoadScene(LPCSTR modelPath, BOOL loadMaterials = TRUE);
    VOID Draw(const D3DXMATRIX& wmat);
    VOID DrawSubset(UINT subset, const D3DXMATRIX& wmat);

    inline UINT GetNumMeshes() { return mMeshes.GetCount(); }
    inline CMesh** GetMeshes() { return mMeshes.GetData(); }
    CMesh* FindMesh(LPCSTR name);
    VOID AddMesh(CMesh* mg);

    inline UINT GetNumLights() { return mLights.GetCount(); }
    inline CLight** GetLights() { return mLights.GetData(); }
    CLight* FindLight(LPCSTR name);
    VOID AddLight(CLight* lit);

private:
    CArray<CMesh*> mMeshes;
    CArray<CLight*> mLights;
};