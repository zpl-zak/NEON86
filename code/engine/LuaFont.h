#pragma once

#include "system.h"
#include "Font.h"

#include <lua/lua.hpp>

auto font_new(lua_State* L) -> int
{
    const auto* const fontFamily = LuaGetInline<LPCSTR>(L);
    const auto fontSize = LuaGetInline<int>(L);
    auto boldness = 0;
    auto italic = FALSE;

    if (LuaLength(L) >= 1)
        boldness = LuaGetInline<int>(L);

    if (LuaLength(L) >= 1)
        italic = LuaGetInline<bool>(L);

    *static_cast<CFont**>(lua_newuserdata(L, sizeof(CFont*))) = new CFont(fontFamily, fontSize, boldness, italic);

    luaL_setmetatable(L, L_FONT);
    return 1;
}

static auto font_drawtext(lua_State* L) -> int
{
    auto* const font = LuaGetInline<CFont*>(L);
    const auto color = LuaGetInline<DWORD>(L);
    const auto* const text = LuaGetInline<LPCSTR>(L);
    const auto x = LuaGetInline<uint32_t>(L);
    const auto y = LuaGetInline<uint32_t>(L);

    uint32_t w = 0, h = 0;
    if (LuaLength(L) >= 2)
    {
        w = LuaGetInline<uint32_t>(L);
        h = LuaGetInline<uint32_t>(L);
    }

    DWORD flags = DT_WORDBREAK;
    if (LuaLength(L) >= 1)
    {
        flags = LuaGetInline<DWORD>(L);
    }

    font->RenderText(color, text, x, y, w, h, flags);

    return 1;
}

static auto font_measuretext(lua_State* L) -> int
{
    auto* const font = LuaGetInline<CFont*>(L);
    const auto* const text = LuaGetInline<LPCSTR>(L);
    const auto flags = LuaGetInline<DWORD>(L);

    RECT rect = {0};
    if (LuaLength(L) >= 1)
    {
        rect.right = LuaGetInline<uint32_t>(L);
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
    auto* const font = LuaGetInline<CFont*>(L);

    font->Release();
    return 0;
}

static void LuaFont$Register(lua_State* L)
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
