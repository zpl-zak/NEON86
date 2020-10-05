#include "pch.h"

static const luaL_Reg bullet[] = {
    ENDF
};

extern "C" INT PLUGIN_API luaopen_bullet(lua_State* L) {
    luaL_newlib(L, bullet);
    return 1;
}
