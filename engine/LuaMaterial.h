#pragma once

#include "system.h"
#include "Material.h"

#include <lua/lua.hpp>

static INT material_new(lua_State* L)
{
	char* matName = NULL;
	UINT w=1, h=1;

	if (lua_gettop(L) == 1)
		matName = (char *)luaL_checkstring(L, 1);
	else if (lua_gettop(L) == 2)
	{
		w = (UINT)luaL_checkinteger(L, 1);
		h = (UINT)luaL_checkinteger(L, 2);
	}

	CMaterial* mat = (CMaterial*)lua_newuserdata(L, sizeof(CMaterial));

    if (matName)
        *mat = CMaterial(TEXTURESLOT_ALBEDO, matName);
    else if (lua_gettop(L) == 2)
        *mat = CMaterial(TEXTURESLOT_ALBEDO, w, h);
    else
        *mat = CMaterial();
	
	luaL_setmetatable(L, L_MATERIAL);
	return 1;
}

static INT material_loadfile(lua_State* L)
{
    CMaterial* mat = (CMaterial*)luaL_checkudata(L, 1, L_MATERIAL);
    LPCSTR texName = (LPCSTR)luaL_checkstring(L, 2);
    UINT userSlot = (UINT)luaL_checkinteger(L, 3) - 1;

    mat->CreateTextureForSlot(userSlot, (LPSTR)texName);

    return 0;
}

static INT material_getres(lua_State* L)
{
    CMaterial* mat = (CMaterial*)luaL_checkudata(L, 1, L_MATERIAL);
    UINT userSlot = (UINT)luaL_checkinteger(L, 3) - 1;
    LPDIRECT3DTEXTURE9 h = mat->GetUserTextureHandle(userSlot);
    D3DSURFACE_DESC a;

    h->GetLevelDesc(0, &a);
    
    lua_newtable(L);

    lua_pushinteger(L, 1);
    lua_pushinteger(L, a.Width);
    lua_settable(L, 3);

    lua_pushinteger(L, 2);
    lua_pushinteger(L, a.Height);
    lua_settable(L, 3);

    lua_pushvalue(L, 3);

    return 1;
}

static INT material_loaddata(lua_State* L)
{
    CMaterial* mat = (CMaterial*)luaL_checkudata(L, 1, L_MATERIAL);
    UINT userSlot = (UINT)luaL_checkinteger(L, 3) - 1;
    UINT width = (UINT)luaL_checkinteger(L, 4);
    UINT height = (UINT)luaL_checkinteger(L, 5);
    
    mat->CreateTextureForSlot(userSlot, NULL, width, height);
    //mat->UploadARGB(userSlot, 2, 2);

    return 0;
}

static INT material_getdata(lua_State* L)
{
    CMaterial* mat = (CMaterial*)luaL_checkudata(L, 1, L_MATERIAL);
    UINT userSlot = (UINT)luaL_checkinteger(L, 3) - 1;
    mat->GetUserTextureHandle(userSlot);
    D3DSURFACE_DESC a;
    mat->GetUserTextureHandle(userSlot)->GetLevelDesc(0, &a);

    UINT* buf = (UINT*)mat->Lock(userSlot);

    lua_newtable(L);

    for (UINT i = 0; i < (a.Width * a.Height); i++)
    {
        lua_pushinteger(L, i + 1ULL);
        lua_pushinteger(L, buf[i]);
        lua_settable(L, 3);
    }

    mat->Unlock(userSlot);

    lua_pushvalue(L, 3);

    return 1;
}

static INT material_setsampler(lua_State* L)
{
    CMaterial* mat = (CMaterial*)luaL_checkudata(L, 1, L_MATERIAL);
	UINT sampler = (UINT)luaL_checkinteger(L, 2);
	UINT val = (UINT)luaL_checkinteger(L, 3);

	mat->SetSamplerState(sampler, val);

    return 0;
}

static INT material_getsampler(lua_State* L)
{
    CMaterial* mat = (CMaterial*)luaL_checkudata(L, 1, L_MATERIAL);
    UINT sampler = (UINT)luaL_checkinteger(L, 2);
    
    lua_pushinteger(L, mat->GetSamplerState(sampler));
    return 1;
}

