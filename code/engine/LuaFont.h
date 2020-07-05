#pragma once

#include "system.h"
#include "Font.h"

#include <lua/lua.hpp>

INT font_new(lua_State* L)
{
    LPCSTR fontPath = luaL_checkstring(L, 1);
    INT fontSize = (INT)luaL_checkinteger(L, 2);
    INT boldness = 0;
    BOOL italic = FALSE;

    if (lua_gettop(L) >= 3)
        boldness = (INT)luaL_checkinteger(L, 3);

    if (lua_gettop(L) >= 4)
        italic = lua_toboolean(L, 4);

    *(CFont**)lua_newuserdata(L, sizeof(CFont*)) = new CFont(fontPath, fontSize, boldness, italic);

    luaL_setmetatable(L, L_FONT);
    return 1;
}

static INT font_drawtext(lua_State* L)
{
    CFont* font = *(CFont**)luaL_checkudata(L, 1, L_FONT);
    DWORD color = (DWORD)luaL_checkinteger(L, 2);
    LPCSTR text = (LPCSTR)luaL_checkstring(L, 3);
    UINT x = (UINT)luaL_checkinteger(L, 4);
    UINT y = (UINT)luaL_checkinteger(L, 5);

    UINT w = 0, h = 0;
    if (lua_gettop(L) >= 6)
    {
        w = (UINT)luaL_checkinteger(L, 6);
        h = (UINT)luaL_checkinteger(L, 7);
    }

    font->RenderText(color, text, x, y, w, h);
    return 0;
}

static INT font_delete(lua_State* L)
{
    CFont* font = *(CFont**)luaL_checkudata(L, 1, L_FONT);

    font->Release();
    return 0;
}

static VOID LuaFont_register(lua_State* L)
{
    lua_register(L, L_FONT, font_new);
    luaL_newmetatable(L, L_FONT);
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");

    REGC("drawText", font_drawtext);

    REGC("__gc", font_delete);

    lua_pop(L, 1);
}
