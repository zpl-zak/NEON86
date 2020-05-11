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

    mCachedTransform = mTransform * GetParent()->GetTransform();
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
