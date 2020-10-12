#pragma once

#include "system.h"
#include "NodeComponent.h"
#include "ReferenceManager.h"

#include <d3dx9.h>

#include <unordered_map>
#include <string>

class CMesh;
class CLight;

using METADATA = std::unordered_map<std::string, std::string>;

struct METADATA_RESULT
{
    BOOL found;
    std::string value;
};

class ENGINE_API CNode : public CNodeComponent, public CReferenceCounter, CAllocable<CNode>
{
public:
    CNode(): CAllocable()
    {
        mTransform = new D3DXMATRIX();
        mCachedTransform = new D3DXMATRIX();
        mMetadata = new METADATA();

        D3DXMatrixIdentity(mTransform);
        D3DXMatrixIdentity(mCachedTransform);
        SetName("(unknown)");
        mIsTransformDirty = TRUE;
        mParent = nullptr;
    }

    CNode(const D3DXMATRIX mat, const CString& name): CAllocable()
    {
        SetName(name);
        mTransform = new D3DXMATRIX(mat);
        mCachedTransform = new D3DXMATRIX();
        mMetadata = new METADATA();
        mMetadata->clear();

        mParent = nullptr;
        mIsTransformDirty = TRUE;
    }

    auto Release() -> VOID;

    ~CNode() override { Release(); }

    VOID Draw(const D3DXMATRIX& wmat);
    VOID DrawSubset(UINT subset, const D3DXMATRIX& wmat);

    VOID SetMetadata(LPCSTR name, LPCSTR value) const { (*mMetadata)[name] = value; }

    auto GetMetadata(LPCSTR name) const -> METADATA_RESULT
    {
        METADATA_RESULT res = {0};
        const auto e = mMetadata->find(name);

        if (e == mMetadata->end())
        {
            res.found = FALSE;
            res.value = "";
        }
        else
        {
            res.found = TRUE;
            res.value = e->second;
        }

        return res;
    }

    auto GetNumMeshes() const -> UINT { return mMeshes.GetCount(); }
    auto GetMeshes() const -> CArray<CMesh*> { return mMeshes; }
    auto GetMeshData() const -> CMesh** { return mMeshes.GetData(); }
    auto FindMesh(LPCSTR name) -> CMesh*;
    VOID AddMesh(CMesh* mg);

    auto GetNumLights() const -> UINT { return mLights.GetCount(); }
    auto GetLightData() const -> CLight** { return mLights.GetData(); }
    auto FindLight(LPCSTR name) -> CLight*;
    VOID AddLight(CLight* lit);

    auto GetNumNodes() const -> UINT { return mNodes.GetCount(); }
    auto GetNodeData() const -> CNode** { return mNodes.GetData(); }

    auto FindNode(LPCSTR name) -> CNode*;

    VOID AddNode(CNode* tgt);

    auto Clone() -> CNode*;

    auto IsEmpty() -> BOOL;

    VOID SetTransform(D3DXMATRIX transform)
    {
        *mTransform = transform;
        InvalidateTransformRecursively();
    }

    auto GetTransform() const -> D3DXMATRIX { return *mTransform; }

    VOID SetParent(CNode* node) { if (node != this) mParent = node; }
    auto GetParent() const -> CNode* { return mParent; }

    auto GetFinalTransform() -> D3DXMATRIX;
    VOID InvalidateTransform() { mIsTransformDirty = TRUE; }
    VOID InvalidateTransformRecursively();

protected:
    CArray<CMesh*> mMeshes;
    CArray<CLight*> mLights;
    CArray<CNode*> mNodes;
    CNode* mParent;

private:
    BOOL mIsTransformDirty;
    D3DXMATRIX *mTransform, *mCachedTransform;
    METADATA* mMetadata;
};
