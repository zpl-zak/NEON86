#pragma once

#include "system.h"
#include "RenderData.h"
#include "ReferenceManager.h"
#include "NodeComponent.h"

class CFaceGroup;

class ENGINE_API CMesh: public CReferenceCounter, public CNodeComponent, CAllocable<CMesh>
{
public:
    CMesh(VOID);
    ~CMesh() { Release(); }

    VOID Release(VOID);
    VOID AddFaceGroup(CFaceGroup*, const D3DXMATRIX&);
    CMesh* Clone();

    VOID Draw(const D3DXMATRIX& wmat);
    VOID Clear(VOID);

    inline UINT GetNumFGroups() { return mFaceGroups.GetCount(); }
    inline CFaceGroup** GetFGroupData() { return mFaceGroups.GetData(); }
    inline D3DXMATRIX* GetTransformData() { return mTransforms.GetData(); }
    inline CArray<D3DXMATRIX> GetTransforms() { return mTransforms; }
private:
    CArray<CFaceGroup*> mFaceGroups;
    CArray<D3DXMATRIX> mTransforms;
};
