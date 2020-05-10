#include "stdafx.h"

#include "Mesh.h"
#include "FaceGroup.h"
#include "SceneLoader.h"
#include "NeonEngine.h"
#include "FileSystem.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

CMesh::CMesh(VOID)
{
    mFaceGroups.Release();
    mTransforms.Release();
}

VOID CMesh::Release(VOID)
{

}

VOID CMesh::AddFaceGroup(CFaceGroup* mesh, const D3DXMATRIX& mat)
{
    if (!mesh)
        return;

    if (FAILED(mFaceGroups.Push(mesh)))
    {
        MessageBoxA(NULL, "Can't add face group to mesh!", "Out of memory error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

    if (FAILED(mTransforms.Push(mat)))
    {
        MessageBoxA(NULL, "Can't add transform to mesh!", "Out of memory error", MB_OK);
        ENGINE->Shutdown();
        return;
    }
}

VOID CMesh::Draw(const D3DXMATRIX& wmat)
{
    for (UINT i = 0; i < mFaceGroups.GetCount(); i++)
    {
        D3DXMATRIX mat = mTransforms[i] * wmat;
        mFaceGroups[i]->Draw(&mat);
    }
}

VOID CMesh::Clear(VOID)
{
    mFaceGroups.Clear();
    mTransforms.Clear();
}
