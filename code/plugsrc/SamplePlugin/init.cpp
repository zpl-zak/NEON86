// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include <NeonEngine.h>
#include <lua_macros.h>

#include <lua/lua.hpp>

static INT sampleplugin_test(lua_State* L) {
    lua_pushnumber(L, 62);
    return 1;
}

static INT sampleplugin_interoptest(lua_State* L) {
    RECT windowRes = RENDERER->GetResolution();
    lua_pushnumber(L, windowRes.right);
    lua_pushnumber(L, windowRes.bottom);
    return 2;
}

static INT sampleplugin_randvec(lua_State* L) {
    D3DXVECTOR4* vec = vector4_ctor(L);
    vec->x = (FLOAT)rand();
    vec->y = (FLOAT)rand();
    vec->z = (FLOAT)rand();
    return 1;
}

static const luaL_Reg sampleplugin[] = {
    {"test", sampleplugin_test},
    {"interopTest", sampleplugin_interoptest},
    {"randvec", sampleplugin_randvec},
    ENDF
};

extern "C" INT PLUGIN_API luaopen_sampleplugin(lua_State* L) {
    luaL_newlib(L, sampleplugin);
    lua_pushnumber(L, 42);
    lua_setfield(L, -2, "answer");
    return 1;
}
