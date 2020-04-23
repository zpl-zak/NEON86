#include "stdafx.h"

#include "Mesh.h"
#include "FaceGroup.h"
#include "MeshLoader.h"
#include "NeonEngine.h"
#include "FileSystem.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

CMesh::CMesh(void)
{
    mCount = 0;
    mCapacity = 4;
    mMeshes = (CFaceGroup**)malloc(mCapacity * sizeof(CFaceGroup*));
    mTransforms = (D3DXMATRIX*)malloc(mCapacity * sizeof(D3DXMATRIX));
}

VOID CMesh::Release(void)
{
    SAFE_FREE(mMeshes);
    SAFE_FREE(mTransforms);
    mCount = mCapacity = 0;
}

VOID CMesh::AddMesh(CFaceGroup* mesh, const D3DXMATRIX& mat)
{
    if (!mesh)
        return;

    if (mCount >= mCapacity)
    {
        mCapacity += 4;

        mMeshes = (CFaceGroup**)realloc(mMeshes, mCapacity * sizeof(CFaceGroup*));
        mTransforms = (D3DXMATRIX*)realloc(mTransforms, mCapacity * sizeof(D3DXMATRIX));

        if (!mMeshes || !mTransforms)
        {
            MessageBoxA(NULL, "Can't add mesh to mesh group!", "Out of memory error", MB_OK);
            ENGINE->Shutdown();
            return;
        }
    }

    mMeshes[mCount] = mesh;
    mTransforms[mCount] = mat;
    mCount++;
}

VOID CMesh::Draw(const D3DXMATRIX& wmat)
{
    if (!mCount)
        return;

    for (UINT i = 0; i < mCount; i++)
    {
        D3DXMATRIX mat = mTransforms[i] * wmat;
        mMeshes[i]->Draw(&mat);
    }
}

VOID CMesh::Clear(void)
{
    mCount = 0;
}
