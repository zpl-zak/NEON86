#include "pch.h"

#include <NeonEngine.h>
#include <lua_macros.h>

#include <lua/lua.hpp>

static const luaL_Reg net[] = {
    ENDF
};

extern "C" INT PLUGIN_API luaopen_net(lua_State * L) {
    luaL_newlib(L, net);
    return 1;
}
