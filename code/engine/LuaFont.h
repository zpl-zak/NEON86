#pragma once

#include "system.h"
#include "Font.h"

#include <lua/lua.hpp>

auto font_new(lua_State* L) -> int
{
    const auto fontPath = luaL_checkstring(L, 1);
    const auto fontSize = static_cast<int>(luaL_checkinteger(L, 2));
    auto boldness = 0;
    auto italic = FALSE;

    if (lua_gettop(L) >= 3)
        boldness = static_cast<int>(luaL_checkinteger(L, 3));

    if (lua_gettop(L) >= 4)
        italic = lua_toboolean(L, 4);

    *static_cast<CFont**>(lua_newuserdata(L, sizeof(CFont*))) = new CFont(fontPath, fontSize, boldness, italic);

    luaL_setmetatable(L, L_FONT);
    return 1;
}

static auto font_drawtext(lua_State* L) -> int
{
    auto font = *static_cast<CFont**>(luaL_checkudata(L, 1, L_FONT));
    const auto color = static_cast<DWORD>(luaL_checkinteger(L, 2));
    const auto text = static_cast<LPCSTR>(luaL_checkstring(L, 3));
    const auto x = static_cast<unsigned int>(luaL_checkinteger(L, 4));
    const auto y = static_cast<unsigned int>(luaL_checkinteger(L, 5));

    unsigned int w = 0, h = 0;
    if (lua_gettop(L) >= 6)
    {
        w = static_cast<unsigned int>(luaL_checkinteger(L, 6));
        h = static_cast<unsigned int>(luaL_checkinteger(L, 7));
    }

    DWORD flags = DT_WORDBREAK;
    if (lua_gettop(L) >= 8)
    {
        flags = static_cast<DWORD>(luaL_checkinteger(L, 8));
    }

    font->RenderText(color, text, x, y, w, h, flags);

    return 1;
}

static auto font_measuretext(lua_State* L) -> int
{
    auto font = *static_cast<CFont**>(luaL_checkudata(L, 1, L_FONT));
    const auto text = static_cast<LPCSTR>(luaL_checkstring(L, 2));
    const auto flags = static_cast<DWORD>(luaL_checkinteger(L, 3));

    RECT rect = {0};
    if (lua_gettop(L) >= 4)
    {
        rect.right = static_cast<unsigned int>(luaL_checkinteger(L, 4));
    }

    font->CalculateRect(text, &rect, flags);

    lua_newtable(L);
    lua_pushinteger(L, 1);
    lua_pushnumber(L, rect.right);
    lua_settable(L, -3);
    lua_pushinteger(L, 2);
    lua_pushnumber(L, rect.bottom);
    lua_settable(L, -3);

    return 1;
}

static auto font_delete(lua_State* L) -> int
{
    auto font = *static_cast<CFont**>(luaL_checkudata(L, 1, L_FONT));

    font->Release();
    return 0;
}

static void LuaFont_register(lua_State* L)
{
    lua_register(L, L_FONT, font_new);
    luaL_newmetatable(L, L_FONT);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    REGC("drawText", font_drawtext);
    REGC("measureText", font_measuretext);

    REGC("__gc", font_delete);

    lua_pop(L, 1);
}
