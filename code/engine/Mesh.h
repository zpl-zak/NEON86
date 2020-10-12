#pragma once

#include "system.h"
#include "RenderData.h"
#include "ReferenceManager.h"
#include "NodeComponent.h"

class CFaceGroup;

class ENGINE_API CMesh : public CReferenceCounter, public CNodeComponent, CAllocable<CMesh>
{
public:
    CMesh(VOID);
    ~CMesh() override { Release(); }

    VOID Release(VOID);
    VOID AddFaceGroup(CFaceGroup*, const D3DXMATRIX&);
    auto Clone() -> CMesh*;

    VOID Draw(const D3DXMATRIX& wmat);
    VOID Clear(VOID);

    auto GetNumFGroups() const -> UINT { return mFaceGroups.GetCount(); }
    auto GetFGroupData() const -> CFaceGroup** { return mFaceGroups.GetData(); }
    auto GetTransformData() const -> D3DXMATRIX* { return mTransforms.GetData(); }
    auto GetTransforms() const -> CArray<D3DXMATRIX> { return mTransforms; }
private:
    CArray<CFaceGroup*> mFaceGroups;
    CArray<D3DXMATRIX> mTransforms;
};
