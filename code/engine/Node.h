#pragma once

#include "system.h"
#include "NodeComponent.h"
#include "ReferenceManager.h"
#include <assimp/matrix4x4.h>

#include <d3dx9.h>

#include <unordered_map>
#include <string>

class CMesh;
class CLight;

using METADATA = std::unordered_map<std::string, std::string>;

struct METADATA_RESULT
{
    BOOL Found;
    std::string Value;
};

class ENGINE_API CNode: public CNodeComponent, public CReferenceCounter, CAllocable<CNode>
{
public:
    CNode(): CAllocable()
    {
        mTransform = new D3DXMATRIX();
        mCachedTransform = new D3DXMATRIX();
        mMetadata = new METADATA();

        D3DXMatrixIdentity(mTransform);
        D3DXMatrixIdentity(mCachedTransform);
        SetName(aiString("(unknown)"));
        mIsTransformDirty = TRUE;
        mParent = NULL;

        mMeshes.Release();
        mLights.Release();
        mNodes.Release();
    }

    CNode(aiMatrix4x4 mat, aiString name): CAllocable()
    {
        SetName(name);
        mTransform = new D3DXMATRIX(*(D3DXMATRIX*)&mat);
        mCachedTransform = new D3DXMATRIX();
        mMetadata = new METADATA();

        mMeshes.Release();
        mLights.Release();
        mNodes.Release();
        mMetadata->clear();

        mParent = NULL;
        mIsTransformDirty = TRUE;
    }

    VOID Release();

    ~CNode() { Release(); }

    VOID Draw(const D3DXMATRIX& wmat);
    VOID DrawSubset(UINT subset, const D3DXMATRIX& wmat);

    inline VOID SetMetadata(LPCSTR name, LPCSTR value) { (*mMetadata)[name] = value; }
    inline METADATA_RESULT GetMetadata(LPCSTR name) 
    {
        METADATA_RESULT res = { 0 };
        auto e = mMetadata->find(name);

        if (e == mMetadata->end())
        {
            res.Found = FALSE;
            res.Value = "";
        }
        else
        {
            res.Found = TRUE;
            res.Value = e->second;
        }

        return res;
    }

    inline UINT GetNumMeshes() { return mMeshes.GetCount(); }
    inline CArray<CMesh*> GetMeshes() { return mMeshes; }
    inline CMesh** GetMeshData() { return mMeshes.GetData(); }
    CMesh* FindMesh(LPCSTR name);
    VOID AddMesh(CMesh* mg);

    inline UINT GetNumLights() { return mLights.GetCount(); }
    inline CLight** GetLightData() { return mLights.GetData(); }
    CLight* FindLight(LPCSTR name);
    VOID AddLight(CLight* lit);

    inline UINT GetNumNodes() { return mNodes.GetCount(); }
    inline CNode** GetNodeData() { return mNodes.GetData(); }
    CNode* FindNode(LPCSTR name);
    VOID AddNode(CNode* tgt);
    CNode* Clone();

    BOOL IsEmpty();

    inline VOID SetTransform(D3DXMATRIX transform) { *mTransform = transform; InvalidateTransformRecursively(); }
    inline D3DXMATRIX GetTransform() { return *mTransform; }

    inline VOID SetParent(CNode* node) { if (node != this) mParent = node; }
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
    D3DXMATRIX* mTransform, *mCachedTransform;
    METADATA *mMetadata;
};