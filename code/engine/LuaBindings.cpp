#include "stdafx.h"
#include "LuaBindings.h"

#include "Engine.h"
#include "VM.h"
#include "Renderer.h"
#include "Input.h"
#include "FileSystem.h"

#include <lua/lua.hpp>
#include <sstream>

static D3DXVECTOR4 luaH_getcomps(lua_State* L, UINT offset=0)
{
    if (luaL_testudata(L, 2+offset, L_VECTOR))
    {
        return *(D3DXVECTOR4*)luaL_checkudata(L, 2+offset, L_VECTOR);
    }

    FLOAT x = (FLOAT)lua_tonumber(L, 2+offset);
    FLOAT y = (FLOAT)lua_tonumber(L, 3+offset);
    FLOAT z = (FLOAT)lua_tonumber(L, 4+offset);
	FLOAT w = (FLOAT)lua_tonumber(L, 5+offset);


    if (lua_gettop(L) == 2+offset)
        w = y = z = x;

    if (lua_gettop(L) == 3+offset)
        w = z = x;

	if (lua_gettop(L) == 4 + offset)
		w = 0;

    return D3DXVECTOR4(x, y, z, w);
}

#include "LuaMatrix.h"
#include "LuaVector4.h"
#include "LuaVertex.h"
#include "LuaMaterial.h"
#include "LuaLight.h"
#include "LuaFaceGroup.h"
#include "LuaMesh.h"
#include "LuaNode.h"
#include "LuaScene.h"
#include "LuaRenderTarget.h"
#include "LuaEffect.h"

