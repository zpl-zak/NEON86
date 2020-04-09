#pragma once

#include "system.h"
#include "RenderData.h"

class MeshBuilder
{
public:
	MeshBuilder(void);

	VOID Release(void);
	VOID SetTexture(DWORD stage, LPDIRECT3DTEXTURE9 tex);
	VOID AddVertex(const VERTEX& vertex);
	VOID AddIndex(SHORT index);
	VOID Draw(void);
	VOID Build(void);

private:
	RENDERDATA mData;

	VERTEX mVerts[MAX_VERTS];
	SHORT mIndices[MAX_INDICES];
};
