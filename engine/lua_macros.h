#pragma once

#define LUAF(MODULE, NAME) static INT ZPL_JOIN4(l_,MODULE,_,NAME)(lua_State* L)
#define LFN(MODULE, NAME) ZPL_JOIN4(l_,MODULE,_,NAME)
#define REGF(MODULE, NAME) \
	lua_pushcfunction(L, ZPL_JOIN4(l_,MODULE,_,NAME)); \
	lua_setglobal(L, #NAME); \
	lua_settop(L, 0);

#define REGE(VALUE) \
	lua_pushnumber(L, VALUE); \
	lua_setglobal(L, #VALUE); \
	lua_settop(L, 0);

#define REGN(NAME, VALUE) \
	lua_pushinteger(L, VALUE); \
	lua_setglobal(L, #NAME); \
	lua_settop(L, 0);

#define REGC(NAME, FUNC) \
	lua_pushcfunction(L, FUNC); \
	lua_setfield(L, -2, NAME);

#define LUAS(CLASS, L_CLASS, NAME, FIELD) \
static INT NAME(lua_State* L) {\
	CLASS* x = (CLASS*)luaL_checkudata(L, 1, L_CLASS); \
	if (lua_gettop(L) == 2) x->FIELD = (FLOAT)luaL_checknumber(L, 2); \
	lua_pushnumber(L, x->FIELD); \
	return 1; }


#define L_MATRIX "Matrix"
#define L_VECTOR3 "Vector3"
#define L_VERTEX "Vertex"
#define L_MATERIAL "Material"
#define L_MESH "Mesh"
#define L_MESHGROUP "MeshGroup"
#define L_MODEL "Model"
#define L_EFFECT "Effect"

#define ENDF {0,0}
