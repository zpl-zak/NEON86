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

	inline UINT GetNumVertices() { return mData.vertCount; }
	inline VERTEX* GetVertices() { return mVerts; }

	inline UINT GetNumIndices() { return mData.indexCount; }
	inline SHORT* GetIndices() { return mIndices; }

	inline CMaterial* GetMaterial() { return mData.mat; }
	inline DWORD GetMaterialStage() { return mData.stage; }
private:
	RENDERDATA mData;
	 
	UINT mVertCapacity;
	UINT mIndexCapacity;
	VERTEX* mVerts;
	SHORT* mIndices; 
	BOOL mIsDirty;
};
