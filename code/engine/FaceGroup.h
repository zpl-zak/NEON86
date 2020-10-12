#pragma once

#include "system.h"
#include "RenderData.h"
#include "ReferenceManager.h"

class CMaterial;

class ENGINE_API CFaceGroup : public CReferenceCounter, CAllocable<CFaceGroup>
{
public:
    CFaceGroup(void);
    ~CFaceGroup();

    void Release(void);
    void SetMaterial(CMaterial* tex);
    void AddVertex(const VERTEX& vertex);
    void AddIndex(short index);
    void Draw(D3DXMATRIX*);
    void CalculateNormals();
    void Build(void);
    void Clear(void);

    auto Clone() -> CFaceGroup*;

    auto GetNumVertices() const -> unsigned int { return mVerts.GetCount(); }
    auto GetVertices() const -> VERTEX* { return mVerts.GetData(); }

    auto GetNumIndices() const -> unsigned int { return mIndices.GetCount(); }
    auto GetIndices() const -> short* { return mIndices.GetData(); }

    auto GetMaterial() const -> CMaterial* { return mData.mat; }

    auto GetBounds() -> D3DXVECTOR4*
    {
        if (mData.meshBounds[0] == mData.meshBounds[1] && D3DXVec4LengthSq(&mData.meshBounds[0]) == 0.0f) Build();
        return mData.meshBounds;
    }

private:
    RENDERDATA mData;

    CArray<VERTEX> mVerts;
    CArray<short> mIndices;
    bool mIsDirty;
};
