#pragma once

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#define SAFE_DELETE(memory) if(memory) { delete memory; memory = NULL; }
#define SAFE_DELETE_ARRAY(memory) if(memory) { delete[] memory; memory = NULL; }
#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = NULL; } }

extern FLOAT GetTime(void);


/// zpl
#if defined(ZPL_CONCAT_EX)
#  undef ZPL_CONCAT_EX
#endif
#define ZPL_CONCAT_EX(a,b) a##b

#if defined(ZPL_CONCAT)
#  undef ZPL_CONCAT
#endif
#define ZPL_CONCAT(a,b) ZPL_CONCAT_EX(a,b)

#ifndef ZPL_JOIN_MACROS
#define ZPL_JOIN_MACROS

#define ZPL_JOIN2 ZPL_CONCAT
#define ZPL_JOIN3(a, b, c) ZPL_JOIN2(ZPL_JOIN2(a, b), c)
#define ZPL_JOIN4(a, b, c, d) ZPL_JOIN2(ZPL_JOIN2(ZPL_JOIN2(a, b), c), d)
#define ZPL_JOIN5(a, b, c, d, e) ZPL_JOIN2(ZPL_JOIN2(ZPL_JOIN2(ZPL_JOIN2(a, b), c), d), e)
#define ZPL_JOIN6(a, b, c, d, e, f) ZPL_JOIN2(ZPL_JOIN2(ZPL_JOIN2(ZPL_JOIN2(ZPL_JOIN2(a, b), c), d), e), f)
#endif

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

#define L_MATRIX "Matrix"
#define L_VECTOR3 "Vector3"
#define L_VERTEX "Vertex"
#define L_TEXTURE "Texture"

#define ENDF {0,0}
