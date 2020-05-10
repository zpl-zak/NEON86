#include "StdAfx.h"
#include "Scene.h"

#include "Mesh.h"
#include "SceneLoader.h"
#include "NeonEngine.h"
#include "FileSystem.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

CScene::CScene(LPSTR modelPath)
{
    mCapacity = 4;
    mCount = 0;
    mMeshes = (CMesh**)neon_malloc(mCapacity * sizeof(CMesh*));
    D3DXMatrixIdentity(&identityMat);

    if (modelPath)
        LoadModel(modelPath);
}

VOID CScene::Release()
{
    SAFE_FREE(mMeshes);
    mCount = 0;
    mCapacity = 0;
}

#define MESHIMPORT_FLAGS \
    aiProcess_ConvertToLeftHanded |\
    aiProcess_Triangulate |\
    aiProcess_CalcTangentSpace |\
    aiProcess_FlipUVs

VOID CScene::LoadModel(LPCSTR modelPath, BOOL loadMaterials)
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

    /*CMesh* mesh = NULL;
    aiString meshName;

    for (UINT i = 0; i < model->mNumMeshes; i++)
    {
        aiMesh* m = model->mMeshes[i];
        if (!mesh || strcmp(meshName.C_Str(), m->mName.C_Str()))
        {
            if (mesh)
                AddMesh(mesh);

            mesh = new CMesh();
            CReferenceManager::TrackRef(mesh);
            
            meshName = m->mName;
            mesh->SetName(meshName);
        }

        CFaceGroup* node = CSceneLoader::LoadNode(model, m, loadMaterials);
        mesh->AddMesh(node, *(D3DMATRIX*)&model->mRootNode->mTransformation);
    }

    AddMesh(mesh);*/
}

VOID CScene::Draw(const D3DXMATRIX& wmat)
{
    if (!mMeshes)
        return;

    for (UINT i=0; i<mCount;i++)
    {
        mMeshes[i]->Draw(wmat);
    }
}

VOID CScene::DrawSubset(UINT subset, const D3DXMATRIX& wmat)
{
    if (!mMeshes)
        return;

    if (subset >= mCount)
        return;

    mMeshes[subset]->Draw(wmat);
}

CMesh* CScene::FindMesh(LPCSTR name)
{
    for (UINT i = 0; i < mCount; i++)
    {
        if (!strcmp(name, mMeshes[i]->GetName().C_Str()))
            return mMeshes[i];
    }

    return NULL;
}

VOID CScene::AddMesh(CMesh* mg)
{
    if (!mg)
        return;

    if (mCount >= mCapacity)
    {
        mCapacity += 4;

        mMeshes = (CMesh**)neon_realloc(mMeshes, mCapacity * sizeof(CMesh*));
        
        if (!mMeshes)
        {
            MessageBoxA(NULL, "Can't add mesh group to model!", "Out of memory error", MB_OK);
            ENGINE->Shutdown();
            return;
        }
    }

    mMeshes[mCount++] = mg;
}

D3DXMATRIX CScene::identityMat;