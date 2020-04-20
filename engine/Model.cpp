#include "StdAfx.h"
#include "Model.h"

#include "MeshGroup.h"
#include "MeshLoader.h"
#include "NeonEngine.h"
#include "FileSystem.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

CModel::CModel(LPSTR modelPath)
{
    mCapacity = 4;
    mCount = 0;
    mMeshGroups = (CMeshGroup**)malloc(mCapacity * sizeof(CMeshGroup*));
    D3DXMatrixIdentity(&identityMat);

    if (modelPath)
        LoadModel(modelPath);
}

void CModel::Release()
{
    SAFE_FREE(mMeshGroups);
    mCount = 0;
    mCapacity = 0;
}

#define MESHIMPORT_FLAGS \
    aiProcess_ConvertToLeftHanded |\
    aiProcess_Triangulate |\
    aiProcess_CalcTangentSpace |\
    aiProcess_FlipUVs |\
    aiProcess_JoinIdenticalVertices |\
    aiProcess_PreTransformVertices

void CModel::LoadModel(LPCSTR modelPath, UINT texFiltering)
{
    Assimp::Importer imp;

    const aiScene* model = imp.ReadFile(FILESYSTEM->ResourcePath(RESOURCEKIND_USER, modelPath), MESHIMPORT_FLAGS);

    if (!model)
    {
        MessageBoxA(NULL, "Mesh import failed!", "Mesh error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

    CMeshGroup* meshGroup = NULL;
    aiString meshName;

    for (UINT i = 0; i < model->mNumMeshes; i++)
    {
        aiMesh* m = model->mMeshes[i];
        if (!meshGroup || strcmp(meshName.C_Str(), m->mName.C_Str()))
        {
            if (meshGroup)
                AddMeshGroup(meshGroup);

            meshGroup = new CMeshGroup();
            CReferenceManager::TrackRef(meshGroup);
            
            meshName = m->mName;
            meshGroup->SetName(meshName);
        }

        CMesh* mesh = CMeshLoader::LoadNode(model, m, texFiltering);
        meshGroup->AddMesh(mesh, identityMat);
    }

    AddMeshGroup(meshGroup);
}

void CModel::Draw(const D3DXMATRIX& wmat)
{
    if (!mMeshGroups)
        return;

    for (UINT i=0; i<mCount;i++)
    {
        mMeshGroups[i]->Draw(wmat);
    }
}

CMeshGroup* CModel::FindMeshGroup(LPCSTR name)
{
    for (UINT i = 0; i < mCount; i++)
    {
        if (!strcmp(name, mMeshGroups[i]->GetName().C_Str()))
            return mMeshGroups[i];
    }

    return NULL;
}

void CModel::AddMeshGroup(CMeshGroup* mg)
{
    if (!mg)
        return;

    if (mCount >= mCapacity)
    {
        mCapacity += 4;

        mMeshGroups = (CMeshGroup**)realloc(mMeshGroups, mCapacity * sizeof(CMeshGroup*));
        
        if (!mMeshGroups )
        {
            MessageBoxA(NULL, "Can't add mesh group to model!", "Out of memory error", MB_OK);
            ENGINE->Shutdown();
            return;
        }
    }

    mMeshGroups[mCount++] = mg;
}

D3DXMATRIX CModel::identityMat;