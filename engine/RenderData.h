#pragma once

#include "system.h"

#include "StdAfx.h"

class CMaterial;

enum RENDERKIND
{
	RENDERKIND_CLEAR,
	RENDERKIND_MATRIX,
	RENDERKIND_POLYGON,
	RENDERKIND_SET_TEXTURE,
	RENDERKIND_SET_SAMPLERSTATE,
    RENDERKIND_SET_RENDERSTATE,
	RENDERKIND_QUAD,
};

enum PRIMITIVEKIND {
	PRIMITIVEKIND_POINTLIST             = 1,
	PRIMITIVEKIND_LINELIST              = 2,
	PRIMITIVEKIND_LINESTRIP             = 3,
	PRIMITIVEKIND_TRIANGLELIST          = 4,
	PRIMITIVEKIND_TRIANGLESTRIP         = 5,
	PRIMITIVEKIND_TRIANGLEFAN           = 6,
};

enum MATRIXKIND {
	MATRIXKIND_VIEW          = 2,
	MATRIXKIND_PROJECTION    = 3,
	MATRIXKIND_TEXTURE0      = 16,
	MATRIXKIND_TEXTURE1      = 17,
	MATRIXKIND_TEXTURE2      = 18,
	MATRIXKIND_TEXTURE3      = 19,
	MATRIXKIND_TEXTURE4      = 20,
	MATRIXKIND_TEXTURE5      = 21,
	MATRIXKIND_TEXTURE6      = 22,
	MATRIXKIND_TEXTURE7      = 23,
};

#define MATRIXKIND_WORLDMATRIX(index) (MATRIXKIND)(index + 256)
#define MATRIXKIND_WORLD  MATRIXKIND_WORLDMATRIX(0)
#define MATRIXKIND_WORLD1 MATRIXKIND_WORLDMATRIX(1)
#define MATRIXKIND_WORLD2 MATRIXKIND_WORLDMATRIX(2)
#define MATRIXKIND_WORLD3 MATRIXKIND_WORLDMATRIX(3)

enum RENDERSTATE
{
#define _X(NAME, VALUE) NAME = VALUE,
#include "RenderStates.h"
#undef _X
};

#define MAX_SAMPLER_STATES 13

enum CLEARFLAG
{
	CLEARFLAG_COLOR = 0x00000001l,
	CLEARFLAG_DEPTH = 0x00000002l,
	CLEARFLAG_STENCIL = 0x00000004l,

	CLEARFLAG_STANDARD = (CLEARFLAG_COLOR|CLEARFLAG_DEPTH),
};

struct VERTEX
{
	FLOAT x, y, z;
	FLOAT nx, ny, nz;
	FLOAT tx, ty, tz;
	union {
		DWORD color;
		struct {
			BYTE a, r, g, b;
		};
	};
	FLOAT su, tv;
};

extern D3DVERTEXELEMENT9 meshVertexFormat[];

struct RENDERBOUNDS {
	FLOAT x1, y1, z1;
	FLOAT x2, y2, z2;
};

struct MATERIAL : D3DMATERIAL9
{
	FLOAT Opacity;
};

#define MAX_VERTS 65536
#define MAX_INDICES 32000

struct RENDERDATA
{
	D3DCOLOR color;
	union 
	{
		UINT flags;
        DWORD stage;
        DWORD state;
	};
	
    UINT kind;

	union
	{
		struct  
		{
			UINT vertCount;
			UINT indexCount;
			UINT primCount;
			RENDERBOUNDS bounds;
		};
	};

	BOOL usesMatrix;
	D3DMATRIX matrix;

	CMaterial* tex;

	LPD3DXMESH mesh;
	D3DXVECTOR3 meshOrigin;
	FLOAT meshRadius;
};