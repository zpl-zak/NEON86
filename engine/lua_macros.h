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
	lua_setfield(L, -2, #NAME);

#define L_MATRIX "Matrix"
#define L_VECTOR3 "Vector3"
#define L_VERTEX "Vertex"
#define L_TEXTURE "Texture"
#define L_MESH "Mesh"
#define L_MESHGROUP "MeshGroup"

#define ENDF {0,0}
