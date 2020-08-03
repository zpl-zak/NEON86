#include "stdafx.h"
#include "LuaBindings.h"

#include "Engine.h"
#include "VM.h"
#include "Renderer.h"
#include "Input.h"
#include "FileSystem.h"
#include "Sound.h"

#include <lua/lua.hpp>
#include <sstream>
#include <vector>

inline std::vector<std::string> split(const std::string& str, const std::string& delim)
{
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos - prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}

D3DXVECTOR4 luaH_getcomps(lua_State* L, UINT offset)
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

DWORD luaH_getcolor(lua_State* L, UINT offset)
{
    DWORD color = 0x0;

    if (luaL_testudata(L, 1 + offset, L_VECTOR))
    {
        D3DXVECTOR4* vec = (D3DXVECTOR4*)luaL_checkudata(L, 1+offset, L_VECTOR);
        BYTE col[4] = { (BYTE)(vec->w * 0xFF), (BYTE)(vec->x * 0xFF), (BYTE)(vec->y * 0xFF), (BYTE)(vec->z * 0xFF) };
		color = D3DCOLOR_ARGB(col[0], col[1], col[2], col[3]);
    }
    else if (lua_gettop(L) == 1+offset)
    {
        color = (DWORD)luaL_checkinteger(L, 1+offset);
    }
    else if ((UINT)lua_gettop(L) == 3+offset)
    {
        UINT r = 0, g = 0, b = 0;

        r = (UINT)luaL_checknumber(L, 1+offset);
        g = (UINT)luaL_checknumber(L, 2+offset);
        b = (UINT)luaL_checknumber(L, 3+offset);
        color = D3DCOLOR_ARGB(255, r, g, b);
        lua_remove(L, 2 + offset);
        lua_remove(L, 2 + offset);
    }

	return color;
}

