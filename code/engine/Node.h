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
    bool found;
    std::string value;
};

class ENGINE_API CNode : public CNodeComponent, public CReferenceCounter, CAllocable<CNode>, NoCopyAssign
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

    CNode(const CNode&&) = delete;
    CNode& operator=(const CNode&&) = delete;

    auto Release() -> void;

    ~CNode() override { Release(); }

    void Draw(const D3DXMATRIX& wmat);
    void DrawSubset(unsigned int subset, const D3DXMATRIX& wmat);

    void SetMetadata(LPCSTR name, LPCSTR value) const { (*mMetadata)[name] = value; }

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

    auto GetNumMeshes() const -> unsigned int { return mMeshes.GetCount(); }
    auto GetMeshes() const -> CArray<CMesh*> { return mMeshes; }
    auto GetMeshData() const -> CMesh** { return mMeshes.GetData(); }
    auto FindMesh(LPCSTR name) -> CMesh*;
    void AddMesh(CMesh* mg);

    auto GetNumLights() const -> unsigned int { return mLights.GetCount(); }
    auto GetLightData() const -> CLight** { return mLights.GetData(); }
    auto FindLight(LPCSTR name) -> CLight*;
    void AddLight(CLight* lit);

    auto GetNumNodes() const -> unsigned int { return mNodes.GetCount(); }
    auto GetNodeData() const -> CNode** { return mNodes.GetData(); }

    auto FindNode(LPCSTR name) -> CNode*;

    void AddNode(CNode* tgt);

    auto Clone() -> CNode*;

    auto IsEmpty() -> bool;

    void SetTransform(D3DXMATRIX transform)
    {
        *mTransform = transform;
        InvalidateTransformRecursively();
    }

    auto GetTransform() const -> D3DXMATRIX { return *mTransform; }

    void SetParent(CNode* node) { if (node != this) mParent = node; }
    auto GetParent() const -> CNode* { return mParent; }

    auto GetFinalTransform() -> D3DXMATRIX;
    void InvalidateTransform() { mIsTransformDirty = TRUE; }
    void InvalidateTransformRecursively();

protected:
    CArray<CMesh*> mMeshes;
    CArray<CLight*> mLights;
    CArray<CNode*> mNodes;
    CNode* mParent;

private:
    bool mIsTransformDirty;
    D3DXMATRIX *mTransform, *mCachedTransform;
    METADATA* mMetadata;
};
