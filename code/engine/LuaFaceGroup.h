#pragma once

#include "system.h"

#include <lua/lua.hpp>

#include "FaceGroup.h"

auto facegroup_new(lua_State* L) -> int
{
    *static_cast<CFaceGroup**>(lua_newuserdata(L, sizeof(CFaceGroup*))) = new CFaceGroup();

    luaL_setmetatable(L, L_FACEGROUP);
    return 1;
}

static auto facegroup_clone(lua_State* L) -> int
{
    auto mesh = *static_cast<CFaceGroup**>(luaL_checkudata(L, 1, L_FACEGROUP));
    *static_cast<CFaceGroup**>(lua_newuserdata(L, sizeof(CFaceGroup*))) = mesh->Clone();

    luaL_setmetatable(L, L_FACEGROUP);
    return 1;
}

static auto facegroup_addvertex(lua_State* L) -> int
{
    auto mesh = *static_cast<CFaceGroup**>(luaL_checkudata(L, 1, L_FACEGROUP));
    const auto vert = static_cast<VERTEX*>(luaL_checkudata(L, 2, L_VERTEX));
    mesh->AddVertex(*vert);

    lua_pushvalue(L, 1);
    return 1;
}

static auto facegroup_addindex(lua_State* L) -> int
{
    auto mesh = *static_cast<CFaceGroup**>(luaL_checkudata(L, 1, L_FACEGROUP));
    const auto index = static_cast<short>(luaL_checkinteger(L, 2));
    mesh->AddIndex(index);

    lua_pushvalue(L, 1);
    return 1;
}

static auto facegroup_addtriangle(lua_State* L) -> int
{
    auto mesh = *static_cast<CFaceGroup**>(luaL_checkudata(L, 1, L_FACEGROUP));
    const auto i1 = static_cast<short>(luaL_checkinteger(L, 2));
    const auto i2 = static_cast<short>(luaL_checkinteger(L, 3));
    const auto i3 = static_cast<short>(luaL_checkinteger(L, 4));

    mesh->AddIndex(i1);
    mesh->AddIndex(i2);
    mesh->AddIndex(i3);

    lua_pushvalue(L, 1);
    return 1;
}

static auto facegroup_setmaterial(lua_State* L) -> int
{
    auto mesh = *static_cast<CFaceGroup**>(luaL_checkudata(L, 1, L_FACEGROUP));
    CMaterial* mat = nullptr;

    if (lua_gettop(L) == 2)
    {
        mat = *static_cast<CMaterial**>(luaL_checkudata(L, 2, L_MATERIAL));
        mat->AddRef();
    }

    mesh->SetMaterial(mat ? mat : nullptr);

    lua_pushvalue(L, 1);
    return 1;
}

static auto facegroup_getmaterial(lua_State* L) -> int
{
    const auto fgroup = *static_cast<CFaceGroup**>(luaL_checkudata(L, 1, L_FACEGROUP));
    const auto mat = fgroup->GetMaterial();

    if (mat) { LUAP(L, L_MATERIAL, CMaterial, mat); }
    else lua_pushnil(L);

    return 1;
}

static auto facegroup_draw(lua_State* L) -> int
{
    auto mesh = *static_cast<CFaceGroup**>(luaL_checkudata(L, 1, L_FACEGROUP));
    auto mat = &D3DXMATRIX();

    if (lua_gettop(L) >= 2)
    {
        mat = static_cast<D3DXMATRIX*>(luaL_checkudata(L, 2, L_MATRIX));
    }
    else mat = nullptr;

    mesh->Draw(mat);

    lua_pushvalue(L, 1);
    return 1;
}

static auto facegroup_build(lua_State* L) -> int
{
    auto mesh = *static_cast<CFaceGroup**>(luaL_checkudata(L, 1, L_FACEGROUP));

    mesh->Build();

    lua_pushvalue(L, 1);
    return 1;
}

static auto facegroup_clear(lua_State* L) -> int
{
    auto mesh = *static_cast<CFaceGroup**>(luaL_checkudata(L, 1, L_FACEGROUP));

    mesh->Clear();

    return 0;
}

static auto facegroup_calcnormals(lua_State* L) -> int
{
    auto mesh = *static_cast<CFaceGroup**>(luaL_checkudata(L, 1, L_FACEGROUP));

    mesh->CalculateNormals();

    return 0;
}

static auto facegroup_delete(lua_State* L) -> int
{
    auto mesh = *static_cast<CFaceGroup**>(luaL_checkudata(L, 1, L_FACEGROUP));

    mesh->Release();

    return 0;
}

static auto facegroup_getvertices(lua_State* L) -> int
{
    const auto mesh = *static_cast<CFaceGroup**>(luaL_checkudata(L, 1, L_FACEGROUP));

    lua_newtable(L);

    for (unsigned int i = 0; i < mesh->GetNumVertices(); i++)
    {
        const auto vert = mesh->GetVertices() + i;
        lua_pushinteger(L, i + 1ULL);
        LUAPT(L, L_VERTEX, VERTEX, vert);
        lua_settable(L, -3);
    }

    return 1;
}

static auto facegroup_getbounds(lua_State* L) -> int
{
    auto mesh = *static_cast<CFaceGroup**>(luaL_checkudata(L, 1, L_FACEGROUP));
    const auto b = mesh->GetBounds();

    lua_newtable(L);

    lua_pushinteger(L, 1);
    const auto minVec = vector4_ctor(L);
    *minVec = b[0];
    lua_settable(L, -3);

    lua_pushinteger(L, 2);
    const auto maxVec = vector4_ctor(L);
    *maxVec = b[1];
    lua_settable(L, -3);

    return 1;
}

static auto facegroup_getindices(lua_State* L) -> int
{
    const auto mesh = *static_cast<CFaceGroup**>(luaL_checkudata(L, 1, L_FACEGROUP));

    lua_newtable(L);

    for (unsigned int i = 0; i < mesh->GetNumIndices(); i++)
    {
        const auto index = *(mesh->GetIndices() + i);
        lua_pushinteger(L, i + 1ULL);
        lua_pushinteger(L, index);
        lua_settable(L, -3);
    }

    return 1;
}

static void LuaFaceGroup_register(lua_State* L)
{
    lua_register(L, L_FACEGROUP, facegroup_new);
    luaL_newmetatable(L, L_FACEGROUP);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    REGC("clone", facegroup_clone);
    REGC("addVertex", facegroup_addvertex);
    REGC("addIndex", facegroup_addindex);
    REGC("addTriangle", facegroup_addtriangle);
    REGC("setMaterial", facegroup_setmaterial);
    REGC("getMaterial", facegroup_getmaterial);
    REGC("getBounds", facegroup_getbounds);
    REGC("draw", facegroup_draw);
    REGC("build", facegroup_build);
    REGC("calcNormals", facegroup_calcnormals);
    REGC("clear", facegroup_clear);

    REGC("getVertices", facegroup_getvertices);
    REGC("getIndices", facegroup_getindices);

    REGC("__gc", facegroup_delete);

    lua_pop(L, 1);
}
