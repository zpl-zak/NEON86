#pragma once

#include "system.h"

struct lua_State;

class ENGINE_API CLuaBindings
{
public:
	static VOID BindBase(lua_State*);
	static VOID BindAudio(lua_State*);
	static VOID BindMath(lua_State*);
	static VOID BindRenderer(lua_State*);
	static VOID BindInput(lua_State*);
};
