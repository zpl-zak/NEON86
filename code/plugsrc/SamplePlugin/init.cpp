// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include <NeonEngine.h>
#include <lua_macros.h>

#include <lua/lua.hpp>

static INT sampleplugin_test(lua_State* L) {
    lua_pushnumber(L, 62);
    return 1;
}

static const luaL_Reg sampleplugin[] = {
    {"test", sampleplugin_test},
    ENDF
};

extern "C" INT PLUGIN_API luaopen_sampleplugin(lua_State* L) {
    luaL_newlib(L, sampleplugin);
    lua_pushnumber(L, 42);
    lua_setfield(L, -2, "answer");
    return 1;
}
