#pragma once

#include "system.h"
#include "RenderData.h"
#include "ReferenceManager.h"
#include "NodeComponent.h"

class CFaceGroup;

class ENGINE_API CMesh : public CReferenceCounter, public CNodeComponent, CAllocable<CMesh>, NoCopyAssign
{
public:
    CMesh(void);
    ~CMesh() override;

    void Release(void);
    void AddFaceGroup(CFaceGroup*, const D3DXMATRIX&);
    auto Clone() -> CMesh*;

    void Draw(const D3DXMATRIX& wmat) const;
    void Clear(void);

    auto GetNumFGroups() const -> unsigned int { return mFaceGroups.GetCount(); }
    auto GetFGroupData() const -> CFaceGroup** { return mFaceGroups.GetData(); }
    auto GetTransformData() const -> D3DXMATRIX* { return mTransforms.GetData(); }
    auto GetTransforms() const -> CArray<D3DXMATRIX> { return mTransforms; }
private:
    CArray<CFaceGroup*> mFaceGroups;
    CArray<D3DXMATRIX> mTransforms;
};
