#include "stdafx.h"

#include "Mesh.h"
#include "Node.h"
#include "FaceGroup.h"

#include "Engine.h"
#include "Renderer.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

CMesh::CMesh() = default;

CMesh::~CMesh()
{
    Release();
}

void CMesh::Release()
{
    if (DelRef())
    {
        for (auto& a : mFaceGroups)
        {
            a->Release();
        }

        mFaceGroups.Release();
        mTransforms.Release();

        delete this;
    }
}

void CMesh::AddFaceGroup(CFaceGroup* mesh, const D3DXMATRIX& mat)
{
    if (mesh == nullptr)
    {
        return;
    }

    if (FAILED(mFaceGroups.Push(mesh)))
    {
        MessageBoxA(nullptr, "Can't add face group to mesh!", "Out of memory error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

    if (FAILED(mTransforms.Push(mat)))
    {
        MessageBoxA(nullptr, "Can't add transform to mesh!", "Out of memory error", MB_OK);
        ENGINE->Shutdown();
    }
}

auto CMesh::Clone() -> CMesh*
{
    auto* clonedMesh = new CMesh();
    unsigned int i = 0;

    for (auto* fg : mFaceGroups)
    {
        clonedMesh->AddFaceGroup(fg->Clone(), mTransforms[i++]);
    }

    return clonedMesh;
}

void CMesh::Draw(const D3DXMATRIX& wmat) const
{
    for (unsigned int i = 0; i < mFaceGroups.GetCount(); i++)
    {
        auto mat = (GetOwner() != nullptr ? mTransforms[i] * GetOwner()->GetFinalTransform() : mTransforms[i]) *
                wmat;
        mFaceGroups[i]->Draw(&mat);
    }
}

void CMesh::Clear()
{
    mFaceGroups.Clear();
    mTransforms.Clear();
}
