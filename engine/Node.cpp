#include "stdafx.h"
#include "Node.h"
#include "Mesh.h"
#include "Light.h"
#include "NeonEngine.h"

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
            delete a;

        for (auto& a : mLights)
            delete a;

        for (auto& a : mNodes)
            delete a;
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