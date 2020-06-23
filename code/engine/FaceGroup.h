#pragma once

#include "system.h"
#include "RenderData.h"
#include "ReferenceManager.h"

class CMaterial;

class CFaceGroup: public CReferenceCounter, CAllocable<CFaceGroup>
{
public:
	CFaceGroup(VOID);
	~CFaceGroup() { Release(); }

	VOID Release(VOID);
	VOID SetMaterial(DWORD stage, CMaterial* tex);
	VOID AddVertex(const VERTEX& vertex);
	VOID AddIndex(SHORT index);
	VOID Draw(D3DXMATRIX*);
	VOID CalculateNormals();
	VOID Build(VOID);
	VOID Clear(VOID);

	CFaceGroup* Clone();

	inline UINT GetNumVertices() { return mVerts.GetCount(); }
	inline VERTEX* GetVertices() { return mVerts.GetData(); }

	inline UINT GetNumIndices() { return mIndices.GetCount(); }
	inline SHORT* GetIndices() { return mIndices.GetData(); }

	inline CMaterial* GetMaterial() { return mData.mat; }
	inline DWORD GetMaterialStage() { return mData.stage; }
	inline D3DXVECTOR4* GetBounds() { if (mData.meshBounds[0] == mData.meshBounds[1] && !D3DXVec4LengthSq(&mData.meshBounds[0])) Build(); return mData.meshBounds; }
private:
	RENDERDATA mData;
	
	CArray<VERTEX> mVerts;
	CArray<SHORT> mIndices;
	BOOL mIsDirty;
};