/// BASE METHODS
LUAF(Base, ShowMessage) 
{
	const char* caption = luaL_checkstring(L, 1);
	const char* text = luaL_checkstring(L, 2);
	MessageBoxA(NULL, text, caption, MB_OK);
	return 0;
}
LUAF(Base, LogString)
{
    const char* text = luaL_checkstring(L, 1);
    OutputDebugStringA(text);
    return 0;
}
LUAF(Base, ExitGame)
{
	ENGINE->Shutdown();
	return 0;
}
LUAF(Base, RestartGame)
{
	VM->Restart();
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
LUAF(Base, loadfile)
{
    const char* scriptName = luaL_checkstring(L, 1);

    FDATA fd = FILESYSTEM->GetResource(RESOURCEKIND_USER, (LPSTR)scriptName);

    if (!fd.data)
    {
        MessageBoxA(NULL, "No loadfile content found!", "Resource error", MB_OK);
        ENGINE->Shutdown();
        return 0;
    }

    lua_pushlstring(L, (char*)fd.data, fd.size);
    FILESYSTEM->FreeResource(fd.data);

    return 1;
}
LUAF(Base, getTime)
{
	lua_pushnumber(L, VM->GetRunTime());
    return 1;
}
///<END

VOID CLuaBindings::BindBase(lua_State* L)
{
	REGF(Base, ShowMessage);
	REGF(Base, LogString);
    REGF(Base, ExitGame);
    REGF(Base, RestartGame);
	REGF(Base, SetFPS);
	REGF(Base, dofile);
	REGF(Base, loadfile);
	REGF(Base, getTime);
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
LUAF(Math, ColorLinear)
{
    FLOAT r = (FLOAT)luaL_checknumber(L, 1) / (FLOAT)0xFF;
    FLOAT g = (FLOAT)luaL_checknumber(L, 2) / (FLOAT)0xFF;
    FLOAT b = (FLOAT)luaL_checknumber(L, 3) / (FLOAT)0xFF;
    FLOAT a = 0xFF;

    if (lua_gettop(L) == 4)
        a = (FLOAT)luaL_checknumber(L, 4) / (FLOAT)0xFF;

    lua_pushnumber(L, r);
	lua_pushnumber(L, g);
	lua_pushnumber(L, b);
	lua_pushnumber(L, a);
    return 4;
}
LUAF(Math, str2vec)
{
	std::string str = std::string(luaL_checkstring(L, 1));
	FLOAT nums[4] = { 0.0f };
	UINT i = 0;

	// HACK
	if (str.find("vec(") != std::string::npos)
	{
		str = str.substr(4, str.size() - 5);
	}

	auto comps = split(str, ",");

	while (i < 4)
	{
		if (i >= comps.size())
			break;

		nums[i] = (FLOAT)::atof(comps.at(i).c_str());
		i++;
	}
	
	vector4_new(L);
	*(D3DXVECTOR4*)lua_touserdata(L, -1) = D3DXVECTOR4(nums[0], nums[1], nums[2], nums[3]);
	return 1;
}
LUAF(Math, vec2str)
{
	D3DXVECTOR4 v = *(D3DXVECTOR4*)luaL_checkudata(L, 1, L_VECTOR);

	std::stringstream ss;
	ss << v.x << "," << v.y << "," << v.z;

	lua_pushstring(L, (LPCSTR)ss.str().c_str());
	return 1;
}
///<END

VOID CLuaBindings::BindMath(lua_State* L)
{
	LuaMatrix_register(L);
	LuaVector_register(L);

	REGF(Math, Color);
	REGF(Math, ColorLinear);
	REGF(Math, str2vec);
	REGF(Math, vec2str);
}

/// RENDERER METHODS
LUAF(Rend, ClearScene)
{
	UINT r = 0, g = 0, b = 0;

	if (lua_gettop(L) >= 3)
	{
		r = (UINT)luaL_checknumber(L, 1);
		g = (UINT)luaL_checknumber(L, 2);
		b = (UINT)luaL_checknumber(L, 3);
	}
	UINT flags = CLEARFLAG_STANDARD;
	
	if (lua_isnumber(L, 4))
		flags = (UINT)luaL_checknumber(L, 4);
	RENDERER->ClearBuffer(D3DCOLOR_XRGB(r,g,b), flags);
	return 0;
}
LUAF(Rend, CameraPerspective)
{
	FLOAT fov = (FLOAT)luaL_checknumber(L, 1);
	FLOAT zNear=0.1f, zFar=1000.0f;

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

	RENDERER->SetMatrix(MATRIXKIND_PROJECTION, matProjection);

	return 0;
}
LUAF(Rend, CameraOrthographic)
{
	RECT res = RENDERER->GetResolution();
	FLOAT w=(FLOAT)res.right, h=(FLOAT)res.bottom;

	if (lua_gettop(L) >= 2)
	{
        w = (FLOAT)luaL_checknumber(L, 1) * ((FLOAT)res.right / (FLOAT)res.bottom);
        h = (FLOAT)luaL_checknumber(L, 2);
	}

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
					 zNear,    
					 zFar);

	RENDERER->SetMatrix(MATRIXKIND_PROJECTION, matProjection);

	return 0;
}
LUAF(Rend, BindTexture)
{
	DWORD stage = (DWORD)luaL_checknumber(L, 1);
	CMaterial* tex = NULL;

    if (luaL_testudata(L, 2, L_RENDERTARGET))
    {
        CRenderTarget* rtt = *(CRenderTarget**)lua_touserdata(L, 2);
		RENDERER->SetTexture(stage, rtt->GetTextureHandle());
    }
    else if (luaL_testudata(L, 2, L_MATERIAL))
    {
        CMaterial* mat = *(CMaterial**)lua_touserdata(L, 2);
		mat->Bind(stage);
    }
    else if (lua_gettop(L) == 2) {
        LPDIRECT3DTEXTURE9 handle = (LPDIRECT3DTEXTURE9)lua_touserdata(L, 2);
		RENDERER->SetTexture(stage, handle);
    }
	else
		RENDERER->SetTexture(stage, NULL);

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
LUAF(Rend, GetMatrix)
{
    DWORD kind = (DWORD)luaL_checkinteger(L, 1);
    
	matrix_new(L);
	D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 2, L_MATRIX);
	*mat = RENDERER->GetDeviceMatrix(kind);
    
    return 1;
}
LUAF(Rend, RenderState)
{
	DWORD kind = (DWORD)luaL_checkinteger(L, 1);
	BOOL state = (BOOL)lua_toboolean(L, 2);

	RENDERER->SetRenderState(kind, state);
	return 0;
}
LUAF(Rend, SamplerState)
{
    DWORD stage = (DWORD)luaL_checkinteger(L, 1);
    DWORD kind = (DWORD)luaL_checkinteger(L, 2);
    BOOL state = (BOOL)lua_toboolean(L, 3);

    RENDERER->SetSamplerState(stage, kind, state);
    return 0;
}
LUAF(Rend, EnableLighting)
{
    BOOL state = (BOOL)lua_toboolean(L, 1);

    RENDERER->EnableLighting(state);
    return 0;
}
LUAF(Rend, AmbientColor)
{
    UINT r = 0, g = 0, b = 0;

    if (lua_gettop(L) >= 3)
    {
        r = (UINT)luaL_checknumber(L, 1);
        g = (UINT)luaL_checknumber(L, 2);
        b = (UINT)luaL_checknumber(L, 3);
    }

	RENDERER->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(r, g, b));
    return 0;
}
LUAF(Rend, ClearTarget)
{
	RENDERER->SetRenderTarget(NULL);
    return 0;
}
LUAF(Rend, DrawQuad)
{
	FLOAT x1 = (FLOAT)luaL_checknumber(L, 1);
	FLOAT x2 = (FLOAT)luaL_checknumber(L, 2);
	FLOAT y1 = (FLOAT)luaL_checknumber(L, 3);
	FLOAT y2 = (FLOAT)luaL_checknumber(L, 4);
	DWORD color = (DWORD)luaL_checkinteger(L, 5);
	BOOL flipY = (BOOL)lua_toboolean(L, 6);

	RENDERER->DrawQuad(x1, x2, y1, y2, color, flipY);
    return 0;
}
LUAF(Rend, FillScreen)
{
	DWORD color = 0x00FFFFFF;
	BOOL flipY = FALSE;

	if (lua_gettop(L) >= 1)
		color = (DWORD)lua_tointeger(L, 1);

	if (lua_gettop(L) >= 2)
		flipY = (BOOL)lua_toboolean(L, 2);

	RECT res = RENDERER->GetResolution();
    RENDERER->DrawQuad(0, (FLOAT)res.right, 0, (FLOAT)res.bottom, color, flipY);
    return 0;
}
///<END