D3DCOLORVALUE luaH_getcolorlinear(lua_State* L, UINT offset)
{
	D3DCOLORVALUE color = { 0.0f, 0.0f, 0.0f, 1.0f };

	if (luaL_testudata(L, 1 + offset, L_VECTOR))
	{
		color = *(D3DCOLORVALUE*)luaL_checkudata(L, 1 + offset, L_VECTOR);
	}
    else if ((UINT)lua_gettop(L) == 1+offset)
    {
        DWORD encodedColor = (DWORD)luaL_checkinteger(L, 1 + offset);
        BYTE r = (BYTE)((encodedColor & 0x00FF0000) >> 16);
		BYTE g = (BYTE)((encodedColor & 0x0000FF00) >> 8);
		BYTE b = (BYTE)((encodedColor & 0x000000FF) >> 0);
		color = { (FLOAT)r / 0xFF, (FLOAT)g / 0xFF, (FLOAT)b / 0xFF, 1.0f};
    }
    else if ((UINT)lua_gettop(L) == 3+offset)
    {
        UINT r = 0, g = 0, b = 0;

        r = (UINT)luaL_checknumber(L, 1 + offset);
        g = (UINT)luaL_checknumber(L, 2 + offset);
        b = (UINT)luaL_checknumber(L, 3 + offset);
        color = { (FLOAT)r / 0xFF, (FLOAT)g / 0xFF, (FLOAT)b / 0xFF, 1.0f };
        lua_remove(L, 2 + offset);
        lua_remove(L, 2 + offset);
    }

    return color;
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
#include "LuaFont.h"
#include "LuaSound.h"

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
	PushLog(CString::Format("%s\n", text).Str());
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
LUAF(Base, IsDebugMode)
{
#ifdef _DEBUG
	lua_pushboolean(L, TRUE);
#else
	lua_pushboolean(L, FALSE);
#endif
	return 1;
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

	FDATA fd = FILESYSTEM->GetResource((LPSTR)scriptName);

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

    FDATA fd = FILESYSTEM->GetResource((LPSTR)scriptName);

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
LUAF(Base, SaveState)
{
	LPCSTR data = (LPCSTR)luaL_checkstring(L, 1);
	size_t len = ::strlen(data);

	LPCSTR out = b64_encode((unsigned char*)data, len);
	len = ::strlen(out);

	FILESYSTEM->SaveResource(out, len);
	neon_free((LPVOID)out);
	return 0;
}
LUAF(Base, LoadState)
{
	FDATA f = FILESYSTEM->GetResource(RESOURCE_UDATA);

	if (f.data == NULL)
	{
		lua_pushnil(L);
		return 1;
	}

	LPCSTR out = (LPCSTR)neon_malloc(f.size+1);
	ZeroMemory((LPVOID)out, f.size+1);

	if (!b64_decode((const char*)f.data, (unsigned char*)out, f.size))
	{
		neon_free((LPVOID)out);
		lua_pushnil(L);
		return 1;
	}

	lua_pushstring(L, out);
	neon_free((LPVOID)out);
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
	REGF(Base, IsDebugMode);
    REGF(Base, RestartGame);
	REGF(Base, SetFPS);
	REGF(Base, dofile);
	REGF(Base, loadfile);
	REGF(Base, SaveState);
	REGF(Base, LoadState);
	REGF(Base, getTime);
	REGFN(Base, "GetTime", getTime);
}

VOID CLuaBindings::BindAudio(lua_State* L)
{
	LuaSound_register(L);
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
LUAF(Math, WorldToScreen)
{
	D3DXVECTOR3* pos3D = (D3DXVECTOR3*)luaL_checkudata(L, 1, L_VECTOR);
    D3DXMATRIX view = *(D3DXMATRIX*)luaL_checkudata(L, 2, L_MATRIX);
    D3DXMATRIX proj = *(D3DXMATRIX*)luaL_checkudata(L, 3, L_MATRIX);
    D3DXMATRIX world;
	D3DXMatrixIdentity(&world);

	D3DXVECTOR3* pos2D = (D3DXVECTOR3*)vector4_ctor(L);
    D3DVIEWPORT9 viewport;
	RENDERER->GetDevice()->GetViewport(&viewport);

    D3DXVec3Project(pos2D, pos3D, &viewport, &proj, &view, &world);
	return 1;
}
LUAF(Math, ScreenToWorld)
{
	D3DXVECTOR3* pos2D = (D3DXVECTOR3*)luaL_checkudata(L, 1, L_VECTOR);
    D3DXMATRIX proj = RENDERER->GetDeviceMatrix(MATRIXKIND_PROJECTION);
    D3DXMATRIX view = RENDERER->GetDeviceMatrix(MATRIXKIND_VIEW);
    D3DXMATRIX world = RENDERER->GetDeviceMatrix(MATRIXKIND_WORLD);

    if (lua_gettop(L) == 4) {
        proj = *(D3DXMATRIX*)luaL_checkudata(L, 2, L_MATRIX);
        view = *(D3DXMATRIX*)luaL_checkudata(L, 3, L_MATRIX);
        world = *(D3DXMATRIX*)luaL_checkudata(L, 4, L_MATRIX);
    }

	D3DXVECTOR3* pos3D = (D3DXVECTOR3*)vector4_ctor(L);
    D3DVIEWPORT9 viewport;
    RENDERER->GetDevice()->GetViewport(&viewport);

    D3DXVec3Unproject(pos2D, pos3D, &viewport, &proj, &view, &world);

	return 1;
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
	REGF(Math, WorldToScreen);
	REGF(Math, ScreenToWorld)
	REGF(Math, ColorLinear);
	REGF(Math, str2vec);
	REGF(Math, vec2str);
}

/// RENDERER METHODS
LUAF(Rend, ClearScene)
{
	RENDERER->ClearBuffer(luaH_getcolor(L), CLEARFLAG_STANDARD);
	return 0;
}
LUAF(Rend, CameraPerspective)
{
	FLOAT fov = (FLOAT)luaL_checknumber(L, 1);
	FLOAT zNear=0.1f, zFar=1000.0f;
	BOOL flipHandedness = FALSE;

	if (lua_gettop(L) >= 3)
	{
		zNear = (FLOAT)luaL_checknumber(L, 2);
		zFar = (FLOAT)luaL_checknumber(L, 3);
	}

	if (lua_gettop(L) >= 4)
	{
		flipHandedness = (BOOL)lua_toboolean(L, 4);
	}

	D3DXMATRIX matProjection;
	RECT res = RENDERER->GetSurfaceResolution();

	if (flipHandedness)
	{
        D3DXMatrixPerspectiveFovRH(&matProjection,
            D3DXToRadian(fov),
            (FLOAT)res.right / (FLOAT)res.bottom,
            zNear,
            zFar);
	}
	else
	{
        D3DXMatrixPerspectiveFovLH(&matProjection,
            D3DXToRadian(fov),
            (FLOAT)res.right / (FLOAT)res.bottom,
            zNear,
            zFar);
	}

	RENDERER->SetMatrix(MATRIXKIND_PROJECTION, matProjection);

	return 0;
}
LUAF(Rend, CameraOrthographic)
{
	RECT res = RENDERER->GetSurfaceResolution();
	FLOAT w=(FLOAT)res.right, h=(FLOAT)res.bottom;
    BOOL flipHandedness = FALSE;

	if (lua_gettop(L) >= 2)
	{
        w = (FLOAT)luaL_checknumber(L, 1) * ((FLOAT)res.right / (FLOAT)res.bottom);
        h = (FLOAT)luaL_checknumber(L, 2);
	}

	FLOAT zNear=0.01f, zFar=100.0f;

	if (lua_gettop(L) >= 4)
	{
		zNear = (FLOAT)luaL_checknumber(L, 3);
		zFar = (FLOAT)luaL_checknumber(L, 4);
	}

    if (lua_gettop(L) >= 5)
    {
        flipHandedness = (BOOL)lua_toboolean(L, 5);
    }

	D3DXMATRIX matProjection;

	if (flipHandedness)
	{
        D3DXMatrixOrthoRH(&matProjection,
            w,
            h,
            zNear,
            zFar);
	}
	else
	{
        D3DXMatrixOrthoLH(&matProjection,
            w,
            h,
            zNear,
            zFar);
	}

	RENDERER->SetMatrix(MATRIXKIND_PROJECTION, matProjection);

	return 0;
}
LUAF(Rend, CameraOrthographicEx)
{
    RECT res = RENDERER->GetSurfaceResolution();
	FLOAT l = 0.0f, t = 0.0f;
    FLOAT r = (FLOAT)res.right, b = (FLOAT)res.bottom;
    BOOL flipHandedness = FALSE;

    if (lua_gettop(L) >= 4)
    {
		l = (FLOAT)luaL_checknumber(L, 1);
		r = (FLOAT)luaL_checknumber(L, 2);
		b = (FLOAT)luaL_checknumber(L, 3);
		t = (FLOAT)luaL_checknumber(L, 4);
    }

    FLOAT zNear = 0.01f, zFar = 100.0f;

    if (lua_gettop(L) >= 5)
    {
        zNear = (FLOAT)luaL_checknumber(L, 5);
        zFar = (FLOAT)luaL_checknumber(L, 6);
    }

    if (lua_gettop(L) >= 7)
    {
        flipHandedness = (BOOL)lua_toboolean(L, 7);
    }

    D3DXMATRIX matProjection;

    if (flipHandedness)
    {
        D3DXMatrixOrthoOffCenterRH(&matProjection,
            l, r, b, t,
            zNear,
            zFar);
    }
    else
    {
        D3DXMatrixOrthoOffCenterLH(&matProjection,
			l, r, b, t,
            zNear,
            zFar);
    }

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
		RENDERER->MarkMaterialOverride(TRUE);
	}
	else if (lua_gettop(L) == 2) {
		LPDIRECT3DTEXTURE9 handle = (LPDIRECT3DTEXTURE9)lua_touserdata(L, 2);
		RENDERER->SetTexture(stage, handle);
	}
	else
	{
		RENDERER->GetDefaultMaterial()->Bind(stage);
		RENDERER->MarkMaterialOverride(FALSE);
	}

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
LUAF(Rend, IsFocused)
{
	lua_pushboolean(L, RENDERER->IsFocused());
    return 1;
}
LUAF(Rend, RenderState)
{
	DWORD kind = (DWORD)luaL_checkinteger(L, 1);
	BOOL state = (BOOL)lua_toboolean(L, 2);

	RENDERER->SetRenderState(kind, state);
	return 0;
}
LUAF(Rend, ToggleDepthTest)
{
	BOOL state = (BOOL)lua_toboolean(L, 1);
	RENDERER->SetRenderState(D3DRS_ZENABLE, state);
	return 0;
}
LUAF(Rend, ToggleWireframe)
{
	BOOL state = (BOOL)lua_toboolean(L, 1);

	RENDERER->SetRenderState(D3DRS_FILLMODE, state ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
	RENDERER->SetRenderState(D3DRS_CULLMODE, state ? D3DCULL_NONE : D3DCULL_CCW);
	RENDERER->SetRenderState(D3DRS_ZENABLE, state ? FALSE : TRUE);
	return 0;
}
LUAF(Rend, SetFog)
{
    DWORD color = luaH_getcolor(L);
    DWORD mode = (DWORD)luaL_checkinteger(L, 2);
	FLOAT start = (FLOAT)luaL_checknumber(L, 3);
	FLOAT end = 0.0f;

	if (mode == D3DFOG_LINEAR)
		end = (FLOAT)luaL_checknumber(L, 4);

	RENDERER->SetFog(color, mode, start, end);
    return 0;
}
LUAF(Rend, ClearFog)
{
	RENDERER->ClearFog();
    return 0;
}
LUAF(Rend, SamplerState)
{
    DWORD stage = (DWORD)luaL_checkinteger(L, 1);
    DWORD kind = (DWORD)luaL_checkinteger(L, 2);
    DWORD state = (DWORD)luaL_checkinteger(L, 3);

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
	if (lua_gettop(L) == 0)
	{
		D3DCOLOR col = 0x0;
		RENDERER->GetDevice()->GetRenderState(D3DRS_AMBIENT, &col);
		lua_pushinteger(L, col);
		return 1;
	}

	RENDERER->SetRenderState(D3DRS_AMBIENT, luaH_getcolor(L));
    return 0;
}
LUAF(Rend, ClearTarget)
{
	RENDERER->SetRenderTarget(NULL);
    return 0;
}
LUAF(Rend, DrawBox)
{
	D3DXMATRIX mat = *(D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
	D3DXVECTOR4 dims = *(D3DXVECTOR4*)luaL_checkudata(L, 2, L_VECTOR);
    DWORD color = (DWORD)luaL_checkinteger(L, 3);

    RENDERER->DrawBox(mat, dims, color);
    return 0;
}
LUAF(Rend, ReadyAlphaBlend)
{
	RENDERER->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    RENDERER->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    RENDERER->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    RENDERER->GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    RENDERER->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0x08);
    RENDERER->GetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    RENDERER->GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    RENDERER->GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    RENDERER->GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    RENDERER->GetDevice()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    RENDERER->GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    RENDERER->GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
    RENDERER->GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
    RENDERER->GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);
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
LUAF(Rend, DrawQuadEx)
{
	D3DXVECTOR3 pos = *(D3DXVECTOR3*)luaL_checkudata(L, 1, L_VECTOR);
	FLOAT w = (FLOAT)luaL_checknumber(L, 2);
	FLOAT h = (FLOAT)luaL_checknumber(L, 3);
	DWORD color = (DWORD)luaL_checkinteger(L, 4);
	BOOL usesDepth = (BOOL)lua_toboolean(L, 5);
	BOOL flipY = (BOOL)lua_toboolean(L, 6);

	RENDERER->DrawQuadEx(pos.x, pos.y, pos.z, w, h, color, usesDepth, flipY);
    return 0;
}
LUAF(Rend, DrawQuad3D)
{
    FLOAT x1 = (FLOAT)luaL_checknumber(L, 1);
    FLOAT x2 = (FLOAT)luaL_checknumber(L, 2);
    FLOAT y1 = (FLOAT)luaL_checknumber(L, 3);
    FLOAT y2 = (FLOAT)luaL_checknumber(L, 4);
    FLOAT z1 = (FLOAT)luaL_checknumber(L, 5);
    FLOAT z2 = (FLOAT)luaL_checknumber(L, 6);
    DWORD color = (DWORD)luaL_checkinteger(L, 7);

    RENDERER->DrawQuad3D(x1, x2, y1, y2, z1, z2, color);
    return 0;
}
LUAF(Rend, DrawPolygon)
{
    VERTEX a = *(VERTEX*)luaL_checkudata(L, 1, L_VERTEX);
	VERTEX b = *(VERTEX*)luaL_checkudata(L, 2, L_VERTEX);
	VERTEX c = *(VERTEX*)luaL_checkudata(L, 3, L_VERTEX);

    RENDERER->DrawPolygon(a, b, c);
    return 0;
}
LUAF(Rend, CullMode)
{
	UINT mode = (UINT)luaL_checkinteger(L, 1);
	RENDERER->SetRenderState(D3DRS_CULLMODE, mode);
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
LUAF(Rend, RegisterFontFile)
{
	LPCSTR path = (LPCSTR)luaL_checkstring(L, 1);
	lua_pushboolean(L, CFont::AddFontToDatabase(path));
	return 1;
}
///<END

VOID CLuaBindings::BindRenderer(lua_State* L)
{
	REGF(Rend, ClearScene);
	REGF(Rend, CameraPerspective);
	REGF(Rend, CameraOrthographic);
	REGF(Rend, CameraOrthographicEx);
	REGF(Rend, GetMatrix);
	REGF(Rend, GetResolution);
	REGF(Rend, IsFocused);
	REGF(Rend, RenderState);
	REGF(Rend, SetFog);
	REGF(Rend, ClearFog);
	REGF(Rend, ReadyAlphaBlend)
	REGF(Rend, SamplerState);
	REGF(Rend, ToggleWireframe);
	REGF(Rend, ToggleDepthTest);
	REGF(Rend, EnableLighting);
	REGF(Rend, CullMode);
	REGF(Rend, AmbientColor);
	REGF(Rend, ClearTarget);
	REGF(Rend, DrawBox)
	REGF(Rend, DrawQuad);
	REGF(Rend, DrawQuadEx)
	REGF(Rend, DrawQuad3D);
	REGF(Rend, DrawPolygon);
	REGF(Rend, FillScreen);
	REGF(Rend, RegisterFontFile);

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
	LuaFont_register(L);

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

		REGN(FOGKIND_NONE, 0);
		REGN(FOGKIND_EXP, 1);
		REGN(FOGKIND_EXP2, 2);
		REGN(FOGKIND_LINEAR, 3);

		// helpers
		REGN(WORLD, MATRIXKIND_WORLD);
		REGN(VIEW, MATRIXKIND_VIEW);
		REGN(PROJ, MATRIXKIND_PROJECTION);

        REGN(CULLKIND_NONE, 1);
        REGN(CULLKIND_CW, 2);
        REGN(CULLKIND_CCW, 3);

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
