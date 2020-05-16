#include "StdAfx.h"
#include "Scene.h"

#include "Mesh.h"
#include "Light.h"
#include "SceneLoader.h"
#include "NeonEngine.h"
#include "FileSystem.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

CScene::CScene(LPSTR modelPath)
{
    Release();

    if (modelPath)
        LoadScene(modelPath);
}

VOID CScene::Release()
{
    mMeshes.Release();
    mLights.Release();
    mNodes.Release();
    mRootNode = NULL;
}

#define MESHIMPORT_FLAGS \
    aiProcess_ConvertToLeftHanded |\
    aiProcess_Triangulate |\
    aiProcess_CalcTangentSpace |\
    aiProcess_FlipUVs |\
    0

VOID CScene::LoadScene(LPCSTR modelPath, BOOL loadMaterials)
{
    Assimp::Importer imp;

    const aiScene* model = imp.ReadFile(FILESYSTEM->ResourcePath(RESOURCEKIND_USER, modelPath), MESHIMPORT_FLAGS);

    if (!model)
    {
        MessageBoxA(NULL, "Mesh import failed!", "Mesh error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

    CSceneLoader::LoadScene(model, this, loadMaterials);

    mRootNode = mNodes[0];
}

VOID CScene::Draw(const D3DXMATRIX& wmat)
{
    for (UINT i=0; i<mMeshes.GetCount();i++)
    {
        mMeshes[i]->Draw(wmat);
    }
}

VOID CScene::DrawSubset(UINT subset, const D3DXMATRIX& wmat)
{
    if (subset >= mMeshes.GetCount())
        return;

    mMeshes[subset]->Draw(wmat);
}
