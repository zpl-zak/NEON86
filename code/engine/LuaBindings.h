#pragma once

#include "system.h"

struct lua_State;

class ENGINE_API CLuaBindings
{
public:
    static void BindBase(lua_State*);
    static void BindAudio(lua_State*);
    static void BindMath(lua_State*);
    static void BindRenderer(lua_State*);
    static void BindInput(lua_State*);
};
