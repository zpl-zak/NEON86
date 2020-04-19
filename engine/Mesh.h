#pragma once

#include "system.h"
#include "RenderData.h"

class CMaterial;

class CMesh
{
public:
	CMesh(void);

	VOID Release(void);
	VOID SetTexture(DWORD stage, CMaterial* tex);
	VOID AddVertex(const VERTEX& vertex);
	VOID AddIndex(SHORT index);
	VOID Draw(D3DXMATRIX*);
	VOID CalculateNormals();
	VOID Build(void);
	VOID Clear(void);

private:
	RENDERDATA mData;

	VERTEX mVerts[MAX_VERTS];
	SHORT mIndices[MAX_INDICES];
	BOOL mIsDirty;
};
