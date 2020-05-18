#include "stdafx.h"
#include "Node.h"
#include "Mesh.h"
#include "Light.h"

#include "Engine.h"
#include "Renderer.h"

CMesh* CNode::FindMesh(LPCSTR name)
{
    return mMeshes.Find(name);
}

VOID CNode::AddMesh(CMesh* mg)
{
    if (!mg)
        return;

    if (FAILED(mMeshes.Push(mg)))
    {
        MessageBoxA(NULL, "Can't push mesh into a scene!", "Out of memory error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

    mg->SetOwner(this);
}

CLight* CNode::FindLight(LPCSTR name)
{
    return mLights.Find(name);
}

VOID CNode::AddLight(CLight* lit)
{
    if (!lit)
        return;

    if (FAILED(mLights.Push(lit)))
    {
        MessageBoxA(NULL, "Can't push light into a scene!", "Out of memory error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

    lit->SetOwner(this);
}

CNode* CNode::FindNode(LPCSTR name)
{
    if (!name)
        return NULL;

    return mNodes.Find(name);
}

VOID CNode::AddNode(CNode* tgt)
{
    if (FAILED(mNodes.Push(tgt)))
    {
        MessageBoxA(NULL, "Can't push node into a scene!", "Out of memory error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

    tgt->SetOwner(this);
}

CNode* CNode::Clone()
{
    CNode* clonedNode = new CNode();

    clonedNode->mMetadata = mMetadata;
    clonedNode->SetName(GetName());

    for (auto a : mMeshes)
        clonedNode->AddMesh(a->Clone());

    for (auto a : mLights)
        clonedNode->AddLight(a->Clone());

    for (auto a : mNodes)
    {
        auto b = a->Clone();
        b->SetParent(clonedNode);
        clonedNode->AddNode(b);
    }

    clonedNode->SetTransform(mTransform);

    return clonedNode;
}

BOOL CNode::IsEmpty()
{
    return (mMeshes.GetCount() == mLights.GetCount() == mNodes.GetCount() == 0);
}

D3DXMATRIX CNode::GetFinalTransform()
{
    if (!mIsTransformDirty)
        return mCachedTransform;

    if (GetParent() == NULL)
        return mTransform;

    mCachedTransform = mTransform * GetParent()->GetFinalTransform();
    mIsTransformDirty = FALSE;

    return mCachedTransform;
}

VOID CNode::InvalidateTransformRecursively()
{
    InvalidateTransform();
    
    for (UINT i=0; i<mNodes.GetCount(); ++i)
    {
        mNodes[i]->InvalidateTransformRecursively();
    }
}

VOID CNode::Release()
{
    if (DelRef())
    {
        for (auto& a : mMeshes)
            a->Release();

        for (auto& a : mLights)
            a->Release();

        for (auto& a : mNodes)
            a->Release();

        mMeshes.Release();
        mLights.Release();
        mNodes.Release();
    }
}

VOID CNode::Draw(const D3DXMATRIX& wmat)
{
    for (UINT i = 0; i < mMeshes.GetCount(); i++)
    {
        mMeshes[i]->Draw(wmat);
    }

    for (UINT i = 0; i < mNodes.GetCount(); i++)
    {
        mNodes[i]->Draw(wmat);
    }
}

VOID CNode::DrawSubset(UINT subset, const D3DXMATRIX& wmat)
{
    if (subset >= mMeshes.GetCount())
        return;

    mMeshes[subset]->Draw(wmat);
}
