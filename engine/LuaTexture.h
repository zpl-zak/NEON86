#pragma once

#include "system.h"
#include "Texture.h"

#include <lua/lua.hpp>

// todo use CTexture

static INT texture_new(lua_State* L)
{
	char* texName = NULL;
	UINT w=1, h=1;

	if (lua_gettop(L) == 1)
		texName = (char *)luaL_checkstring(L, 1);
	else if (lua_gettop(L) == 2)
	{
		w = (UINT)luaL_checkinteger(L, 1);
		h = (UINT)luaL_checkinteger(L, 2);
	}

	CTexture* tex = (CTexture*)lua_newuserdata(L, sizeof(CTexture));
	*tex = CTexture(texName, w, h);
	
	luaL_setmetatable(L, L_TEXTURE);
	return tex->GetTextureHandle() != NULL;
}

static INT texture_setsampler(lua_State* L)
{
    CTexture* tex = (CTexture*)luaL_checkudata(L, 1, L_TEXTURE);
	UINT sampler = (UINT)luaL_checkinteger(L, 2);
	UINT val = (UINT)luaL_checkinteger(L, 3);

	tex->SetSamplerState(sampler, val);

    return 0;
}

static INT texture_getsampler(lua_State* L)
{
    CTexture* tex = (CTexture*)luaL_checkudata(L, 1, L_TEXTURE);
    UINT sampler = (UINT)luaL_checkinteger(L, 2);
    

    lua_pushinteger(L, tex->GetSamplerState(sampler));
    return 1;
}

static INT texture_delete(lua_State* L)
{
	CTexture *tex = (CTexture*)luaL_checkudata(L, 1, L_TEXTURE);

	tex->Release();
	return 0;
}

static VOID LuaTexture_register(lua_State* L)
{
	lua_register(L, L_TEXTURE, texture_new);
	luaL_newmetatable(L, L_TEXTURE);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	
	lua_pushcfunction(L, texture_setsampler); lua_setfield(L, -2, "setSamplerState");
	lua_pushcfunction(L, texture_getsampler); lua_setfield(L, -2, "getSamplerState");

	lua_pushcfunction(L, texture_delete); lua_setfield(L, -2, "__gc");
	lua_pop(L, 1);
}
