// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#define PLUGIN_EXPORTS

#include <NeonEngine.h>
#include <lua_macros.h>

#include <lua/lua.hpp>

static INT musiclib_test(lua_State* L) {
    lua_pushnumber(L, 62);
    return 1;
}

static const luaL_Reg musiclib[] = {
    {"test", musiclib_test},
    ENDF
};

extern "C" INT PLUGIN_API luaopen_musiclib(lua_State * L) {
    luaL_newlib(L, musiclib);
    return 1;
}
