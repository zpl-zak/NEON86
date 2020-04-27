#pragma once

#include "system.h"
#include "RenderData.h"
#include "ReferenceManager.h"

#include <Assimp/Importer.hpp>

class CFaceGroup;

class CMesh: public CReference<CMesh>
{
public:
    CMesh(void);
     
    VOID Release(void);
    VOID AddMesh(CFaceGroup*, const D3DXMATRIX&);
    
    VOID Draw(const D3DXMATRIX& wmat);
    VOID Clear(void);

    inline UINT GetNumFGroups() const { return mCount; }
    inline CFaceGroup** GetFGroups() { return mFaceGroups; }
    inline D3DXMATRIX* GetTransforms() { return mTransforms; }

    inline aiString GetName() { return mName; }
    inline void SetName(aiString name) { mName = name; }

private:
    CFaceGroup** mFaceGroups;
    D3DXMATRIX* mTransforms;
    UINT mCount;
    UINT mCapacity;
    aiString mName;
};
