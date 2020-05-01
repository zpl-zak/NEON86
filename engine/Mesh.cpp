#include "stdafx.h"

#include "Mesh.h"
#include "FaceGroup.h"
#include "MeshLoader.h"
#include "NeonEngine.h"
#include "FileSystem.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

CMesh::CMesh(VOID)
{
    mCount = 0;
    mCapacity = 4;
    mFaceGroups = (CFaceGroup**)malloc(mCapacity * sizeof(CFaceGroup*));
    mTransforms = (D3DXMATRIX*)malloc(mCapacity * sizeof(D3DXMATRIX));
}

VOID CMesh::Release(VOID)
{
    SAFE_FREE(mFaceGroups);
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

        mFaceGroups = (CFaceGroup**)realloc(mFaceGroups, mCapacity * sizeof(CFaceGroup*));
        mTransforms = (D3DXMATRIX*)realloc(mTransforms, mCapacity * sizeof(D3DXMATRIX));

        if (!mFaceGroups || !mTransforms)
        {
            MessageBoxA(NULL, "Can't add mesh to mesh group!", "Out of memory error", MB_OK);
            ENGINE->Shutdown();
            return;
        }
    }

    mFaceGroups[mCount] = mesh;
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
        mFaceGroups[i]->Draw(&mat);
    }
}

VOID CMesh::Clear(VOID)
{
    mCount = 0;
}
