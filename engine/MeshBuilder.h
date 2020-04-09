#pragma once

#include "system.h"
#include "RenderData.h"

class CMeshBuilder
{
public:
	CMeshBuilder(void);

	VOID Release(void);
	VOID SetTexture(DWORD stage, LPDIRECT3DTEXTURE9 tex);
	VOID AddVertex(const VERTEX& vertex);
	VOID AddIndex(SHORT index);
	VOID Draw(void);
	VOID Build(void);
	VOID Clear(void);

private:
	RENDERDATA mData;

	VERTEX mVerts[MAX_VERTS];
	SHORT mIndices[MAX_INDICES];
	BOOL mIsDirty;
};
