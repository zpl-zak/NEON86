#pragma once

#include "system.h"
#include "RenderData.h"
#include "ReferenceManager.h"

class CMaterial;

class ENGINE_API CFaceGroup : public CReferenceCounter, CAllocable<CFaceGroup>
{
public:
    CFaceGroup(VOID);
    ~CFaceGroup();

    VOID Release(VOID);
    VOID SetMaterial(CMaterial* tex);
    VOID AddVertex(const VERTEX& vertex);
    VOID AddIndex(SHORT index);
    VOID Draw(D3DXMATRIX*);
    VOID CalculateNormals();
    VOID Build(VOID);
    VOID Clear(VOID);

    auto Clone() -> CFaceGroup*;

    auto GetNumVertices() const -> UINT { return mVerts.GetCount(); }
    auto GetVertices() const -> VERTEX* { return mVerts.GetData(); }

    auto GetNumIndices() const -> UINT { return mIndices.GetCount(); }
    auto GetIndices() const -> SHORT* { return mIndices.GetData(); }

    auto GetMaterial() const -> CMaterial* { return mData.mat; }

    auto GetBounds() -> D3DXVECTOR4*
    {
        if (mData.meshBounds[0] == mData.meshBounds[1] && D3DXVec4LengthSq(&mData.meshBounds[0]) == 0.0f) Build();
        return mData.meshBounds;
    }

private:
    RENDERDATA mData;

    CArray<VERTEX> mVerts;
    CArray<SHORT> mIndices;
    BOOL mIsDirty;
};
