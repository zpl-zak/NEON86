#pragma once

#define LUAF(MODULE, NAME) static int ZPL_JOIN4(l_,MODULE,_,NAME)(lua_State* L)
#define LFN(MODULE, NAME) ZPL_JOIN4(l_,MODULE,_,NAME)
#define REGF(MODULE, NAME) \
	lua_pushcfunction(L, ZPL_JOIN4(l_,MODULE,_,NAME)); \
	lua_setglobal(L, #NAME); \
	lua_settop(L, 0);

#define REGFN(MODULE, ALTNAME, NAME) \
	lua_pushcfunction(L, ZPL_JOIN4(l_,MODULE,_,NAME)); \
	lua_setglobal(L, ALTNAME); \
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
static int NAME(lua_State* L) {\
	CLASS* x = (CLASS*)luaL_checkudata(L, 1, L_CLASS); \
	if (lua_gettop(L) == 2) x->FIELD = (float)luaL_checknumber(L, 2); \
	lua_pushnumber(L, x->FIELD); \
	return 1; }

#define LUAP(L, M, T, O) \
	do { \
		T** wptr = (T**)lua_newuserdata(L, sizeof(T*)); \
		*wptr = O; \
		(*wptr)->AddRef(); \
		luaL_setmetatable(L, M); \
	} while (0);

#define LUAPT(L, M, T, O) \
	do { \
		T* wptr = (T*)lua_newuserdata(L, sizeof(T)); \
		*wptr = *O; \
		luaL_setmetatable(L, M); \
	} while (0);

#define L_MATRIX "Matrix"
#define L_VECTOR "Vector"
#define L_VERTEX "Vertex"
#define L_MATERIAL "Material"
#define L_FACEGROUP "FaceGroup"
#define L_MESH "Mesh"
#define L_SCENE "Scene"
#define L_NODE "Node"
#define L_EFFECT "Effect"
#define L_RENDERTARGET "RenderTarget"
#define L_LIGHT "Light"
#define L_FONT "Font"
#define L_SOUND "Sound"
#define L_MUSIC "Music"

#define ENDF {0,0}

struct lua_State;
struct D3DXVECTOR4;

ENGINE_API extern auto vector4_ctor(lua_State* L) -> D3DXVECTOR4*;
ENGINE_API extern auto luaH_getcomps(lua_State* L, unsigned int offset = 0) -> D3DXVECTOR4;
ENGINE_API extern auto luaH_getcolor(lua_State* L, unsigned int offset = 0) -> DWORD;
ENGINE_API extern auto luaH_getcolorlinear(lua_State* L, unsigned int offset = 0) -> struct _D3DCOLORVALUE;

ENGINE_API extern auto matrix_new(lua_State* L) -> int;
ENGINE_API extern auto effect_new(lua_State* L) -> int;
ENGINE_API extern auto facegroup_new(lua_State* L) -> int;
ENGINE_API extern auto font_new(lua_State* L) -> int;
ENGINE_API extern auto sound_new(lua_State* L) -> int;
ENGINE_API extern auto light_new(lua_State* L) -> int;
ENGINE_API extern auto material_new(lua_State* L) -> int;
ENGINE_API extern auto mesh_new(lua_State* L) -> int;
ENGINE_API extern auto node_new(lua_State* L) -> int;
ENGINE_API extern auto rtt_new(lua_State* L) -> int;
ENGINE_API extern auto scene_new(lua_State* L) -> int;
ENGINE_API extern auto vector4_new(lua_State* L) -> int;
ENGINE_API extern auto vertex_new(lua_State* L) -> int;
