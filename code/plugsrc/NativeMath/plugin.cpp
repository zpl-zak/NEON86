// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include <NeonEngine.h>
#include <lua_macros.h>

#pragma comment(lib, "d3d9/d3dx9.lib")

#include <lua/lua.hpp>

#include "math.h"

vec3 vm_popvec(lua_State* L, INT offset = 0)
{
    return *reinterpret_cast<vec3*>(luaL_checkudata(L, offset, L_VECTOR));
}

VOID vm_pushvec(lua_State* L, vec3 vec)
{
    D3DXVECTOR4* vmvec = vector4_ctor(L);
    *vmvec = { vec.x, vec.y, vec.z, 1.0f };
}

NATCOLS(IntersectSphereTriangle) {
    vec3 sc = vm_popvec(L, 1);
    float sr = (FLOAT)luaL_checknumber(L, 2);
    vec3 v = vm_popvec(L, 3);
    vec3 tp0 = vm_popvec(L, 4);
    vec3 tp1 = vm_popvec(L, 5);
    vec3 tp2 = vm_popvec(L, 6);
    float t;
    vec3 p;
    vec3 n;
    bool contact = FALSE;

    contact = intersection_moving_sphere_triangle(sc, sr, v, tp0, tp1, tp2, &t, &p, &n);

    lua_pushboolean(L, contact);
    lua_pushnumber(L, (lua_Number)t);
    vm_pushvec(L, p);
    vm_pushvec(L, n);

    return 4; // ok, t, p, n
}


static const luaL_Reg nativemath[] = {
    NATCOLSDEF(IntersectSphereTriangle),
    ENDF
};

extern "C" INT PLUGIN_API luaopen_nativemath(lua_State* L) {
    luaL_newlib(L, nativemath);
    return 1;
}
