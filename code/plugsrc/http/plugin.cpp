// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include <NeonEngine.h>
#include <lua_macros.h>

#include <lua/lua.hpp>

static const luaL_Reg http[] = {
    ENDF
};

extern "C" INT PLUGIN_API luaopen_http(lua_State* L) {
    luaL_newlib(L, http);
    return 1;
}
