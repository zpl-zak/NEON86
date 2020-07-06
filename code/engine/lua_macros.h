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

#define ENDF {0,0}

struct lua_State;
struct D3DXVECTOR4;

ENGINE_API extern D3DXVECTOR4* vector4_ctor(lua_State* L);
ENGINE_API extern D3DXVECTOR4 luaH_getcomps(lua_State* L, UINT offset=0);
ENGINE_API extern DWORD luaH_getcolor(lua_State* L, UINT offset = 0);
ENGINE_API extern struct _D3DCOLORVALUE luaH_getcolorlinear(lua_State* L, UINT offset = 0);

ENGINE_API extern INT matrix_new(lua_State* L);
ENGINE_API extern INT effect_new(lua_State* L);
ENGINE_API extern INT facegroup_new(lua_State* L);
ENGINE_API extern INT font_new(lua_State* L);
ENGINE_API extern INT sound_new(lua_State* L);
ENGINE_API extern INT light_new(lua_State* L);
ENGINE_API extern INT material_new(lua_State* L);
ENGINE_API extern INT mesh_new(lua_State* L);
ENGINE_API extern INT node_new(lua_State* L);
ENGINE_API extern INT rtt_new(lua_State* L);
ENGINE_API extern INT scene_new(lua_State* L);
ENGINE_API extern INT vector4_new(lua_State* L);
ENGINE_API extern INT vertex_new(lua_State* L);