VOID CLuaBindings::BindRenderer(lua_State* L)
{
	REGF(Rend, ClearScene);
	REGF(Rend, CameraPerspective);
	REGF(Rend, CameraOrthographic);
	REGF(Rend, GetMatrix);
	REGF(Rend, GetResolution);
	REGF(Rend, RenderState);
	REGF(Rend, SamplerState);
	REGF(Rend, EnableLighting);
	REGF(Rend, AmbientColor);
	REGF(Rend, ClearTarget);
	REGF(Rend, DrawQuad);
	REGF(Rend, FillScreen);

	REGF(Rend, BindTexture);

	LuaVertex_register(L);
	LuaMaterial_register(L);
	LuaLight_register(L);
	LuaMesh_register(L);
	LuaFaceGroup_register(L);
	LuaScene_register(L);
	LuaNode_register(L);
	LuaEffect_register(L);
	LuaRenderTarget_register(L);

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

        REGN(TEXTURESLOT_ALBEDO, TEXTURESLOT_ALBEDO + 1);
        REGN(TEXTURESLOT_SPECULAR, TEXTURESLOT_SPECULAR + 1);
        REGN(TEXTURESLOT_NORMAL, TEXTURESLOT_NORMAL + 1);
        REGN(TEXTURESLOT_DISPLACE, TEXTURESLOT_DISPLACE + 1);
        REGN(TEXTURESLOT_USER_END, TEXTURESLOT_USER_END + 1);
        REGN(MAX_TEXTURE_SLOTS, MAX_TEXTURE_SLOTS + 1);

		// helpers
		REGN(WORLD, MATRIXKIND_WORLD);
		REGN(VIEW, MATRIXKIND_VIEW);
		REGN(PROJ, MATRIXKIND_PROJECTION);


		// render states
#define _X(NAME, VALUE) REGN(NAME, VALUE);
#include "RenderStates.h"
#undef _X

		// light types
		REGN(LIGHTKIND_DIRECTIONAL, D3DLIGHT_DIRECTIONAL);
		REGN(LIGHTKIND_POINT, D3DLIGHT_POINT);
		REGN(LIGHTKIND_SPOT, D3DLIGHT_SPOT);
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

		CHAR c = tolower(str[0]);

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
LUAF(Input, GetMouse)
{
    DWORD code = (DWORD)luaL_checkinteger(L, 1);
    lua_pushboolean(L, INPUT->GetMouse(code));
    return 1;
}
LUAF(Input, GetMouseDown)
{
    DWORD code = (DWORD)luaL_checkinteger(L, 1);
    lua_pushboolean(L, INPUT->GetMouseDown(code));
    return 1;
}
LUAF(Input, GetMouseUp)
{
    DWORD code = (DWORD)luaL_checkinteger(L, 1);
    lua_pushboolean(L, INPUT->GetMouseUp(code));
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
	REGF(Input, GetMouse);
	REGF(Input, GetMouseDown);
	REGF(Input, GetMouseUp);

	REGF(Input, IsCursorVisible);
	REGF(Input, ShowCursor);

	REGF(Input, GetCursorMode);
	REGF(Input, SetCursorMode);
	REGF(Input, ShowCursor);

	// keys
	{
#define _X(NAME, VALUE) REGN(NAME, VALUE)
#include "InputCodes.h"
#undef _X
	}

	// mouse buttons
	{
		REGN(MOUSE_LEFT_BUTTON, CInput::MOUSE_LEFT_BUTTON);
        REGN(MOUSE_MIDDLE_BUTTON, CInput::MOUSE_MIDDLE_BUTTON);
        REGN(MOUSE_RIGHT_BUTTON, CInput::MOUSE_RIGHT_BUTTON);
        REGN(MOUSE_WHEEL_UP, CInput::MOUSE_WHEEL_UP);
        REGN(MOUSE_WHEEL_DOWN, CInput::MOUSE_WHEEL_DOWN);
	}

	// cursor modes
	{
		REGE(CURSORMODE_DEFAULT);
		REGE(CURSORMODE_CENTERED);
		REGE(CURSORMODE_WRAPPED);
	}
}