static INT material_gethandle(lua_State* L)
{
    CMaterial* mat = (CMaterial*)luaL_checkudata(L, 1, L_MATERIAL);
    UINT slot = (UINT)luaL_checkinteger(L, 2) - 1;

    lua_pushlightuserdata(L, (VOID*)mat->GetTextureHandle(slot));
    return 1;
}

static INT material_sethandle(lua_State* L)
{
    CMaterial* mat = (CMaterial*)luaL_checkudata(L, 1, L_MATERIAL);
    UINT slot = (UINT)luaL_checkinteger(L, 2) - 1;
    LPDIRECT3DTEXTURE9 handle = (LPDIRECT3DTEXTURE9)lua_touserdata(L, 3);

    mat->SetUserTexture(slot, handle);
    return 1;
}

static INT material_delete(lua_State* L)
{
	CMaterial* mat = (CMaterial*)luaL_checkudata(L, 1, L_MATERIAL);

	mat->Release();
	return 0;
}

static INT material_setdiffuse(lua_State* L)
{
    CMaterial* mat = (CMaterial*)luaL_checkudata(L, 1, L_MATERIAL);
    D3DCOLORVALUE color = { (FLOAT)luaL_checknumber(L, 2), (FLOAT)luaL_checknumber(L, 3), (FLOAT)luaL_checknumber(L, 4) };
    mat->SetDiffuse(color);

    return 0;
}

static INT material_setambient(lua_State* L)
{
    CMaterial* mat = (CMaterial*)luaL_checkudata(L, 1, L_MATERIAL);
    D3DCOLORVALUE color = { (FLOAT)luaL_checknumber(L, 2), (FLOAT)luaL_checknumber(L, 3), (FLOAT)luaL_checknumber(L, 4) };
    mat->SetAmbient(color);

    return 0;
}

static INT material_setemission(lua_State* L)
{
    CMaterial* mat = (CMaterial*)luaL_checkudata(L, 1, L_MATERIAL);
    D3DCOLORVALUE color = { (FLOAT)luaL_checknumber(L, 2), (FLOAT)luaL_checknumber(L, 3), (FLOAT)luaL_checknumber(L, 4) };
    mat->SetEmission(color);

    return 0;
}

static INT material_setspecular(lua_State* L)
{
    CMaterial* mat = (CMaterial*)luaL_checkudata(L, 1, L_MATERIAL);
    D3DCOLORVALUE color = { (FLOAT)luaL_checknumber(L, 2), (FLOAT)luaL_checknumber(L, 3), (FLOAT)luaL_checknumber(L, 4) };
    mat->SetSpecular(color);

    return 0;
}

static INT material_setpower(lua_State* L)
{
    CMaterial* mat = (CMaterial*)luaL_checkudata(L, 1, L_MATERIAL);
    FLOAT val = (FLOAT)luaL_checknumber(L, 2);
    mat->SetPower(val);

    return 0;
}

static INT material_setopacity(lua_State* L)
{
    CMaterial* mat = (CMaterial*)luaL_checkudata(L, 1, L_MATERIAL);
    FLOAT val = (FLOAT)luaL_checknumber(L, 2);
    mat->SetOpacity(val);

    return 0;
}

static VOID LuaMaterial_register(lua_State* L)
{
	lua_register(L, L_MATERIAL, material_new);
	luaL_newmetatable(L, L_MATERIAL);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	
    REGC("setSamplerState", material_setsampler);
    REGC("getSamplerState", material_getsampler);

    REGC("loadFile", material_loadfile);
    REGC("loadData", material_loaddata);
    REGC("res", material_getres);
    REGC("data", material_getdata);
    REGC("getHandle", material_gethandle);
    REGC("setHandle", material_sethandle);

    REGC("setDiffuse", material_setdiffuse);
    REGC("setAmbient", material_setambient);
    REGC("setSpecular", material_setspecular);
    REGC("setEmission", material_setemission);
    REGC("setPower", material_setpower);
    REGC("setOpacity", material_setopacity);

    REGC("__gc", material_delete);

	lua_pop(L, 1);
}
