#include "StdAfx.h"
#include "Scene.h"

#include "Mesh.h"
#include "Light.h"
#include "SceneLoader.h"

#include "Engine.h"
#include "VM.h"
#include "Renderer.h"
#include "FileSystem.h"

CScene::CScene(LPCSTR modelPath)
{
    mRootNode = nullptr;

    if (modelPath)
        LoadScene(modelPath);
}

void CScene::Release()
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

bool CScene::LoadScene(LPCSTR modelPath, bool loadMaterials, bool optimizeMesh)
{
    bool ok = CSceneLoader::LoadScene(modelPath, this, loadMaterials, optimizeMesh);

    if (!ok)
    {
        VM->PostError(CString::Format("Could not load model file: %s", modelPath));
        return FALSE;
    }

    mRootNode = mNodes[0];
    return TRUE;
}
