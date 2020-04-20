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
	*mat = CMaterial(matName, w, h);
	
	luaL_setmetatable(L, L_MATERIAL);
	return mat->GetTextureHandle() != NULL;
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

    REGC("setDiffuse", material_setdiffuse);
    REGC("setAmbient", material_setambient);
    REGC("setSpecular", material_setspecular);
    REGC("setEmission", material_setemission);
    REGC("setPower", material_setpower);
    REGC("setOpacity", material_setopacity);

    REGC("__gc", material_delete);

	lua_pop(L, 1);
}
