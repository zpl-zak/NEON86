#pragma once

#include "system.h"

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
		w = (UINT)luaL_checknumber(L, 1);
		h = (UINT)luaL_checknumber(L, 2);
	}

	LPDIRECT3DDEVICE9 dev = RENDERER->GetDevice();
	LPDIRECT3DTEXTURE9 tex = NULL; 
	
	HRESULT ok = S_FALSE;

	if (!texName)
		ok = D3DXCreateTexture(dev, w, h, 0, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &tex);
	else
	{
		FDATA img = FILESYSTEM->GetResource(RESOURCEKIND_IMAGE, texName);
		if (!img.data)
		{
			MessageBoxA(NULL, "Image not found!", "Resource error", MB_OK);
			ENGINE->Shutdown();
			return 0;
		}
		ok = D3DXCreateTextureFromFileInMemory(dev, img.data, img.size, &tex);
	}

	*(LPDIRECT3DTEXTURE9*)(lua_newuserdata(L, sizeof(LPDIRECT3DTEXTURE9*))) = tex;
	
	luaL_setmetatable(L, L_TEXTURE);
	return ok == D3D_OK;
}

static INT texture_delete(lua_State* L)
{
	LPDIRECT3DTEXTURE9 *tex = (LPDIRECT3DTEXTURE9*)luaL_checkudata(L, 1, L_TEXTURE);

	if ((*(LPDIRECT3DTEXTURE9*)tex))
		(*(LPDIRECT3DTEXTURE9*)tex)->Release();
	return 0;
}

static VOID LuaTexture_register(lua_State* L)
{
	lua_register(L, L_TEXTURE, texture_new);
	luaL_newmetatable(L, L_TEXTURE);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
	
	lua_pushcfunction(L, texture_delete); lua_setfield(L, -2, "__gc");
	lua_pop(L, 1);
}
