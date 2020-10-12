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

void CNode::AddMesh(CMesh* mg)
{
    if (!mg)
        return;

    if (FAILED(mMeshes.Push(mg)))
    {
        MessageBoxA(nullptr, "Can't push mesh into a scene!", "Out of memory error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

    mg->SetOwner(this);
}

CLight* CNode::FindLight(LPCSTR name)
{
    return mLights.Find(name);
}

void CNode::AddLight(CLight* lit)
{
    if (!lit)
        return;

    if (FAILED(mLights.Push(lit)))
    {
        MessageBoxA(nullptr, "Can't push light into a scene!", "Out of memory error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

    lit->SetOwner(this);
}

CNode* CNode::FindNode(LPCSTR name)
{
    if (!name)
        return nullptr;

    return mNodes.Find(name);
}

void CNode::AddNode(CNode* tgt)
{
    if (FAILED(mNodes.Push(tgt)))
    {
        MessageBoxA(nullptr, "Can't push node into a scene!", "Out of memory error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

    tgt->SetOwner(this);
}

CNode* CNode::Clone()
{
    CNode* clonedNode = new CNode();

    *clonedNode->mMetadata = *mMetadata;
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

    clonedNode->SetTransform(*mTransform);

    return clonedNode;
}

bool CNode::IsEmpty()
{
    return (mMeshes.GetCount() == mLights.GetCount() == mNodes.GetCount() == 0);
}

D3DXMATRIX CNode::GetFinalTransform()
{
    if (!mIsTransformDirty)
        return *mCachedTransform;

    if (GetParent() == nullptr)
        return *mTransform;

    *mCachedTransform = *mTransform * GetParent()->GetFinalTransform();
    mIsTransformDirty = FALSE;

    return *mCachedTransform;
}

void CNode::InvalidateTransformRecursively()
{
    InvalidateTransform();

    for (unsigned int i = 0; i < mNodes.GetCount(); ++i)
    {
        mNodes[i]->InvalidateTransformRecursively();
    }
}

void CNode::Release()
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

        SAFE_DELETE(mTransform);
        SAFE_DELETE(mCachedTransform);
        SAFE_DELETE(mMetadata);

        delete this;
    }
}

void CNode::Draw(const D3DXMATRIX& wmat)
{
    for (unsigned int i = 0; i < mMeshes.GetCount(); i++)
    {
        mMeshes[i]->Draw(wmat);
    }

    for (unsigned int i = 0; i < mNodes.GetCount(); i++)
    {
        mNodes[i]->Draw(wmat);
    }
}

void CNode::DrawSubset(unsigned int subset, const D3DXMATRIX& wmat)
{
    if (subset >= mMeshes.GetCount())
        return;

    mMeshes[subset]->Draw(wmat);
}
