#pragma once

#include "system.h"
#include "RenderData.h"
#include "ReferenceManager.h"

class CMaterial;

class CFaceGroup: public CReferenceCounter
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

	inline UINT GetNumVertices() { return mVerts.GetCount(); }
	inline VERTEX* GetVertices() { return mVerts.GetData(); }

	inline UINT GetNumIndices() { return mIndices.GetCount(); }
	inline SHORT* GetIndices() { return mIndices.GetData(); }

	inline CMaterial* GetMaterial() { return mData.mat; }
	inline DWORD GetMaterialStage() { return mData.stage; }
private:
	RENDERDATA mData;
	
	CArray<VERTEX> mVerts;
	CArray<SHORT> mIndices;
	BOOL mIsDirty;
};
