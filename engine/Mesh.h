#pragma once

#include "system.h"
#include "RenderData.h"
#include "ReferenceManager.h"
#include "NodeComponent.h"

class CFaceGroup;

class CMesh: public CReferenceCounter, public CNodeComponent
{
public:
    CMesh(VOID);
     
    VOID Release(VOID);
    VOID AddFaceGroup(CFaceGroup*, const D3DXMATRIX&);
    
    VOID Draw(const D3DXMATRIX& wmat);
    VOID Clear(VOID);

    inline UINT GetNumFGroups() { return mFaceGroups.GetCount(); }
    inline CFaceGroup** GetFGroups() { return mFaceGroups.GetData(); }
    inline D3DXMATRIX* GetTransforms() { return mTransforms.GetData(); }
private:
    CArray<CFaceGroup*> mFaceGroups;
    CArray<D3DXMATRIX> mTransforms;
};
