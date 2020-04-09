#include "stdafx.h"
#include "LuaBindings.h"
#include "NeonEngine.h"

#define LUA_COMPAT_MODULE
#include <lua/lua.hpp>

#include "LuaMatrix.h"
#include "LuaVector3.h"
#include "LuaVertex.h"

/// BASE METHODS
LUAF(Base, ShowMessage) 
{
	const char* caption = luaL_checkstring(L, 1);
	const char* text = luaL_checkstring(L, 2);
	MessageBoxA(NULL, text, caption, MB_OK);
	return 0;
}
LUAF(Rend, SetFPS)
{
	FLOAT fps = (FLOAT)luaL_checknumber(L, 1);

	ENGINE->SetFPS(fps);

	return 0;
}
///<END

VOID CLuaBindings::BindBase(lua_State* L)
{
	REGF(Base, ShowMessage);
	REGF(Rend, SetFPS);
}

/// MATH METHODS
LUAF(Math, Color)
{
	UINT r = (UINT)luaL_checknumber(L, 1);
	UINT g = (UINT)luaL_checknumber(L, 2);
	UINT b = (UINT)luaL_checknumber(L, 3);
	
	lua_pushnumber(L, D3DCOLOR_XRGB(r,g,b));
	return 1;
}
///<END

VOID CLuaBindings::BindMath(lua_State* L)
{
	LuaMatrix_register(L);
	LuaVector_register(L);
	LuaVertex_register(L);

	REGF(Math, Color);
}

/// RENDERER METHODS
LUAF(Rend, ClearScene)
{
	UINT r = (UINT)luaL_checknumber(L, 1);
	UINT g = (UINT)luaL_checknumber(L, 2);
	UINT b = (UINT)luaL_checknumber(L, 3);
	UINT flags = CLEARFLAG_STANDARD;
	
	if (lua_isnumber(L, 4))
		flags = (UINT)luaL_checknumber(L, 4);
	RENDERER->PushClear(D3DCOLOR_XRGB(r,g,b), flags);
	return 0;
}
LUAF(Rend, DrawIndexedTriangle)
{
	VERTEX vertBuf[MAX_VERTS];
	SHORT indexBuf[MAX_INDICES];
	UINT vertCount=0, indexCount=0;

	if (lua_gettop(L) != 2)
		return 0;

	if (lua_isnil(L, -1))
		return 0;

	if (lua_isnil(L, -2))
		return 0;

	lua_pushnil(L);
	while (lua_next(L, 1))
	{
		indexBuf[indexCount++] = (SHORT)luaL_checknumber(L, -1);
		lua_pop(L, 1);
	}

	lua_pushnil(L);
	while (lua_next(L, 2))
	{
		vertBuf[vertCount++] = *(VERTEX*)luaL_checkudata(L, -1, L_VERTEX);
		lua_pop(L, 1);
	}

	RENDERER->PushIndexedPolygon(PRIMITIVEKIND_TRIANGLELIST, indexCount/3, indexBuf, indexCount, vertBuf, vertCount);
	return 0;
}
LUAF(Rend, DrawTriangle)
{
	VERTEX vertBuf[MAX_VERTS];
	UINT vertCount=0;

	if (lua_gettop(L) != 1)
		return 0;

	if (lua_isnil(L, -1))
		return 0;

	lua_pushnil(L);
	while (lua_next(L, 1))
	{
		vertBuf[vertCount++] = *(VERTEX*)luaL_checkudata(L, -1, L_VERTEX);
		lua_pop(L, 1);
	}

	RENDERER->PushPolygon(PRIMITIVEKIND_TRIANGLELIST, vertCount/3, vertBuf, vertCount);
	return 0;
}
LUAF(Rend, CameraPerspective)
{
	FLOAT fov = (FLOAT)luaL_checknumber(L, 1);
	FLOAT zNear=1.0f, zFar=100.0f;

	if (lua_gettop(L) == 3)
	{
		zNear = (FLOAT)luaL_checknumber(L, 2);
		zFar = (FLOAT)luaL_checknumber(L, 3);
	}

	D3DXMATRIX matProjection;    
	RECT res = RENDERER->GetResolution();

	D3DXMatrixPerspectiveFovLH(&matProjection,
		D3DXToRadian(fov),
		(FLOAT)res.right / (FLOAT)res.bottom,
		zNear,    
		zFar);    

	RENDERER->GetDevice()->SetTransform(D3DTS_PROJECTION, &matProjection);

	return 0;
}
LUAF(Rend, CameraOrthographic)
{
	RECT res = RENDERER->GetResolution();
	FLOAT w=(FLOAT)res.right, h=(FLOAT)res.bottom;

	w = (FLOAT)luaL_checknumber(L, 1) * ((FLOAT)res.right / (FLOAT)res.bottom);
	h = (FLOAT)luaL_checknumber(L, 2);

	FLOAT zNear=1.0f, zFar=100.0f;

	if (lua_gettop(L) == 4)
	{
		zNear = (FLOAT)luaL_checknumber(L, 3);
		zFar = (FLOAT)luaL_checknumber(L, 4);
	}
	D3DXMATRIX matProjection;    

	D3DXMatrixOrthoLH(&matProjection,
		  			 w,
					 h,
					 1.0f,    
					 100.0f);

	RENDERER->GetDevice()->SetTransform(D3DTS_PROJECTION, &matProjection);

	return 0;
}
///<END

VOID CLuaBindings::BindRenderer(lua_State* L)
{
	REGF(Rend, ClearScene);
	REGF(Rend, CameraPerspective);
	REGF(Rend, CameraOrthographic);

	REGF(Rend, DrawIndexedTriangle);
	REGF(Rend, DrawTriangle);

	// enums
	{
		REGE(RENDERKIND_CLEAR);
		REGE(RENDERKIND_MATRIX);
		REGE(RENDERKIND_POLYGON);
		REGE(RENDERKIND_POLYGON_INDEXED);

		REGE(PRIMITIVEKIND_POINTLIST);
		REGE(PRIMITIVEKIND_LINELIST);
		REGE(PRIMITIVEKIND_LINESTRIP);
		REGE(PRIMITIVEKIND_TRIANGLELIST);
		REGE(PRIMITIVEKIND_TRIANGLESTRIP);
		REGE(PRIMITIVEKIND_TRIANGLEFAN);

		REGE(MATRIXKIND_VIEW);
		REGE(MATRIXKIND_PROJECTION);
		REGE(MATRIXKIND_TEXTURE0);
		REGE(MATRIXKIND_TEXTURE1);
		REGE(MATRIXKIND_TEXTURE2);
		REGE(MATRIXKIND_TEXTURE3);
		REGE(MATRIXKIND_TEXTURE4);
		REGE(MATRIXKIND_TEXTURE5);
		REGE(MATRIXKIND_TEXTURE6);
		REGE(MATRIXKIND_TEXTURE7);
		REGE(MATRIXKIND_WORLD);

		REGE(CLEARFLAG_COLOR);
		REGE(CLEARFLAG_DEPTH);
		REGE(CLEARFLAG_STENCIL);
		REGE(CLEARFLAG_STANDARD);

		// helpers
		lua_pushnumber(L, MATRIXKIND_WORLD); lua_setglobal(L, "WORLD");
		lua_pushnumber(L, MATRIXKIND_VIEW); lua_setglobal(L, "VIEW");
		lua_pushnumber(L, MATRIXKIND_PROJECTION); lua_setglobal(L, "PROJ");

	}
}
