#include "StdAfx.h"
#include "Scene.h"

#include "Mesh.h"
#include "Light.h"
#include "SceneLoader.h"

#include "Engine.h"
#include "VM.h"
#include "Renderer.h"
#include "FileSystem.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

CScene::CScene(LPSTR modelPath)
{
    mRootNode = NULL;

    if (modelPath)
        LoadScene(modelPath);
}

VOID CScene::Release()
{
    if (DelRef())
    {
        SAFE_RELEASE(mRootNode);

        mMeshes.Release();
        mLights.Release();
        mNodes.Release();

        delete this;
    }
}

#define MESHIMPORT_FLAGS \
    aiProcess_ConvertToLeftHanded |\
    aiProcess_Triangulate |\
    aiProcess_CalcTangentSpace |\
    aiProcess_FlipUVs |\
    aiProcess_SplitLargeMeshes |\
    0

BOOL CScene::LoadScene(LPCSTR modelPath, BOOL loadMaterials, BOOL optimizeMesh)
{
    Assimp::Importer imp;
    imp.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, 32762);

    DWORD meshFlags = MESHIMPORT_FLAGS;

    if (optimizeMesh)
    {
        meshFlags |= aiProcess_PreTransformVertices 
                    | aiProcess_RemoveRedundantMaterials
                    | aiProcess_Triangulate;
    }

    const aiScene* model = imp.ReadFile(FILESYSTEM->ResourcePath(RESOURCEKIND_USER, modelPath), meshFlags);

    if (!model)
    {
        VM->PostError(std::string("Could not load model file: ") + modelPath);
        return FALSE;
    }

    CSceneLoader::LoadScene(model, this, loadMaterials, optimizeMesh);

    mRootNode = mNodes[0];
    return TRUE;
}

