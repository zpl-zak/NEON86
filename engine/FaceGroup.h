#pragma once

#include "system.h"
#include "RenderData.h"
#include "ReferenceManager.h"

class CMaterial;

class CFaceGroup: public CReference<CFaceGroup>
{
public:
	CFaceGroup(void);

	VOID Release(void);
	VOID SetMaterial(DWORD stage, CMaterial* tex);
	VOID AddVertex(const VERTEX& vertex);
	VOID AddIndex(SHORT index);
	VOID Draw(D3DXMATRIX*);
	VOID CalculateNormals();
	VOID Build(void);
	VOID Clear(void);

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
