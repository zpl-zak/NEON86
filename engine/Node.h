#pragma once

#include "system.h"
#include "NodeComponent.h"
#include "ReferenceManager.h"
#include <assimp/matrix4x4.h>

#include <d3dx9.h>

class CMesh;
class CLight;

class CNode: public CNodeComponent, public CReferenceCounter
{
public:
    CNode() 
    {
        D3DXMatrixIdentity(&mTransform);
        SetName(aiString("(Imported scene)"));
    }

    CNode(aiMatrix4x4 mat, aiString name)
    {
        SetName(name);
        mTransform = *(D3DXMATRIX*)&mat;

        mMeshes.Release();
        mLights.Release();
        mNodes.Release();

        mParent = NULL;
        mIsTransformDirty = TRUE;
    }

    VOID Release() {}

    ~CNode() {}

    inline UINT GetNumMeshes() { return mMeshes.GetCount(); }
    inline CMesh** GetMeshes() { return mMeshes.GetData(); }
    CMesh* FindMesh(LPCSTR name);
    VOID AddMesh(CMesh* mg);

    inline UINT GetNumLights() { return mLights.GetCount(); }
    inline CLight** GetLights() { return mLights.GetData(); }
    CLight* FindLight(LPCSTR name);
    VOID AddLight(CLight* lit);

    inline UINT GetNumNodes() { return mNodes.GetCount(); }
    inline CNode** GetNodes() { return mNodes.GetData(); }
    CNode* FindNode(LPCSTR name);
    VOID AddNode(CNode* tgt);

    inline D3DXMATRIX GetTransform() { return mTransform; }

    inline VOID SetParent(CNode* node) { mParent = node; }
    inline CNode* GetParent() { return mParent; }

    D3DXMATRIX GetFinalTransform();
    inline VOID InvalidateTransform() { mIsTransformDirty = TRUE; }
    VOID InvalidateTransformRecursively();

protected:
    CArray<CMesh*> mMeshes;
    CArray<CLight*> mLights;
    CArray<CNode*> mNodes;
    CNode* mParent;

private:
    BOOL mIsTransformDirty;
    D3DXMATRIX mTransform, mCachedTransform;
};