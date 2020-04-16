#include "stdafx.h"

#include "MeshGroup.h"
#include "NeonEngine.h"

CMeshGroup::CMeshGroup(void)
{
    mCount = 0;
    mCapacity = 4;
    mMeshes = (CMesh**)malloc(mCapacity * sizeof(CMesh*));
    mTransforms = (D3DXMATRIX*)malloc(mCapacity * sizeof(D3DXMATRIX));
}

VOID CMeshGroup::Release(void)
{
    SAFE_FREE(mMeshes);
    SAFE_FREE(mTransforms);
    mCount = mCapacity = 0;
}

VOID CMeshGroup::AddMesh(CMesh* mesh, const D3DXMATRIX& mat)
{
    if (!mesh)
        return;

    if (mCount >= mCapacity)
    {
        mCapacity += 4;

        mMeshes = (CMesh**)realloc(mMeshes, mCapacity * sizeof(CMesh*));
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

VOID CMeshGroup::Draw(const D3DXMATRIX& wmat)
{
    if (!mCount)
        return;

    for (UINT i = 0; i < mCount; i++)
    {
        D3DXMATRIX mat = mTransforms[i] * wmat;
        mMeshes[i]->Draw(&mat);
    }
}

VOID CMeshGroup::Clear(void)
{
    mCount = 0;
}
