#include "stdafx.h"
#include "LuaBindings.h"
#include "NeonEngine.h"

#include <lua/lua.hpp>

#include "LuaMatrix.h"
#include "LuaVector3.h"
#include "LuaVertex.h"
#include "LuaTexture.h"
#include "LuaMesh.h"
#include "LuaMeshGroup.h"

/// BASE METHODS
LUAF(Base, ShowMessage) 
{
	const char* caption = luaL_checkstring(L, 1);
	const char* text = luaL_checkstring(L, 2);
	MessageBoxA(NULL, text, caption, MB_OK);
	return 0;
}
LUAF(Base, ExitGame)
{
	ENGINE->Shutdown();
	return 0;
}
LUAF(Base, SetFPS)
{
	FLOAT fps = (FLOAT)luaL_checknumber(L, 1);

	ENGINE->SetFPS(fps);

	return 0;
}
LUAF(Base, dofile)
{
	const char* scriptName = luaL_checkstring(L, 1);

	FDATA fd = FILESYSTEM->GetResource(RESOURCEKIND_TEXT, (LPSTR)scriptName);

	if (!fd.data)
	{
		MessageBoxA(NULL, "No dofile game script found!", "Resource error", MB_OK);
		ENGINE->Shutdown();
		return 0;
	}

	INT res = luaL_dostring(L,(char *)fd.data);
	VM->CheckVMErrors(res);
	FILESYSTEM->FreeResource(fd.data);

	return 0;
}
LUAF(Base, time)
{
	lua_pushnumber(L, GetTime());
    return 1;
}
///<END

VOID CLuaBindings::BindBase(lua_State* L)
{
	REGF(Base, ShowMessage);
	REGF(Base, ExitGame);
	REGF(Base, SetFPS);
	REGF(Base, dofile);
	REGF(Base, time);
}

/// MATH METHODS
LUAF(Math, Color)
{
	UINT r = (UINT)luaL_checknumber(L, 1);
	UINT g = (UINT)luaL_checknumber(L, 2);
	UINT b = (UINT)luaL_checknumber(L, 3);
	UINT a = 0xFF;

	if (lua_gettop(L) == 4)
		a = (UINT)luaL_checknumber(L, 4);
	
	lua_pushnumber(L, D3DCOLOR_ARGB(a,r,g,b));
	return 1;
}
///<END

VOID CLuaBindings::BindMath(lua_State* L)
{
	LuaMatrix_register(L);
	LuaVector_register(L);

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
LUAF(Rend, BindTexture)
{
	DWORD stage = (DWORD)luaL_checknumber(L, 1);
	LPDIRECT3DTEXTURE9* tex = NULL;

	if (lua_gettop(L) == 2)
		tex = (LPDIRECT3DTEXTURE9*)luaL_checkudata(L, 2, L_TEXTURE);

	RENDERER->PushTexture(stage, tex ? *tex : NULL);
	return 0;
}
LUAF(Rend, GetResolution)
{
    RECT res = RENDERER->GetResolution();
    lua_newtable(L);

    // x
    lua_pushinteger(L, 1);
    lua_pushnumber(L, res.right);
    lua_settable(L, -3);

    // y
    lua_pushinteger(L, 2);
    lua_pushnumber(L, res.bottom);
    lua_settable(L, -3);

    return 1;
}
///<END

VOID CLuaBindings::BindRenderer(lua_State* L)
{
	REGF(Rend, ClearScene);
	REGF(Rend, CameraPerspective);
	REGF(Rend, CameraOrthographic);
	REGF(Rend, GetResolution);

	REGF(Rend, BindTexture);

	LuaVertex_register(L);
	LuaTexture_register(L);
	LuaMesh_register(L);
	LuaMeshGroup_register(L);

	// enums
	{
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

/// INPUT METHODS
static DWORD GetScanCodeFromLua(lua_State* L)
{
	if (lua_isinteger(L, 1))
		return (DWORD)lua_tointeger(L, 1);

	if (lua_isstring(L, 1))
	{
		const CHAR* str = lua_tostring(L, 1);

		if (strlen(str) != 1)
			return 0x0;

		CHAR c = str[0];

		if (isalpha(c))
		{
			CHAR dist = c - 'a';
			return (0x41 + dist);
		}

		if (isdigit(c))
		{
            CHAR dist = c - '0';
            return (0x30 + dist);
		}
	}

	return 0x0;
}

LUAF(Input, IsCursorVisible)
{
    lua_pushboolean(L, INPUT->GetCursor());
    return 1;
}
LUAF(Input, ShowCursor)
{
	BOOL state = (BOOL)lua_toboolean(L, 1);
    INPUT->SetCursor(state);
    return 0;
}
LUAF(Input, GetKey)
{
	DWORD code = GetScanCodeFromLua(L);
	lua_pushboolean(L, INPUT->GetKey(code));
	return 1;
}
LUAF(Input, GetKeyDown)
{
	DWORD code = GetScanCodeFromLua(L);
	lua_pushboolean(L, INPUT->GetKeyDown(code));
	return 1;
}
LUAF(Input, GetKeyUp)
{
	DWORD code = GetScanCodeFromLua(L);
    lua_pushboolean(L, INPUT->GetKeyUp(code));
    return 1;
}
LUAF(Input, GetMouseXY)
{
	POINT pos = INPUT->GetMouseXY();
	lua_newtable(L);

	// x
    lua_pushinteger(L, 1);
    lua_pushnumber(L, pos.x);
    lua_settable(L, -3);

	// y
    lua_pushinteger(L, 2);
    lua_pushnumber(L, pos.y);
    lua_settable(L, -3);

    return 1;
}
LUAF(Input, GetMouseDelta)
{
    POINT pos = INPUT->GetMouseDelta();
    lua_newtable(L);

    // x
    lua_pushinteger(L, 1);
    lua_pushnumber(L, pos.x);
    lua_settable(L, -3);

    // y
    lua_pushinteger(L, 2);
    lua_pushnumber(L, pos.y);
    lua_settable(L, -3);

    return 1;
}
LUAF(Input, SetMouseXY)
{
	SHORT x = (SHORT)luaL_checkinteger(L, 1);
	SHORT y = (SHORT)luaL_checkinteger(L, 2);
	INPUT->SetMouseXY(x, y);

    return 0;
}
LUAF(Input, GetCursorMode)
{
	lua_pushinteger(L, INPUT->GetCursorMode());
    return 1;
}
LUAF(Input, SetCursorMode)
{
	UCHAR mode = (UCHAR)luaL_checkinteger(L, 1);
	INPUT->SetCursorMode(mode);
    return 0;
}
///<END

VOID CLuaBindings::BindInput(lua_State* L)
{
	REGF(Input, GetKey);
	REGF(Input, GetKeyDown);
	REGF(Input, GetKeyUp);
	REGF(Input, GetMouseXY);
	REGF(Input, GetMouseDelta);
	REGF(Input, SetMouseXY);

	REGF(Input, IsCursorVisible);
	REGF(Input, ShowCursor);

	REGF(Input, GetCursorMode);
	REGF(Input, SetCursorMode);
	REGF(Input, ShowCursor);

	// keys
	{
#define _X(NAME, VALUE) \
		lua_pushinteger(L, VALUE); \
		lua_setglobal(L, #NAME); \
		lua_settop(L, 0);
#include "InputCodes.h"
#undef _X
	}
}
